/* AllThingsTalk Arduino WiFi SDK
 * https://github.com/allthingstalk/arduino-wifi-sdk
 *
 * About this example:
 * Shows the minimal code required to connect to Wi-Fi and send a message to your AllThingsTalk Maker.
 * Sends a "Hello World!" message upon boot.
 * Uses the "keys.h" file that separates credentials from the sketch.
 * 
 * Notes:
 * - Create a device on your https://maker.allthingstalk.com (if you don't already have it)
 * - Create a Sensor asset of type String on your AllThingsTalk Maker
 *
 * These are all the things in this example that you need to change to make it work: 
 *   SensorAsset
 *   Edit the "keys.h" file (second tab of your Arduino IDE) and enter your credentials there.
 */
 
#include <AllThingsTalk_WiFi.h>
#include "keys.h"

auto wifiCreds   = WifiCredentials(WIFI_SSID, WIFI_PASSWORD); // Edit these in the "keys.h" file
auto deviceCreds = DeviceConfig(DEVICE_ID, DEVICE_TOKEN);     // Edit these in the "keys.h" file
auto device      = Device(wifiCreds, deviceCreds);            // Create "device" object

void setup() {
  device.init();                                 // Initialize WiFi and AllThingsTalk
  device.send("SensorAsset", "Hello World!");   // Send "Hello World!" to asset named "Sensor-Asset" (change this to fit your asset name)
}

void loop() {
  device.loop();    // Keeps the WiFi and AllThingsTalk connection alive (optional in this example)
}