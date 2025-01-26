/* AllThingsTalk Arduino WiFi SDK
 * https://github.com/allthingstalk/arduino-wifi-sdk
 *
 * About this example: 
 * This is an ARDUINO WIFI MKR1010 board specific example. It does not work on other boards.
 * Utilizes MKR1010's built-in RGB LED and gives you ability to control the RGB LED
 * using the color picker wheel on AllThingsTalk Maker.
 * Requires no external components.
 * 
 * Notes:
 * - Create a device on your https://maker.allthingstalk.com (if you don't already have it)
 * - Create an Actuator asset of type String (name it however you like, but make sure to put the name in the "actuator" variable)
 * - Click on the asset, then click "Advanced" in the Profile section.
 * - You should see { "type": "string" } inside the text box field. Delete everything shown in that field.
 * - Now copy/paste this into the empty field: {"type": "string", "pattern": "^#([a-fA-F0-9]{6})$"}
 * - Click the checkmark in the bottom right corner.
 * - Now go to your Pinboards (create a new one if you don't have any)
 * - Pin a new "Color picker" control to your Pinboard
 *   - Choose the device you created.
 *   - Choose the asset you created for controlling the RGB LED and pin it.
 *   - Pin it and save.
 * - Click on the newly created color picker, choose a color and see your MKR1010 RGB LED light up in that color
 *
 * These are all the things in this example that you need to change to make it work: 
 *   WiFiSSID, WiFiPassword, DeviceID, DeviceToken, ActuatorAsset
 */

#include <AllThingsTalk_WiFi.h> // Where the magic happens
#include <WiFiNINA.h>           // This is required only for the purpose of loading the library below.
#include <utility/wifi_drv.h>   // Exposes MKR1010's underlying functions to control the RGB LED

auto wifiCreds   = WifiCredentials("WiFiSSID", "WiFiPassword");   // Your WiFi Network Name and Password
auto deviceCreds = DeviceConfig("DeviceID", "DeviceToken");       // Go to AllThingsTalk Maker > Devices > Your Device > Settings > Authentication to get your Device ID and Token
auto device      = Device(wifiCreds, deviceCreds);                // Create "device" object
char* actuator   = "ActuatorAsset";                               // Change this to fit the name of your asset
int r,g,b;                                                        // RGB values will be held in these variables

void setup() {
  Serial.begin(115200);                 // Baud rate for Serial output (debug), but you can define any baud rate you want
  WiFiDrv::pinMode(25, OUTPUT);         // Initialize the Green LED pin on the MKR1010 board
  WiFiDrv::pinMode(26, OUTPUT);         // Initialize the Red LED pin on the MKR1010 board
  WiFiDrv::pinMode(27, OUTPUT);         // Initialize the Blue LED pin on the MKR1010 board
  device.debugPort(Serial);             // Set AllThingsTalk library to output its debug to "Serial"
  device.setActuationCallback(actuator, rgb); // Add an actuation callback for asset defined above and run function "rgb" once command is received
  device.init();                        // Initialize WiFi and AllThingsTalk
}

void rgb(String value) {                // Function that will be called when you pick a color on AllThingsTalk
  Serial.println("RGB Color Changed!"); // Outputs to Serial
  // Parses the received RGB information into three separate variables (R, G, B)
  long hexColor = (long) strtol(&value[1], NULL, 16);
  r = hexColor >> 16;
  g = hexColor >> 8 & 0xFF;
  b = hexColor & 0xFF;
  WiFiDrv::analogWrite(25, g);          // Set the Green LED to received value
  WiFiDrv::analogWrite(26, r);          // Set the Red LED to received value
  WiFiDrv::analogWrite(27, b);          // Set the Blue LED to received value
}

void loop() {                           // Runs as long as the device in on
  device.loop();                        // Keep AllThingsTalk & WiFi connection alive
}