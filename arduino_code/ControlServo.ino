/* AllThingsTalk Arduino WiFi SDK
 * https://github.com/allthingstalk/arduino-wifi-sdk
 *
 * About this example: 
 * Control a servo motor using your AllThingsTalk Maker.
 * This example will automatically create an asset "servo" on your AllThingsTalk for you to control the motor
 * It will also automatically create a "wifi-signal" asset on your AllThingsTalk so you can see the WiFi signal strength of your device
 * 
 * Notes:
 * - Create a device on your https://maker.allthingstalk.com (if you don't already have it)
 * - Assuming your servo motor has 3 pins, connect them as follows:
 *   - Connect servo's Positive/+/Red wire to VIN/5V pin on your board. DO NOT Connect it to 3v3 pins
 *   - Connect servo's GND/-/Black wire to any GND pin on your board
 *   - Connect servo's Signal/Yellow/Orange wire to a desired pin on your board (check beforehand if that pin can be used for output)
 *     - Check beforehand if that pin can be used for output
 *     - Change "SERVO_PIN" down below to the pin you connected your servo's signal wire to
 * - Change WiFiSSID, WiFiPassword, DeviceID, DeviceToken to your credentials
 * - Flash this sketch to your board
 * - Create a new pin on your pinboard:
 *   - Control: "Slider"
 *   - Min: 0
 *   - Max: 180
 *   - Asset: "servo" (asset is automatically created when the device boots)
 * - Move the slider and see the servo move!
 */

#include <AllThingsTalk_WiFi.h>
#include <Servo.h>

auto wifiCreds   = WifiCredentials("WiFiSSID", "WiFiPassword");   // Your WiFi Network Name and Password
auto deviceCreds = DeviceConfig("DeviceID", "DeviceToken");       // Go to AllThingsTalk Maker > Devices > Your Device > Settings > Authentication to get your Device ID and Token
auto device      = Device(wifiCreds, deviceCreds);                // Create "device" object
int  servoPin    = SERVO_PIN;                                     // The pin to which your Servo motor's signal wire is connected to
char* asset      = "servo";                                       // Name of asset on AllThingsTalk that you'll use to control the Servo motor (automatically created below)
Servo servo;                                                      // Create servo object

void setup() {
  Serial.begin(115200);                 // Baud rate: 115200, but you can define any baud rate you want
  device.debugPort(Serial);             // Set AllThingsTalk library to output its debug to "Serial"
  device.wifiSignalReporting(true);     // Enables WiFi Signal Strength reporting to your AllThingsTalk
  device.setActuationCallback(asset, controlServo);  // "asset" - Variable we defined above, "controlServo" - Name of the function below that does the work
  device.createAsset(asset, "Servo (SDK Example)", "actuator", "integer"); // Create asset on your AllThingsTalk to control the Servo
  device.init();                        // Initialize AllThingsTalk
}

void loop() {
  device.loop();                        // Keep AllThingsTalk & WiFi alive
}

void controlServo(int value) {          // Function called when message arrives to your specified asset. "value" is the actual value received.
  servo.attach(servoPin);               // Activates the servo motor
  Serial.println("Changing Servo Motor Position to: ");
  Serial.println(value);                // Print current value of servo to serial monitor
  servo.write(value);                   // Send the value to which the servo motor should move to
  delay(700);                           // Give the servo motor some time to finish its movement
  servo.detach();                       // Disactivate servo motor
}