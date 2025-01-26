#include <Wire.h>
#include <Ciao.h>

/* 

This example show the interaction between the Ciao Library and the Thingspeak Cloud. 
To run the example you need to register an account on thingspeak.com and create a 
new channel by clicking "Channels" section in the website (Channels -> My Channels -> New Channel). 
In the new channel you need to add two fields. The first one refers to the humidity data and the second one to the temperature value.
After that, replace the "XXXXXXXXX" value of APIKEY_THINGSPEAK with "Write API key" value reported in the API Keys section of the channel. 

*/

#define CONNECTOR     "rest" 
#define SERVER_ADDR   "api.thingspeak.com"

#define APIKEY_THINGSPEAK  "XXXXXXXXXXXXX"

short hum = 60;
short temp = 22;

void setup() {

  Ciao.begin(); // CIAO INIT
}

void loop() {
      
    String uri = "/update?api_key=";
    uri += APIKEY_THINGSPEAK;
    uri += "&field1=";
    uri += String(hum);
    uri += "&field2=";
    uri += String(temp);

    Ciao.println("Send data on ThingSpeak Channel"); 
      
    CiaoData data = Ciao.write(CONNECTOR, SERVER_ADDR, uri);

    if (!data.isEmpty()){
      Ciao.println( "State: " + String (data.get(1)) );
      Ciao.println( "Response: " + String (data.get(2)) );
    }
    else{ 
      Ciao.println("Write Error");
    }    
 
  delay(30000); // Thinkspeak policy

}  
