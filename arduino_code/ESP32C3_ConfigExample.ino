/**
   ESP32C3_ConfigExample.ino
   by Matthew Ford,  2022/04/27
   (c)2022 Forward Computing and Control Pty. Ltd.
   NSW, Australia  www.forward.com.au
   This code may be freely used for both private and commerical use.
   Provide this copyright is maintained.
*/

// This example is for ESP32 C3 and similar boards that have a ws8212 RGB led installed
// use -ve for the ledPin to trigger ws2812 led control on that pin
// It runs a simple hello server
// Configure a staticIP so you can connect the the hello server

#include <ESPAutoWiFiConfig.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

WebServer server(80);

// which pin drives the ws2812 addressable RGB led on ESP32 C/S
// -ve pin value => use ws2812 driver
// for Adafruit ESP32C3 QTPY use pinNo = -2
// for ESP32C3_RGB_LED use pinNo = -8
// for ESP32-S2-DevKitM-1 	use pinNo = -18
// for ESP32-S2-Saola-1 	use pinNo = -18
// for ESP32-S3-DevKitC-1 	use pinNo = -48
// for Fire_Beetle ESP32-E 	use pinNo = -D8   not D8 not just 8

int ledPin = -8; // for ESP32C3 RGB LED,
// default colour GREEN,
// use ESPAutoWiFiConfig_setRGBLedColor(uint32_t red, uint32_t green, uint32_t blue), arguments in range 0 to 255, to change the colour
bool highForLedOn = true; // false for ws8212 will leave the led ON when WiFi is connected
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
  ESPAutoWiFiConfig_setColor(0, 0, 128); // make led flash Blue instead of default light GREEN
  if (ESPAutoWiFiConfigSetup(ledPin, highForLedOn, 0)) { // check if we should start access point to configure WiFi settings
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
