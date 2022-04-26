# Prepare system:
ToDo

# Wifi Connection:
source: http://geii.eu/index.php?option=com_content&view=article&id=225&Itemid=935#se-connecter-au-r%C3%A9seau-wifi-et-g%C3%A9rer-les-%C3%A9v%C3%A9nements-wifi
```
#include "ESP8266WiFi.h"
#include "secrets.h"

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
 
  // Démarrage de la connexion
  WiFi.begin(SSID, PASSWORD);

  static WiFiEventHandler onConnectedHandler = WiFi.onStationModeConnected(onConnected);
  static WiFiEventHandler onGotIPHandler = WiFi.onStationModeGotIP(onGotIP);
}

void loop() {
  if (WiFi.isConnected()) {
    //digitalWrite(PIN_LED_ROUGE, HIGH);
  }
  else {
    //digitalWrite(PIN_LED_ROUGE, LOW);
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
```
# Integrate Light sensor BH1750
Add the following BH1750 Library to your IDE
https://github.com/claws/BH1750

## Do the proper includes
```
#include <Wire.h>
#include <BH1750.h>
```
## Define Lightmeter object

```
BH1750 lightMeter;
```
## Lightmeter setup
```
  // Setup lightmeter
  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  // On esp8266 devices you can select SCL and SDA pins using Wire.begin(D4, D3);
  // If you are using a Nodemcu, you can use the default D1 & D2
  Wire.begin();

  lightMeter.begin();
  Serial.println(F("BH1750 Test"));
  ```
  
## Lightmeter loop
```
  // Lightmeter loop
  // put your main code here, to run repeatedly:
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);
```
# Webserver & JSON Response
Add Arduino JSON to your library

https://arduinojson.org/?utm_source=meta&utm_medium=library.properties

## Includes
```
#include "ESP8266WebServer.h"
#include <ArduinoJson.h>
```

## Defines
```
// Objet WebServer
ESP8266WebServer serverWeb(80);
```
Our JSON result
```
void handleRoot() {
  String webPage;
  DynamicJsonDocument doc(1024);
  
  doc["sensors"]["light"]["value"] = lightMeter.readLightLevel();
  doc["sensors"]["light"]["unit"] = "lx";

  serializeJson(doc, webPage);
  serverWeb.send(200, "application/json", webPage);
}
```
## Webserver setup
```
// Mise en place du serveur WebServer
  serverWeb.on("/", handleRoot);
  serverWeb.begin();
```
## Webserver loop
If the WiFi is connected launch the client
```
serverWeb.handleClient();
```

You should see the result

![image](https://user-images.githubusercontent.com/5168811/165161434-b96943e1-0b9b-4a74-87c4-26b7544ca526.png)

# Motion Sensor
I got my PIR sensor from AZ Delivery. From the documentation, install DigitalSensor library from https://cdn.shopify.com/s/files/1/1509/1638/files/Bewegungsmelder_Arduino.rar?4080858123666336120

## Includes
```
///PIR Sensor Include
#include <DigitalSensor.h>
```

## Defines 
```
/// PIR Sensor Define
// Create digital PIR sensor instance on D5 pin
DigitalSensor pir(14);
bool presence = false;
```

## PIR Setup
```
//empty
```

## PIR loop
```
  ///PIR Sensor Loop
  Serial.println("PIR state: ");
  if (pir.status()) {
    presence = true;
    Serial.println("Someone detected");
  } else {
    Serial.println("No one detected");
    presence = false;
  }
```
Then we add a line in our JSON handler. As <presence> is globally defined and assigned in the loop, we can retrieve value.

```
doc["sensors"]["motion"]["value"] = presence;
```
  
# DHT Sensor
  I bought the DHT sensor from AZ delivery, refering to the [wiring](https://cdn.shopify.com/s/files/1/1509/1638/files/DHT_22_-_AM2302_Temperatur-_und_Luftfeuchtigkeitssensor_Arduino_Schematics_1.pdf?9648280433239966886) and following library https://github.com/adafruit/DHT-sensor-library
    
## Includes
```
///DHT Sensor Include
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
```
  
## Defines
  ```
  ///DHT Sensor Define
#define DHTPIN 2     // Digital pin connected to the DHT sensor 
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
  ```
  
  ## DHT Setup
  ```
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
  ```
  
  ## DHT loop
  ```
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
 
  ```
  
  Thanks to the global float h & t variable, we can output the values in our API
  
  ```
  doc["sensors"]["Temperature"]["value"] = t;
  doc["sensors"]["Temperature"]["unit"] = "°C";
  doc["sensors"]["Humidity"]["value"] = h;
  doc["sensors"]["Humidity"]["unit"] = "%";
  ```
