#include "ESP8266WiFi.h"
#include "secrets.h"
#include <Wire.h>
#include <BH1750.h>
#include "ESP8266WebServer.h"
#include <ArduinoJson.h>
///PIR Sensor Include
#include <DigitalSensor.h>
///DHT Sensor Include
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Arduino.h>
///Gas Sensor Include
#include "MQ135.h"

String Hostname = "SweetBee";

bool ModeDHT = true;
bool ModePIR = true;
bool ModeLux = true;

// Define LED
// LED Wifi
#define WiFiLED  BUILTIN_LED
#define MotionLED  15

//Define Lightmeter
BH1750 lightMeter;

/// PIR Sensor Define
// Create digital PIR sensor instance on D2 pin
DigitalSensor pir(14);
bool presence = false;

///DHT Sensor Define
#define DHTPIN 13     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

float t = 0.0;
float h = 0.0;

/// Gas Sensor Define
#define ANALOGPIN A0    //  Define Analog PIN on Arduino Board
#define RZERO 206.85    //  Define RZERO Calibration Value
MQ135 gasSensor = MQ135(ANALOGPIN);

// Objet WebServer
ESP8266WebServer serverWeb(80);


void handleRoot() {
  String webPage;
  DynamicJsonDocument doc(1024);
  doc["device"]["name"] = Hostname;
  if(ModeDHT){
    doc["sensors"]["temperature"]["value"] = t;
    doc["sensors"]["temperature"]["unit"] = "°C";
    doc["sensors"]["humidity"]["value"] = h;
    doc["sensors"]["humidity"]["unit"] = "%";
  } 
  if (ModePIR)
  {
    doc["sensors"]["motion"]["value"] = presence;
  }
  if (ModeLux)
  {
    doc["sensors"]["light"]["value"] = lightMeter.readLightLevel();
    doc["sensors"]["light"]["unit"] = "lx";
  }
  else {
    doc["sensors"]["gas"]["value"] = gasSensor.getPPM();
    doc["sensors"]["gas"]["unit"] = "ppm";
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
  Wire.begin();
  
  if(ModeDHT){
    // DHT Sensor
    // Initialize device.
    dht.begin();
    Serial.println(F("DHTxx Unified Sensor Example"));
    // Print temperature sensor details.
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    Serial.println(F("------------------------------------"));
    Serial.println(F("Temperature Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
    Serial.println(F("------------------------------------"));
    // Print humidity sensor details.
    dht.humidity().getSensor(&sensor);
    Serial.println(F("Humidity Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
    Serial.println(F("------------------------------------"));
    // Set delay between sensor readings based on sensor details.
    delayMS = sensor.min_delay / 1000;
  
  }
  if(ModeLux){
    // Setup lightmeter
    // Initialize the I2C bus (BH1750 library doesn't do this automatically)
    // On esp8266 devices you can select SCL and SDA pins using Wire.begin(D4, D3);
    // If you are using a Nodemcu, you can use the default D1 & D2
    lightMeter.begin();
    Serial.println(F("BH1750 Setup"));
  }
  else {
  ///Gas Sensor Setup
  float rzero = gasSensor.getRZero();
  delay(3000);
  Serial.print("MQ135 RZERO Calibration Value : ");
  Serial.println(rzero);
  }
  
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
  pinMode(MotionLED, OUTPUT);
  
  static WiFiEventHandler onConnectedHandler = WiFi.onStationModeConnected(onConnected);
  static WiFiEventHandler onGotIPHandler = WiFi.onStationModeGotIP(onGotIP);
}

void loop() {
  if(ModeDHT){
    //DHT Sensor
    // Delay between measurements.
    delay(delayMS);
    // Get temperature event and print its value.
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
    }
    else {
      t = event.temperature;
      Serial.print(F("Temperature: "));
      Serial.print(event.temperature);
      Serial.println(F("°C"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
    }
    else {
      h = event.relative_humidity;
      Serial.print(F("Humidity: "));
      Serial.print(event.relative_humidity);
      Serial.println(F("%"));
    }
  } if(ModePIR){
    ///PIR Sensor Loop
    Serial.println("PIR state: ");
    if (pir.status()) {
      presence = true;
      digitalWrite(MotionLED, HIGH);
      Serial.println("Someone detected");
    } else {
      digitalWrite(MotionLED, LOW);
      Serial.println("No one detected");
      presence = false;
    }
  }
  if(ModeLux){
    // Lightmeter loop
    // put your main code here, to run repeatedly:
    float lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
  }
  else {
    ///Gas Sensor Loop
    float ppm = gasSensor.getPPM();
    Serial.print("CO2 ppm value : ");
    Serial.println(ppm);
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
