#include <GSheet32.h>
GSheet32 Sheet("your GAS_ID");

const char* ssid = "your WiFi ssid"; 
const char* password = "your WiFi password";

void setup() {
  Serial.begin(115200);
  Sheet.connectWiFi(ssid, password);
  Sheet.clearData(); // function to delete all data in the sheet
  // Add // if you don't want it to be used
}

void loop() {
  // Adjust to the needs of your column, maximum data that can be sent in this version is 5 columns
  // Sheet.sendData(column C,column D, column E,column F,column G);
  Sheet.sendData("Hello","World","My","name","is");

  // if you want to add space in 1 column then change the space to %20, or you can type it with the help of url encode
  Sheet.sendData("Hello%20World");
}
