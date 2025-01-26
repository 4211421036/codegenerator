#include<MFUthings.h>

/**
 *  This is the library that be used in www.mfuthings.com which belongs to Mae Fah Luang University
 *  this library has dependencies that are PubSubClient and ESP8266WiFi so make sure you installed these libraries 
 *  also make sure the Arduino IDE version is 1.6.8 or greater
 */

#define ID      1                   // EQUIPMENT ID
#define KEY     "key"               // PRIVATE KEY
#define TYPE    "OUTPUT"            // EQUIPMENT TYPE WHICH ARE INPUT OR OUTPUT
#define OUTPUT  D2                  // OUTPUT PIN

#define SSID    "WiFi SSID"         // WiFi SSID
#define PASSWD  "WiFi Password"     // WiFi Password

/**
 * MFUthings    => the class instance
 * @param ID    => EQUIPMENT ID
 * @param KEY   => PRIVATE KEY
 * @param TYPE  => EQUIPMENT TYPE
 * @return  MFUthings instance
 */
MFUthings mfuThings(ID,KEY,TYPE);   

void setup() {
  // uncomment if you want to pass third argument as the output
  // mfuThings.connectWiFi(SSID,PASSWD,D8);
  // you can replace D8 with another pin as you want
  mfuThings.connectWiFi(SSID,PASSWD,D8);
  // if type is output, the output pin must be determined
  // comment if type is input
  mfuThings.output(OUTPUT);
  // begin   
  mfuThings.begin();
}

void loop() {
  // uncomment if type is input, input value must be specified
  // mfuThings.handle(value);
  mfuThings.handle();
  delay(1);
}
