/**
   ESP32ConfigExample.ino
   by Matthew Ford,  2022/04/07
   (c)2022 Forward Computing and Control Pty. Ltd.
   NSW, Australia  www.forward.com.au
   This code may be freely used for both private and commerical use.
   Provide this copyright is maintained.
*/

// This example is for ESP32 boards --  Sparkfun ESP32 Thing in this example
// It runs a simple hello server
// Configure a staticIP so you can connect the the hello server

#include <ESPAutoWiFiConfig.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

WebServer server(80);

int ledPin = 5; // for Sparkfun ESP32 Thing - onboard Led connected to GPIO5
bool highForLedOn = true; // need to make output high to turn Sparkfun ESP32 Thing's GPIO5 Led ON
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

  if (ESPAutoWiFiConfigSetup(ledPin, highForLedOn,0)) { // check if we should start access point to configure WiFi settings
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
