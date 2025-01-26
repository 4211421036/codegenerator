#include <SoftwareSerial.h>
#include <stdlib.h>

SoftwareSerial ESP8266(2, 3);
unsigned char checkConnected = 0;
unsigned char timesCheck = 0;
void setup() {
  Serial.begin(115200);
  ESP8266.begin(115200);

  ESP8266.print("**VER: ");
  delay(2000);
  ESP8266.println("AT+RST");
  delay(1000);
  ESP8266.println("AT+CWMODE=3");
  delay(1000);
  ESP8266.println("AT+CWLAP");
  delay(1000);
  // put your setup code here, to run once:

}

void loop() {
  Serial.println("Connecting to WiFi");
  while(checkConnected==0){
    Serial.print(".");
    ESP8266.println("AT+CWJAP=\"Galaxy\",\"fkhw8785\"\r\n");
    ESP8266.setTimeout(5000);
    if(ESP8266.find("WIFI CONNECTED")){
      Serial.print("WIFI CONNECTED");
      break;
    };
    timesCheck++;
    if(timesCheck>3){
      timesCheck=0;
      Serial.println("Trying to Reconnect...");
    }
  }
  while(1);

}
