/* AllThingsTalk Arduino WiFi SDK
 * https://github.com/allthingstalk/arduino-wifi-sdk
 *
 * About this example: 
 * Shows the minimal code required to connect to Wi-Fi and send a message to your AllThingsTalk Maker
 * Sends a "Hello World!" message upon boot.
 *
 * Notes:
 * - Create a device on your https://maker.allthingstalk.com (if you don't already have it)
 * - Create a Sensor asset of type String on your AllThingsTalk Maker
 *
 * These are all the things in this example that you need to change to make it work:
 *   WiFiSSID, WiFiPassword, DeviceID, DeviceToken, SensorAsset
 */

#include <AllThingsTalk_WiFi.h>

auto wifiCreds   = WifiCredentials("WiFiSSID", "WiFiPassword");   // Your WiFi Network Name and Password
auto deviceCreds = DeviceConfig("DeviceID", "DeviceToken");       // Go to AllThingsTalk Maker > Devices > Your Device > Settings > Authentication to get your Device ID and Token
auto device      = Device(wifiCreds, deviceCreds);                // Create "device" object

void setup() {
  device.init();                                // Initialize WiFi and AllThingsTalk
  device.send("SensorAsset", "Hello World!");   // Send "Hello World!" to asset named "SensorAsset" (change this to fit your asset name)
}

void loop() {
  device.loop();    // Keeps the WiFi and AllThingsTalk connection alive (optional in this example)
}