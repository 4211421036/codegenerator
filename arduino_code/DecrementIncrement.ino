#include <WifiConnection.h>
#include <CloudStorage.h>

/*
* NOTE: make sure to replace all parameters for CloudStorage and Wifi
*/
#define BUTTON_PIN_1 8
#define BUTTON_PIN_2 9

CloudStorage storage("BASE_URL", "USERNAME", "PASSWORD");
int score = 0;

bool isButton1Pressed() {
    return digitalRead(BUTTON_PIN_1) == false;
}

bool isButton2Pressed() {
    return digitalRead(BUTTON_PIN_2) == false;
}


void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  
  // Try to connect to a wifi network
  WifiConnection::tryConnect("WIFI_SSID", "WIFI_PASSWORD");  

  // Give the ESP some time to connect
  delay(5 * 1000);

  score = storage.get<int>("counter");
}

void loop() {
  if(WifiConnection::isConnected() == false) {
    Serial.println("No Connection");
    return;
  }

  // if first button is pressed, increment the counter
  if(isButton1Pressed()) {
      score = storage.inc("counter");
  }

  // if second button is pressed, decrement the counter
  if(isButton2Pressed()) {
      score = storage.dec("counter");
  }

  Serial.print("Latest Score: ");
  Serial.println(score);

  delay(500);
}
