/***************************************************
The channel_websocket firmware is designed to demonstrate the implementation 
of the semilimes SDK and establish communication with a server via WebSocket. 

This example firmware, once connected to a Wi-Fi network and thereby to the 
internet, establishes a WebSocket connection to a server, utilizing callback functions. 

Upon establishing the connection, the firmware check if the channelname already 
exists and eventually creates a new channel in the subaccount associated with 
the provided API key. 
Once the channel is successfully created, it sends a form containing two form 
components: a label and a button list. 

Through the use of callbacks, it is possible to print messages sent to the channel 
via the app, including those related to interactions with the newly created form, 
directly to the serial output.

Unlike the example firmware called "provisioning," where the API key is sent from the 
server after scanning a QR code, in the channel_websocket firmware, the API key is
directly embedded in the firmware. You can request an API key from the following 
page: https://my.semilimes.net/apikeys.
The advantage of using provisioning is that it allows you to create devices intended 
for third-party use without the need to share the API key. During the provisioning 
process, ownership of the device is established, ensuring that the API key remains 
secure and the device is correctly assigned to the user or account during the 
provisioning phase.

This project is licensed under the MIT License.

Dependencies:
	ArduinoJson by Benoit Blanchon 
	WebSockets_Generic by Markus Sattler 
	semilimes by semilimes
 ****************************************************/
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

#include <Arduino.h>
#include <ArduinoJson.h>
#include <semilimes.h>
    
bool wsConnected = false;
bool WiFiConnected = false;
bool newMsg = false;

Header header;

WiFiMulti WiFiMulti;
const uint32_t connectTimeoutMs = 5000;
WebSocketsClient webSocket;
#define _WEBSOCKETS_LOGLEVEL_     2

String channelId;
JsonDocument doc;

void setup()
{
    Serial.begin(115200);
    while (!Serial);
    delay(200);
      
    WiFi.mode(WIFI_STA);   
    WiFiMulti.addAP("ssid", "password");
    if (WiFiMulti.run(connectTimeoutMs) == WL_CONNECTED) 
    {
      WiFiConnected = true;
      Serial.print("WiFi connected: ");
      Serial.print(WiFi.SSID());
      Serial.print(" ");
      Serial.println(WiFi.localIP());
    } else 
    {
      Serial.println("WiFi not connected!");
    }  

    if(WiFiConnected==true)
    {      
      header.setAuthorization("********** ApiKey **********");
      webSocket.beginSSL(header.getWsHost(),header.getWsPort(),header.getWsEP()); //init the websocket
      webSocket.setExtraHeaders(header.getWsHeader());  //feed the ws_client with the authorization header, needed to connect to the semilimes server 
      webSocket.onEvent(webSocketEvent);  //initialize the websocket client callback event manager
    }  
}

void webSocketEvent(const WStype_t& type, uint8_t * payload, const size_t& length)
{
  switch(type)
  {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      wsConnected = true;
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] message received: %s\n", payload);
      deserializeJson(doc, (char*)payload);
      newMsg=true;
      break;
    case WStype_BIN:
      break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
    default:
      break;
  }
}

void loop() 
{
  static int state=0;
  WebsocketHeader websocketheader;  //create a new "WebsocketHeader" object and feed it with the pointer to the char-array that will contain the json
  
  webSocket.loop();

  String chName="New Channel from WS";
  if(wsConnected==true && WiFiConnected==true)
  {
    if(state==0)
    {
      Serial.println("state 0, channelsmyget request");
      ChannelsMyGet channelsmyget;
      channelsmyget.set(true,true,true);
      webSocket.sendTXT(websocketheader.getRequest("reqId_1",channelsmyget.getWSEPurl(), channelsmyget.get()));
      state=1;
    }
    else if(state==1)  //check if channel already exists
    {
      if(newMsg==true)
      {
        Serial.println("state 1, check channel name");
        newMsg=false;      
        for(int i=0; i<doc["eventBody"]["data"].size(); i++)
        {
          if(doc["eventBody"]["data"][i]["title"].as<String>() == chName) 
          {
            Serial.println("The channel already exists!!");      
            channelId = doc["eventBody"]["data"][i]["channelId"].as<String>();
            state=4; //send the msg
            break;
          }
          else
          {
            state=2; //create a new channel
          }
        }
        if(state==2) Serial.println("The channel doesn't exist!!");    
      }
    }
    else if(state==2)  //create a new channel
    {
      Serial.println("Create new hannel");   
      //create a new channel
      ChannelCreate channelcreate; //create a new "CreateChannel" 
      channelcreate.set(chName.c_str(),"",false,true); //name="New Channel", avatar="", visible=false, locked=true
      webSocket.sendTXT(websocketheader.getRequest("reqId_2", channelcreate.getWSEPurl(), channelcreate.get()));
      state=3;
    }
    else if(state==3)  //wait till the channel has been created 
    {    
      if(newMsg==true)
      {
        newMsg=false;    
        Serial.println("wait till the channel has been created");     
        if(doc["eventBody"]["data"]["createdChannel"]["channelId"])
        {
          Serial.println("Channel created!!");      
          channelId = doc["eventBody"]["data"]["createdChannel"]["channelId"].as<String>();
          state=4; //send the msg
        }
      }
    }
    else if(state==4)  //send a message to the channel
    {
      Serial.println("Sending the message to the channel"); 
      //**********************************************
      //send a message to the newly created channel
      //**********************************************
      
      //create a new Form    
      DcForm form; //create a new "DcForm" object and feed it with the pointers to the char-arrays that will contain the json 
      form.set(channelId.c_str(),form.featureType[2],false,false,"","refname",form.align[0],false,true); //(channelId, featureType="channel",submitEnabled=false, retainStatus=false, "", refName,align, authorizeSubmit, hideSubmissionMsg)
      
      //create Label Form Component
      FcLabel fclabel; //create a new "FcLabel" object and feed it with the pointers to the char-arrays that will contain the json
      fclabel.set("Label","Example Form on a newly created Channel"); //(refname,title)
      //Serial.println(fclabel.get());
      form.addFormComponents(fclabel.get()); //add the "fclabel" to the FormComponents of "form"
      
      //create a button list Form Component
      FcButtonList fcbuttonlist; //create a new "FcButtonList" object and feed it with the pointers to the char-arrays that will contain the json
      fcbuttonlist.set("myButtonList","Button List",false,"buttonName",true,fcbuttonlist.lineSize[1]); //(refname, title, required selection, value, vertList, lineSize)
      fcbuttonlist.addOptions("button1","choice1",""); //add the first button (name, value, iconName)  
      fcbuttonlist.addOptions("button2","choice2",""); //add the second button (name, value, iconName)
      fcbuttonlist.appendOptions(); //append the buttons to the "fcbuttonlist"
      //Serial.println(fcbuttonlist.get());
      form.addFormComponents(fcbuttonlist.get()); //add the "fcbuttonlist" to the FormComponents of "form"
      
      form.appendFormComponents(); //append the "added" Form Components (fclabel and fcbuttonlist) to the "form"
      //Serial.println(form.get());
      
      //send message to channel
      ChannelMessageSend channelmessagesend; //create a new "ChannelMessageSend" object and feed it with the pointers to the char-array that will contain the json
      channelmessagesend.set(channelId.c_str(),form.get(),false); //(channelId, dataComponent, silent)
      //Serial.println(channelmessagesend.get());
      //Serial.println(channelmessagesend.getEPurl());
      
      webSocket.sendTXT(websocketheader.getRequest("reqId_3", channelmessagesend.getWSEPurl(), channelmessagesend.get())); //embeds the "channelmessagesend" json into the "websocketheader" and send the message to the sme server (request id: "reqId_3")      
      state=5;
    }
    else if(state==5)  //receive messages
    {    
      if(newMsg==true)
      {
        newMsg=false;
      }
    }
  }
}
