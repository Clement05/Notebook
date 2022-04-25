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
