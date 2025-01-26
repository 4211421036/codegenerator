/**
   ESP8266ConfigExample.ino
   by Matthew Ford,  2022/04/07
   (c)2022 Forward Computing and Control Pty. Ltd.
   NSW, Australia  www.forward.com.au
   This code may be freely used for both private and commerical use.
   Provide this copyright is maintained.
*/

// This example is for ESP8266 boards --  Adafruit ESP8266 HUZZAH in this example
// It runs a simple hello server
// Configure a staticIP so you can connect the the hello server

#include <ESPAutoWiFiConfig.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

int ledPin = 0; // for Adafruit ESP8266 HUZZAH - onboard Led connected to GPIO0
// use ledPin = 2 for ESP-01S (Blue led on GPIO2)
// use ledPin = 1 for ESP-01 (Blue led on TX (GPIO1), ESP-01 also has a Red power led)
bool highForLedOn = false; // need to make output low to turn Adafruit ESP8266 HUZZAH's, ESP-01, ESP-01S  Led ON
size_t eepromOffset = 0; // if you use EEPROM.begin(size) in your code add the size here so AutoWiFi data is written after your data

//#define DEBUG

void startWebServer();

void setup() {
  // setup code that must be run every time
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println();
  for (int i = 10; i > 0; i--) {
    Serial.print(i); Serial.print(' ');
    delay(500);
  }
  Serial.println();
#endif

#ifdef DEBUG
  setESPAutoWiFiConfigDebugOut(Serial); // turns on debug output for the ESPAutoWiFiConfig code
#endif

  if (ESPAutoWiFiConfigSetup(ledPin, highForLedOn, eepromOffset)) { // check if we should start access point to configure WiFi settings
    return; // in config mode so skip rest of setup
  }
  
  // when we get here the WiFi has connected !!
  
  // normal setup code that is run when not configuring the WiFi settings
  startWebServer();
}

void loop() {
  //  .. other code that MUST run all the time
  if (ESPAutoWiFiConfigLoop()) {  // handle WiFi config webpages
    return;  // skip the rest of the loop until config finished
  }
  
  // .. normal loop code here that is run when not configuring the WiFi settings
  server.handleClient();
}

// ========== trivial webserver example =============
void notFound() {
  server.send(404, "text/plain", "Not found");
}

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!\r\n");
}

void startWebServer() {
  server.on("/", handleRoot);
  server.onNotFound(notFound);
  server.begin();
}
