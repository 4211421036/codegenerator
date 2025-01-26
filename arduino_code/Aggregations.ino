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
    
    float averageTemperture = storage.avg("today_temperture_samples");
    float highestTemperture = storage.max<float>("today_temperture_samples");
    float lowestTemperture = storage.min<float>("today_temperture_samples");
    int numSamples = storage.count("today_temperture_samples");

    Serial.print("Today's tempertures ranged from ");
    Serial.print(lowestTemperture); 
    Serial.print(" degrees to ");
    Serial.print(highestTemperture);
    Serial.println(" degrees celsius.");

    Serial.print("With the average ");
    Serial.print(averageTemperture);
    Serial.println(" degrees celsius.");

    Serial.print("Total of ");
    Serial.print(numSamples);
    Serial.println(" samples.");

  } else {
    Serial.println("No Connection");
  }

  delay(5 * 1000);
}
