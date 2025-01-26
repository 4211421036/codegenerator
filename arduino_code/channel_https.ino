/***************************************************
The channel_websocket firmware is designed to demonstrate the implementation 
of the semilimes SDK and establish communication with a server via https. 

This example firmware, once connected to a Wi-Fi network and thereby to the 
internet, establishes an https connection to a server. 

Upon establishing the connection, the firmware check if the channelname already 
exists and eventually creates a new channel in the subaccount associated with 
the provided API key. 
Once the channel is successfully created, it sends a form containing two form 
components: a label and a button list. 

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
	semilimes by semilimes
 ****************************************************/
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include <Arduino.h>
#include <ArduinoJson.h>
#include <semilimes.h>
    
bool WiFiConnected = false;

Header header;

WiFiMulti WiFiMulti;
const uint32_t connectTimeoutMs = 5000;
HTTPClient http;

String channelId;

String httpsPost(String ep_url, String json)
{
  http.begin(ep_url);
  http.addHeader("accept",header.getAccept());
  http.addHeader("Content-Type",header.getContentType());
  http.addHeader("Authorization",header.getAuthorization());
  http.POST(json);  
  String response = http.getString();
  http.end();    
  return response;
}

void createChannelIfNew(String chName)
{        
  bool chNew = true;
  JsonDocument doc;

  //check if chName already exists
  ChannelsMyGet channelsmyget;
  channelsmyget.set(true,true,true);
  deserializeJson(doc, httpsPost(channelsmyget.getEPurl(),channelsmyget.get()));
  for(int i=0; i<doc["data"].size(); i++)
  {
    if(doc["data"][i]["title"].as<String>() == chName) 
    {
      chNew=false;
      Serial.println("Channel already exists!!");      
      channelId = doc["data"][i]["channelId"].as<String>();
      break;
    }
  }  
  if(chNew)
  {
    //create a new channel
    ChannelCreate channelcreate; //create a new "CreateChannel" 
    channelcreate.set(chName.c_str(),"",false,true); //name="New Channel", avatar="", visible=false, locked=true
    deserializeJson(doc, httpsPost(channelcreate.getEPurl(),channelcreate.get()));
    if(doc["data"]["createdChannel"]["channelId"])
    {
      channelId = doc["data"]["createdChannel"]["channelId"].as<String>();
    }
  }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);
    delay(200);
      
    //devWiFiConnected = wifiProvisioning(rebootPin, led);  
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
      header.setAuthorization("*********************************************************************************");
    }    
    createChannelIfNew("New Channel from https");

    Serial.print("New Channel Id: ");
    Serial.println(channelId);

    //**********************************************
    //send a message to the channel
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

    httpsPost(channelmessagesend.getEPurl(),channelmessagesend.get());
}

void loop() 
{
}
