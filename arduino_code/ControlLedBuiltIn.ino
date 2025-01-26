/* AllThingsTalk Arduino WiFi SDK
 * https://github.com/allthingstalk/arduino-wifi-sdk
 *
 * About this example: 
 * Control the built-in LED of your board using your AllThingsTalk Maker.
 * This example will automatically create an asset "builtin-led-example" on your AllThingsTalk for you to control the LED
 * Requires no external components.
 * 
 * Notes:
 * - Create a device on your https://maker.allthingstalk.com (if you don't already have it)
 * - The Built-In LED of the ESP8266 is inverted by design. 
 *   Because of this, we'll need to use "HIGH" when we want to turn it off and "LOW" when we want it on.
 *   If you're using some other board and it's inverted, "LOW" to "HIGH" and vice versa.
 *
 * These are all the things in this example that you need to change to make it work: 
 *   WiFiSSID, WiFiPassword, DeviceID, DeviceToken, LED_BUILTIN (Change only if this doesn't work for you)
 */

#include <AllThingsTalk_WiFi.h>

auto wifiCreds   = WifiCredentials("WiFiSSID", "WiFiPassword");   // Your WiFi Network Name and Password
auto deviceCreds = DeviceConfig("DeviceID", "DeviceToken");       // Go to AllThingsTalk Maker > Devices > Your Device > Settings > Authentication to get your Device ID and Token
auto device      = Device(wifiCreds, deviceCreds);                // Create "device" object
int  ledPin      = LED_BUILTIN;                                   // Built-in LED of your board. "LED_BUILTIN" is automatically recognized as the built-in LED pin.
char* asset      = "builtin-led-example";                         // Name of asset on AllThingsTalk that you'll use to control the LED (automatically created below)

void setup() {
  Serial.begin(115200);                 // Baud rate: 115200, but you can define any baud rate you want
  pinMode(ledPin, OUTPUT);              // Set the pin of our LED to OUTPUT
  digitalWrite(ledPin, HIGH);           // Because the built-in LED is inverted, it's turned on at boot. We'll turn it off here.
  device.debugPort(Serial);             // Set AllThingsTalk library to output its debug to "Serial"
  device.connectionLed(false);          // Disable the Connection LED feature because we're going to control that exact LED
  device.setActuationCallback(asset, actuation);  // "asset" - Variable we defined above, "actuation" - Name of the function below that does the work
  device.createAsset("builtin-led-example", "Built-In LED (SDK Example)", "actuator", "boolean"); // Create asset on your AllThingsTalk to control the LED
  device.init();                        // Initialize AllThingsTalk
}

void loop() {
  device.loop();                        // Keep AllThingsTalk & WiFi alive
}

void actuation(bool value) {            // Function called when message arrives to your specified asset. "value" is the actual value received.
  if (value == true) {
    digitalWrite(ledPin, LOW);          // Set pin "ledPin" to LOW (ON, because it's inverted)
    Serial.println("LED Turned ON");    // Prints to serial port
  } else {
    digitalWrite(ledPin, HIGH);         // Sets pin "ledPin" to HIGH (OFF, because it's inverted)
    Serial.println("LED Turned OFF");   // Prints to serial port
  }
}