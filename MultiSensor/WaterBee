#include "ESP8266WiFi.h"
#include "secrets.h"
#include "ESP8266WebServer.h"
#include <ArduinoJson.h>

String Hostname = "WaterBee";

// Define LED
// LED Wifi
#define WiFiLED  BUILTIN_LED

// Define Water Level Value
int WaterLevelValue = 0 ;

// Objet WebServer
ESP8266WebServer serverWeb(80);

void handleRoot() {
  String webPage;
  DynamicJsonDocument doc(1024);
  doc["device"]["name"] = Hostname;
  doc["sensors"]["water"]["value"] = WaterLevelValue;

  serializeJson(doc, webPage);
  serverWeb.send(200, "application/json", webPage);
}

// Gestion des événements du WiFi
// Lorsque la connexion vient d'aboutir
void onConnected(const WiFiEventStationModeConnected& event);
// Lorsque l'adresse IP est attribuée
void onGotIP(const WiFiEventStationModeGotIP& event);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
   
  // Mode de connexion
  WiFi.mode(WIFI_STA);
  WiFi.hostname(Hostname.c_str());
 
  // Démarrage de la connexion
  WiFi.begin(SSID, PASSWORD);

  // Mise en place du serveur WebServer
  serverWeb.on("/", handleRoot);
  serverWeb.begin();

  //LED Setup
  pinMode(WiFiLED, OUTPUT);
  
  static WiFiEventHandler onConnectedHandler = WiFi.onStationModeConnected(onConnected);
  static WiFiEventHandler onGotIPHandler = WiFi.onStationModeGotIP(onGotIP);
}

void loop() {

  int s1=analogRead(A0); // Water Level Sensor output pin connected A0  
  Serial.println(s1);  // See the Value In Serial Monitor 
  WaterLevelValue = s1;    
  delay(100);      // for timer  
  
  if (WiFi.isConnected()) {
      digitalWrite(WiFiLED, HIGH);
      serverWeb.handleClient();
    }
  else {
    digitalWrite(WiFiLED, LOW);
    delay(1000);
    digitalWrite(WiFiLED, HIGH);
  }
}

void onConnected(const WiFiEventStationModeConnected& event) {
  Serial.println("WiFi connecté");
  Serial.println("Adresse IP : " + WiFi.localIP().toString());
}

void onGotIP(const WiFiEventStationModeGotIP& event) {
  Serial.println("Adresse IP : " + WiFi.localIP().toString());
  Serial.println("Passerelle IP : " + WiFi.gatewayIP().toString());
  Serial.println("DNS IP : " + WiFi.dnsIP().toString());
  Serial.print("Puissance de réception : ");
  Serial.println(WiFi.RSSI());
}
