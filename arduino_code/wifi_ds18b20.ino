#include "Senses_wifi.h"
#include <OneWire.h>
#include <DallasTemperature.h>

const char *ssid = "your-wifi-network-name";
const char *passw = "your-wifi-password";
const char *userid = "your-senses-user-id";
const char *key = "your-registered-key";

#define ONE_WIRE_BUS D2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

String response;
float data;

float t;

Senses_wifi myiot;

void setup(){
  Serial.begin(9600);
  response = myiot.connect(ssid, passw, userid, key);
  Serial.println(response);
}

void loop(){

  response = "";

  sensors.requestTemperatures();
  Serial.println("done !");
  t = sensors.getTempCByIndex(0);
  Serial.println("t = " + String(t));
  
  response = myiot.send(1,t); /* myiot.send(slotnumber, data) */
  Serial.println(response);

  delay(10000);
}
