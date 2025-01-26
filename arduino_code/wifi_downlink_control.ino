/*
  This code already tested with IoT board : WEMOS D1 mini
  If use another, please correct some parameter.
*/

#include "Senses_wifi.h"

const char *ssid = "your-wifi-network-name";
const char *passw = "your-wifi-password";
const char *userid = "your-senses-user-id"; /* You can get it at https://www.sensesiot.com/accountinfo */
const char *key = "your-device-key"; /* You can get it at https://www.sensesiot.com/myiotgarage */

String response;
int control_port = 1;

Senses_wifi myiot;

void setup(){
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  response = myiot.connect(ssid, passw, userid, key);
  Serial.println(response);
}

void loop(){
  
  response = myiot.getDigitalControl(control_port);
  Serial.println(response);

    if(response=="on"){
      digitalWrite(LED_BUILTIN, LOW);
      }else{
        //..
     } 
  
    if(response=="off"){
      digitalWrite(LED_BUILTIN, HIGH);
      }else{
        //..  
     }

  delay(1000);
  
}
