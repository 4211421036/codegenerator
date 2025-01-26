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
  delay(5 * 1000);
}

void loop() {
  // Check for connection
  if (WifiConnection::isConnected()) {
    
    // Put a number value in the server with the key "age"
    bool isOk = storage.put<int>("age", 20);
    
    // notify if error occurred
    if(isOk == false) {
      Serial.println("Something went wrong...");  
    }

    // Put a string value in the server with the key "name"
    isOk = storage.put<String>("name", "Chuck Norris");
    
    // notify if error occurred
    if(isOk == false) {
      Serial.println("Something went wrong...");  
    }
  
  } else {
    Serial.println("No Connection");
  }

  // Wait 10 seconds between delays
  delay(10 * 1000);
}
