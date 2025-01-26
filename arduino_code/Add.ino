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

void loop() {
  Serial.println("Checking Connection");
  if (WifiConnection::isConnected()) {
    
    // add that value to array in the server
    bool isOk = storage.add("times_alive", millis());
    
    // notify if error occurred
    if(isOk == false) {
      Serial.println("Something went wrong...");  
    }

  } else {
    Serial.println("No Connection");
  }

  delay(5 * 1000);
}
