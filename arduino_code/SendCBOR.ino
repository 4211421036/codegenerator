/* AllThingsTalk Arduino WiFi SDK
 * https://github.com/allthingstalk/arduino-wifi-sdk
 *
 * About this example: 
 * Increments a counter from 0 (by 1) and uploads the new value to your AllThingsTalk Maker.
 * Value is uploaded using CBOR sending method.
 * Process is repeated every 5 seconds.
 * This example will automatically create an asset "counter-example" on your AllThingsTalk to which you'll receive data to
 *
 * Notes:
 * - Create a device on your https://maker.allthingstalk.com (if you don't already have it)
 *
 * These are all the things in this example that you need to change to make it work:
 *   WiFiSSID, WiFiPassword, DeviceID, DeviceToken
 */

#include <AllThingsTalk_WiFi.h>

auto wifiCreds   = WifiCredentials("WiFiSSID", "WiFiPassword");   // Your WiFi Network Name and Password
auto deviceCreds = DeviceConfig("DeviceID", "DeviceToken");       // Go to AllThingsTalk Maker > Devices > Your Device > Settings > Authentication to get your Device ID and Token
auto device      = Device(wifiCreds, deviceCreds);                // Create "device" object
char* sensor     = "counter-example";                             // Name of asset on AllThingsTalk to which you'll receive the value (automatically created below)
CborPayload payload;                                              // Create CBOR payload object, so we can use CBOR sending

int value = 0;                 // Initial value of counter

void setup() {
  Serial.begin(115200);        // Baud rate: 115200, but you can define any baud rate you want   
  device.debugPort(Serial);    // Set AllThingsTalk library to output its debug to "Serial"
  device.createAsset("counter-example", "Counter (SDK Example)", "sensor", "integer"); // Create asset on AllThingsTalk to send value to
  device.init();               // Initialize WiFi and AllThingsTalk
}

void loop() {
  device.loop();               // Keep AllThingsTalk & WiFi Alive
  ++value;                     // Increment the value by 1
  payload.reset();             // Reset the payload
  payload.set(sensor, value);  // Set "value" (counter) to be sent to sensor asset defined above
  device.send(payload);        // Send the payload
  delay(3000);                 // Wait 5 seconds until repeating
}