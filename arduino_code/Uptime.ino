/*
   Create a text widget and connect it to user function 'sendUptime'.
   The widget will show the time the ESP8266 has been running since the last reset.
*/


#include "RedkeaESP8266.h"

char ssid[] = "xxx";     // your WiFi SSID
char pass[] = "xxx";     // your WiFi password
char deviceID[] = "xxx"; // your device ID

RedkeaESP8266 redkea;

REDKEA_SENDER(sendUptime, widgetID) {
    long secondsSinceStart = millis() / 1000;
    String str = "Device running since ";
    str += secondsSinceStart;
    str += " seconds";
    redkea.sendToTextWidget(widgetID, str);
}

REDKEA_REGISTER_SENDER(redkea, sendUptime);

void setup() {
    Serial.begin(9600);
    redkea.begin(ssid, pass, deviceID);
}

void loop() {
    redkea.loop();
}