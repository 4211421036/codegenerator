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
  
  // initialize value to 0
  if (WifiConnection::isConnected()) {
    storage.put<int>("count", 0);
  }

}

void loop() {
  Serial.println("Checking Connection");
  if (WifiConnection::isConnected()) {
    
    // Get the current value
    int val = storage.get<int>("count");

    // Log
    Serial.print("Got value: ");
    Serial.print(val);
    Serial.println(" . Updating...");
    
    // Increment by 1 and store in the server
    storage.put<int>("count", val + 1);
    
  } else {
    Serial.println("No Connection");
  }

  delay(1000);
}
