/*
Pointznet platform, esp8266 code.
Version : 27.Aug.2017, 15:00
*/

#include <PointzNet.h>

const char *ssid = "";// "wifi ssid"                    
const char *password = ""; // "wifi password"
const char *username = ""; // user1, user2, user3

  float d0 = 0;
  float d1 = 0;
  String lat = "37.8344203";  // Lat. as string
  String lng = "-122.427631"; // Lng. as string

pointznet iot;

void setup() {

  Serial.begin(115200);

  bool conn = iot.begin(ssid, password);
  if(!conn)
    {
      Serial.println("Pointnet connect fails.");
    }else
    { 
      Serial.println("Pointznet connected !.");
     }

}

void loop() {
  
  /* Send data to dashboard */
  String respData = iot.Senddata(username, d0, d1, lat, lng);

  /* Send notify to display at dashboard (Available fo user1, user2 & user3) */
  String respNotify = iot.Sendnotify(username, "Hello from IoT device to Pointznet");
  
  delay(10000);

}
