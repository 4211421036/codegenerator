#include <WifiConnection.h>
#include <CloudStorage.h>

/*
* NOTE: make sure to replace all parameters for CloudStorage and Wifi
*/

CloudStorage storage("BASE_URL", "USERNAME", "PASSWORD");
#define LED_PIN 0

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  // Try to connect to a wifi network
  WifiConnection::tryConnect("WIFI_SSID", "WIFI_PASSWORD");


  // Give the ESP some time to connect
  delay(10 * 1000);

  Serial.print("Connected? ");
  Serial.println(WifiConnection::isConnected());

  // listen for changes on key `led_state`
  storage.listen("led_state");
  storage.onChange([&](String key, AnyValue newValue){
      if(key == "led_state") {
          // get the new state
          bool state = newValue.as<bool>();
          
          // log to serial
          Serial.print("Changing led to: ");
          Serial.println(state);

          // update led
          digitalWrite(LED_PIN, state);
      }
  });
}

void loop() {
  // Check for connection
  if (WifiConnection::isConnected()) {
      // if you want real time updates, you must call `loop` frequently
      storage.loop();
  } else {
    Serial.println("No Connection");
  }
}
