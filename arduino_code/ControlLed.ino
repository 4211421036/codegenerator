/* AllThingsTalk Arduino WiFi SDK
 * https://github.com/allthingstalk/arduino-wifi-sdk
 *
 * About this example: 
 * Control an externally connected LED using your AllThingsTalk Maker.
 * Try the "ControlLedBuiltIn" example if you don't have an LED lying around.
 * This example will automatically create an asset "led-example" on your AllThingsTalk for you to control the LED
 * 
 * Notes:
 * - Create a device on your https://maker.allthingstalk.com (if you don't already have it)
 * - Connect your LED's Negative (-) leg to your board's GND
 * - Connect your LED's Positive (+) leg to an adequate digital pin on your board
 *
 *  These are all the things in this example that you need to change to make it work:
 *    WiFiSSID, WiFiPassword, DeviceID, DeviceToken, YourLedPin
 */

#include <AllThingsTalk_WiFi.h>

auto wifiCreds   = WifiCredentials("WiFiSSID", "WiFiPassword");   // Your WiFi Network Name and Password
auto deviceCreds = DeviceConfig("DeviceID", "DeviceToken");       // Go to AllThingsTalk Maker > Devices > (Your Device) > Settings > Authentication to get your Device ID and Token
auto device      = Device(wifiCreds, deviceCreds);                // Create "device" object
int  ledPin      = YourLedPin;                                    // Change YourLedPin to the pin you connected your LED to
char* asset      = "led-example";                                 // Name of asset on AllThingsTalk that you'll use to control the LED (automatically created below)

void setup() {
  Serial.begin(115200);                 // Baud rate: 115200, but you can define any baud rate you want
  pinMode(ledPin, OUTPUT);              // Set the pin of our LED to OUTPUT
  device.debugPort(Serial);             // Set AllThingsTalk library to output its debug to "Serial"
  device.setActuationCallback(asset, actuation);  // "asset" - Variable we defined above, "actuation" - Name of the function below that does the work
  device.createAsset("led-example", "LED (SDK Example)", "actuator", "boolean"); // Create asset "led-example" on your AllThingsTalk to control the LED
  device.init();                        // Initialize AllThingsTalk
}

void loop() {
  device.loop();                        // Keep AllThingsTalk & WiFi alive
}

void actuation(bool value) {            // Function called when message arrives to your specified asset. "value" is the actual value received.
  if (value == true) {
    digitalWrite(ledPin, HIGH);         // Set pin "ledPin" to HIGH (ON)
    Serial.println("LED Turned ON");    // Prints to serial port
  } else {
    digitalWrite(ledPin, LOW);          // Sets pin "ledPin" to LOW (OFF)
    Serial.println("LED Turned OFF");   // Prints to serial port
  }
}