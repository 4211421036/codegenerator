#include <WifiConnection.h>
#include <CloudStorage.h>

float measureTempertureInCelsius() {
    // TODO: Implement actual sensor readings
    return 42;
}

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
    
    float temperture = measureTempertureInCelsius();
    // `put_min` will only update "coolest_temperture" if `temperture` is 
    // actually smaller than what is already stored in the server
    float minTemp = storage.put_min<float>("coolest_temperture", temperture);

    // `put_max` acts like `put_min` but will only update if the given
    // value is greater than what is already present in the server
    float maxTemp = storage.put_max<float>("hottest_temperture", temperture);

    Serial.print("Coolest temp: ");
    Serial.println(minTemp);

    Serial.print("Hottest temp: ");
    Serial.println(maxTemp);

  } else {
    Serial.println("No Connection");
  }

  // Wait 10 seconds between updates
  delay(10 * 1000);
}
