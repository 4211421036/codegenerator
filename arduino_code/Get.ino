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
    
    // Get value of "age" without checking for errors
    int age = storage.get<int>("age");
    Serial.println(age);

    // Get the packed result and check for errors
    auto name = storage.get<String>("name");
    if(name.isOk == false) {
        Serial.println("Error getting name");
    } else {
        Serial.println(name.value);
    }

  } else {
    Serial.println("No Connection");
  }

  // Wait 10 seconds between delays
  delay(10 * 1000);
}
