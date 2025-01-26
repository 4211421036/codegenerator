/*
  This code already tested with IoT board : WEMOS D1 mini
  If use another, please correct some parameter.
*/

#include "Senses_wifi.h"
#define relay_pin D1

const char *ssid = "your-wifi-network-name";
const char *passw = "your-wifi-password";
const char *userid = "your-senses-user-id"; /* You can get it at https://www.sensesiot.com/accountinfo */
const char *key = "your-device-key";  /* You can get it at https://www.sensesiot.com/myiotgarage */

String response;
int control_port = 1;

Senses_wifi myiot;

void setup(){
  Serial.begin(9600);
  pinMode(relay_pin, OUTPUT);
  response = myiot.connect(ssid, passw, userid, key);
  Serial.println(response);
}

void loop(){
  
  response = myiot.getDigitalControl(control_port);
  Serial.println(response);

    if(response=="on"){
      digitalWrite(relay_pin, HIGH);
      }else{
        //..
     } 
  
    if(response=="off"){
      digitalWrite(relay_pin, LOW);
      }else{
        //..  
     }

  delay(1000);
  
}
