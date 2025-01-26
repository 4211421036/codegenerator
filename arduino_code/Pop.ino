#include <WifiConnection.h>
#include <CloudStorage.h>

/*
* NOTE: make sure to replace all parameters for CloudStorage and Wifi
*/

CloudStorage storage("BASE_URL", "USERNAME", "PASSWORD");

void setup() {
  Serial.begin(115200);
  
  // Try to connect to a wifi network
  WifiConnection::tryConnect("WIFI_SSID", "WIFI_PASSWORD");  

  // Give the ESP some time to connect
  delay(3 * 1000);
}

bool haveMoreItems = true;

void loop() {
  // if nothing else to fetch, sleep for a minute and try again
  // in case someone added more items to the collection in the server
  if(haveMoreItems == false) {
    Serial.println("Nothing else to pop!");
    delay(60 * 1000);
    haveMoreItems = true;
    return; 
  }
  
  Serial.println("Checking Connection");
  if (WifiConnection::isConnected()) {

    // Pop the last item from the array
    auto popResult = storage.pop<String>("things_to_say", PopFrom_End);
    
    if(popResult.isOk) {
      haveMoreItems = popResult.hasNext;
      Serial.println(popResult.value);
    } else {
      // notify if error occurred
      Serial.println("Something went wrong...");  
    
      // Error could occurre when no more items are present
      haveMoreItems = false;
    }

  } else {
    Serial.println("No Connection");
  }

  delay(5 * 1000);
}
