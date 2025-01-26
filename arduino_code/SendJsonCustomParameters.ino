/* AllThingsTalk Arduino WiFi SDK
 * https://github.com/allthingstalk/arduino-wifi-sdk
 *
 * About this example: 
 * Shows various functions of this SDK.
 * It sets the Connection LED (that shows connection status) to LED_BUILTIN, 
 * sets WiFi Signal Reporting to 60 seconds and sets the WiFi hostname to "ATT-EXAMPLE".
 * It automatically creates asset on your AllThingsTalk named "custom-example" to which you'll receive data on.
 * Upon boot, it sends a message that it has booted and that it will disconnect soon.
 * It then disconnects and reconnects after 5 seconds, after which it sends a message
 * that it has reconnected. The process is repeated after 5 seconds.
 * This example will also automatically create an asset "wifi-signal" on your AllThingsTalk to which you'll receive WiFi Signal Strength info to.
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
char* asset      = "custom-example";                              // Name of asset on AllThingsTalk to which you'll receive data (automatically created below)

void setup() {
  Serial.begin(115200);                    // Baud rate: 115200, but you can define any baud rate you want
  device.debugPort(Serial, true);          // Set library to output verbose debug to "Serial" port (defined above)
  device.connectionLed(true, LED_BUILTIN); // Built-in LED defined as Connection LED (LED_BUILTIN is a global variable that specifies the default onboard LED)
  device.wifiSignalReporting(true, 60);    // Report WiFi Signal every 60 seconds
  device.setHostname("ATT-EXAMPLE");       // Sets hostname of the device (Shows up on your WiFi network under this name)
  device.createAsset("custom-example", "SendJsonCustomParameters (SDK Example)", "sensor", "string"); // Create asset on AllThingsTalk to send value to
  device.init();                           // Initialize WiFi and AllThingsTalk
  device.send(asset, "Booted! Hello!");    // Send message "Booted! Hello!" to topic "asset_name"
}
void loop() {
  device.loop();         // Used to keep AllThingsTalk & WiFi connection alive
  device.send(asset, "Disconnecting WiFi & AllThingsTalk in 3 seconds"); // Send message to "asset" variable defined on top
  delay(3000);           // Wait 3 seconds
  device.send(asset, "Disconnecting WiFi & AllThingsTalk Now");
  device.disconnect();   // Disconnect from both AllThingsTalk and WiFi
  delay(5000);           // Wait 5 seconds
  device.connect();      // Connect to both WiFi and AllThingsTalk
  device.send(asset, "WiFi and AllThingsTalk Connection Re-Established");
  delay(5000);           // Wait 5 seconds
}