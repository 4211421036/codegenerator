/* AllThingsTalk Arduino WiFi SDK
 * https://github.com/allthingstalk/arduino-wifi-sdk
 *
 * About this example: 
 * Waits for you to send a message from your AllThingsTalk Maker from an Actuator asset
 * and, once received, relays that message (using CBOR) back to a specified Sensor asset 
 * on your AllThingsTalk Maker.
 * This example will automatically create an actuator asset "relay-msg-actuator-example" on your AllThingsTalk for you to send a message from
 * This example will also automatically create a sensor asset "relay-msg-sensor-example" on your AllThingsTalk for you to receive a message on
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
char* actuator   = "relay-msg-actuator-example";                  // Name of asset on AllThingsTalk that you'll use to send a message (automatically created below)
char* sensor     = "relay-msg-sensor-example";                    // Name of asset on AllThingsTalk that you'll receive a message on (automatically created below)
CborPayload payload;                                              // Create CBOR payload object, so we can use CBOR to send data

void setup() {
  Serial.begin(115200);                   // Baud rate: 115200, but you can define any baud rate you want
  device.debugPort(Serial);               // Set AllThingsTalk library to output its debug to "Serial"
  device.setActuationCallback(actuator, actuation);  // Actuator asset defined above will trigger function "actuation"
  device.createAsset("relay-msg-actuator-example", "Relay Message Actuator (SDK Example)", "actuator", "string"); // Create asset on AllThingsTalk to send message
  device.createAsset("relay-msg-sensor-example", "Relay Message Sensor (SDK Example)", "sensor", "string"); // Create asset on AllThingsTalk to receive message
  device.init();                          // Initialize WiFi and AllThingsTalk
}

void loop() {
  device.loop();                          // Keep AllThingsTalk & WiFi alive
}

void actuation(String value) {            // Function called when message arrives on asset "actuator" defined above
  Serial.print("Received message: ");     // Prints to serial output
  Serial.println(value);                  // Prints actual value received to serial output
  payload.reset();                        // Resets the payload
  payload.set(sensor, value);             // Adds "value" received to be sent to sensor asset (defined above) on AllThingsTalk Maker
  device.send(payload);                   // Sends the set payload(s)
}