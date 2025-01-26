#include <pm25senses.h>

const char *ssid = "your-wifi-ssid";
const char *passw = "your-wifi-password";

String sendername;
String lat, lng;
float pm25, pm10;

String response; 

pm25senses mydevice;

void setup() {

  Serial.begin(9600);
  
  mydevice.begin(ssid, passw);
  response = mydevice.checkServerReady();
  Serial.println("PM25Senses response :" + String(response)); 
  
}

void loop() {

  /* Set your lat, lng, pm25, pm10 here */
  // Please note, This is Mock up data only
  // Please replace your data here.
  sendername = "your-reporter-name";
  lat = "your-latitude";  
  lng = "your-longitude";
  pm25 = 25;
  pm10 = 90;

  response = mydevice.reportPM25senses(sendername, lat, lng, pm25, pm10);
  Serial.println(response);
  
  delay(10000);
}
