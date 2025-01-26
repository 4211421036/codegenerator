/***************************************************
The Provisioning firmware is designed to test the provisioning procedure of new semilimes devices. 
This process starts by sending a request to the server containing the DeviceId and ProvisioningKey 
to register the device.

After the device is successfully registered, the firmware repeatedly sends requests to verify the 
device claim status, ensuring that the user has scanned the QR code. These verification requests 
are sent up to 20 times, with a 5-second delay between each attempt.

If the claim is successful, the server responds with an ApiKey, which the firmware stores in non-volatile 
memory for future connections. This firmware ensures the correct execution of the device registration 
and provisioning process, while securely saving the ApiKey for ongoing connectivity.

This project is licensed under the MIT License.

Dependencies:
	ArduinoJson by Benoit Blanchon 
	Preferences by Volodymyr Shymanskyy
	semilimes by semilimes
 ****************************************************/
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <semilimes.h>
#include <Preferences.h>

//fill the following defines with the DeviceId and ProvisioningKey received from semilimes
#define DeviceId "********-****-****-****-************" 
#define ProvisioningKey "********-****-****-****-************"

Preferences preferences;

Header header;
Provisioning provisioning;

WiFiMulti WiFiMulti;
const uint32_t connectTimeoutMs = 5000;
bool wifi_connected = false;
HTTPClient http;
   
void saveApiKey(const char* ApiKey) 
{
  Serial.println("Saving the Apikey!");
  preferences.begin("authorization", false); //namespace: "authorization" - read/write mode   
  preferences.putString("ApiKey", ApiKey);    
  preferences.end();
}

bool provisioning_device()
{    
  JsonDocument doc;
  String response, apikey;
  
  preferences.begin("authorization", true); //namespace: "authorization" - read-only mode 
  apikey = preferences.getString("ApiKey", "");  
  preferences.end();
  if(apikey=="") //check if ApiKey is already in the non volatile memory
  {
    Serial.println("Device not yet claimed - no ApiKey saved in the non volatile memory of this device.");    
    //Register the device
    provisioning.set(DeviceId, ProvisioningKey);
    http.begin(provisioning.getRegisterDeviceURL());
    http.addHeader("Content-Type",header.getContentType());
    Serial.println("Registering the Device on semilimes server...");
    http.POST(provisioning.get());
    response = http.getString();
    http.end();
    deserializeJson(doc, response);
    if(doc["success"] == true)  //check if the server replied with success
    {      
      Serial.println("Device Registered!");  
      Serial.println("Starting the claiming procedure.");
      Serial.println("\r\n*** Please scan the QR code associated to the DeviceID and the ProvisioningKey ***");
      //wait the QR code to be scanned and the device claimed, it tries x20 times with 5sec of delay each try
      for(int i=0;i<20;i++)
      {
        delay(5000);
        Serial.print(".");
        //send the Claim status request
        http.begin(provisioning.getDeviceClaimStatusURL());
        http.addHeader("Content-Type",header.getContentType());
        http.POST(provisioning.get());
        response = http.getString();
        http.end(); 
        deserializeJson(doc, response);
        if(doc["data"]["authToken"])  //check if the QR code has been scanned by the user
        {
          Serial.println("\r\nDevice Claimed!");
          Serial.println("The ApiKey received will be saved in the non volatile memory of this device!");
          //Serial.print("ApiKey : "); 
          //Serial.println(doc["data"]["authToken"]);  
          saveApiKey(doc["data"]["authToken"]);
          header.setAuthorization(doc["data"]["authToken"]);
          Serial.println("The following Authorization is retreived from the non volative memory of this device:");
          Serial.println(header.getAuthorization());
          return true;
        }     
      }
      Serial.println("Device Unclaimed");    
      return false;
    }
    else
    {
      Serial.println("Device Unegistered!");  
      return false;  
    }
  }
  else
  {
    Serial.println("The Device is aleady Claimed and ApiKey is already saved in non volatile memory!!");
    header.setAuthorization(apikey.c_str());
    Serial.println(header.getAuthorization());
    return true;
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  
  WiFi.mode(WIFI_STA);   
  WiFiMulti.addAP("ssid", "password");

  if (WiFiMulti.run(connectTimeoutMs) == WL_CONNECTED) 
  {
    wifi_connected = true;
    Serial.print("WiFi connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.localIP());
  } 
  else 
  {
    Serial.println("WiFi not connected!");
  }    
}

bool singletone = false;
void loop() 
{
  if(wifi_connected)
  {
    if(singletone==false)
    {
      singletone = true;
  
      Serial.println("******** Provision a new Device ********");
      Serial.println("to test the procedure, the ApiKey saved in the non volatile memory of this device will be deleted.");
      saveApiKey(""); //remove the ApiKey saved in non volatile memory
      Serial.println("Starting the provisioning procedure:");
      provisioning_device();  //start the provisioning procedure of a new device
      delay(5000);
      Serial.println("\r\nStarting the provisioning procedure again, but this time the saved ApiKay will we used!");
      provisioning_device();  //start the provisioning procedure of a new device
    }
  }
}
