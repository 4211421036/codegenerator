#include <WifiConnection.h>
#include <CloudStorage.h>

/*
* NOTE: make sure to replace all parameters for CloudStorage and Wifi
*/
#define INPUT_PIN_NUMBER 8

CloudStorage storage("BASE_URL", "USERNAME", "PASSWORD");

bool lastSwitchState;

void setup() {
  Serial.begin(115200);
  pinMode(INPUT_PIN_NUMBER, INPUT);
  lastSwitchState = digitalRead(INPUT_PIN_NUMBER);

  // Try to connect to a wifi network
  WifiConnection::tryConnect("WIFI_SSID", "WIFI_PASSWORD");  

  // Give the ESP some time to connect
  delay(3 * 1000);
}

void loop() {
  bool newSwitchState = digitalRead(INPUT_PIN_NUMBER);
  
  // if input state has changed
  if(newSwitchState != lastSwitchState && WifiConnection::isConnected()) {
    
    // store the new state in the server
    bool isOk = storage.put<bool>("switch_state", newSwitchState);
    
    // Make sure `put` was successful
    if(isOk) {
      // save current time in the server in the key "last_update_time"
      // `datetime` also returns the newly stored value 
      String time = storage.datetime("last_update_time");
      Serial.println("Last Update Time in the server now equals to: ");
      Serial.println(time);
    } else {
      Serial.println("Something went wrong");
    }

    // remember last state
    lastSwitchState = newSwitchState;
  } else if(WifiConnection::isConnected() == false) {
    Serial.println("No Connection");
  }

  delay(1000);
}
