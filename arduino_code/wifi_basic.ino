#include "Senses_wifi.h"

const char *ssid = "your-wifi-network-name";
const char *passw = "your-wifi-password";
const char *userid = "your-senses-user-id";
const char *key = "your-device-key";

String response;
int slot_number = 1;
float data;

Senses_wifi myiot;

void setup(){
  Serial.begin(9600);
  response = myiot.connect(ssid, passw, userid, key);
  Serial.println(response);
}

void loop(){

  response = "";

  data = random(10,50);
  response = myiot.send(slot_number, data);
  Serial.println(response);

  delay(5000);
}
