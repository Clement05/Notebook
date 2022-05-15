#include "ESP8266WiFi.h"
#include "secrets.h"
#include "ESP8266WebServer.h"
#include <ArduinoJson.h>

String Hostname = "BumbleBee";

// Define LED
// LED Wifi
#define WiFiLED  0
#define WarningLED  2
bool WarningLED_Blink = false;

// Objet WebServer
ESP8266WebServer serverWeb(80);


void handleRoot() {
  String webPage;
  DynamicJsonDocument doc(1024);
  doc["device"]["name"] = Hostname;

  serializeJson(doc, webPage);
  serverWeb.send(200, "application/json", webPage);
}

void handleAPI(){
  String response;

  String webPage;
  DynamicJsonDocument doc(1024);
  
  if(serverWeb.args()>0)
  {
    if(serverWeb.argName(0) == "cmd"){
     if(serverWeb.args()==1){
        if(serverWeb.arg(0)=="getWarningLed"){
          if(digitalRead(WarningLED) == LOW){
            doc["WarningLED"]["Value"] = 0;
          } else if (digitalRead(WarningLED) == HIGH){
            doc["WarningLED"]["Value"] = 1;
          } else {
            doc["WarningLED"]["Value"] = -1;         
          }
        } else {
          doc["Error"]["405"] = "Unknown command or missing argument(s)";
        }
      } 
      else if (serverWeb.args()==2){
        if(serverWeb.arg(0)=="setWarningLed"){
          if(serverWeb.argName(1)=="value"){
            if(serverWeb.arg(1)=="On"){
              digitalWrite(WarningLED, HIGH);
              doc["WarningLED"]["Value"] = 1;
            } else if (serverWeb.arg(1)=="off"){
              digitalWrite(WarningLED, LOW);
              doc["WarningLED"]["Value"] = 0;
            } else if (serverWeb.arg(1)=="Blink_On"){
              WarningLED_Blink = true;
              doc["WarningLED"]["Value"] = "Blink";
            } else if (serverWeb.arg(1)=="Blink_Off"){
              WarningLED_Blink = false;
              doc["WarningLED"]["Value"] = "Blink_Off";
            }
            else {
              doc["Error"]["405"] = "Unknown argument(s)";
            }
          }else {
            doc["Error"]["405"] = "Unknown argument(s)";
          }
        } else {
         doc["Error"]["405"] = "Unknown command";
        }
      } else {
        doc["Error"]["405"] = "Too many args";
      }
    }
    else {
      doc["Error"]["405"] = "Unknown command";
    }
  }
  else {
    doc["Error"]["400"] = "Empty call";
  }

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
  serverWeb.on("/api", handleAPI);
  serverWeb.begin();

  //LED Setup
  pinMode(WiFiLED, OUTPUT);
  pinMode(WarningLED, OUTPUT);
  
  static WiFiEventHandler onConnectedHandler = WiFi.onStationModeConnected(onConnected);
  static WiFiEventHandler onGotIPHandler = WiFi.onStationModeGotIP(onGotIP);
}

void loop() {
  if(WarningLED_Blink){
    digitalWrite(WarningLED, LOW);
    delay(1000);
    digitalWrite(WarningLED, HIGH);
  } else if (!WarningLED_Blink){
    digitalWrite(WarningLED, LOW);
  }
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
