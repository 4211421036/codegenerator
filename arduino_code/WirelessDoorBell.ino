/* 
 *  Wireless door bell example with ESP8266-01:
 *  
 *  1. Publish message "/bell/button" with value "true" whenever GPIO0 (board pin 5) triggered.
 *  2. When the push button is being pressed, it must send low signal to GPIO0 (Board pin 5)
 *  3. ESP will immidiately publish "/bell/button" with value "true"
 *  4. When receive message "/bell/button/set" with value "true", bellMe() will pulse low signal on GPIO2 (board pin 3).
 *  
 */

#include "M1128.h"

#define DEBUG true
#define DEBUG_BAUD 9600

#define DEVELOPER_ROOT "1"
#define DEVELOPER_USER "dmI0OkvoFRLRzHu3J3tEWQbIXQwDeF9q"
#define DEVELOPER_PASS "dyUiAb1cjkS8FRrokTXxtY1s4DUmOJsa"

#define WIFI_DEFAULT_SSID "SmartBell"
#define WIFI_DEFAULT_PASS "abcd1234"

#define DEVICE_PIN_RESET 3

#define DEVICE_PIN_BUTTON_DEFSTATE HIGH // default is HIGH, it means active LOW.
#define DEVICE_PIN_BUTTON_INPUT 0 // pin GPIO0 for input
#define DEVICE_PIN_BUTTON_OUTPUT 2 // pin GPio2 for output

HardwareSerial *SerialDEBUG = &Serial;
M1128 iot;

bool pinButtonLastState = DEVICE_PIN_BUTTON_DEFSTATE;
bool pinButtonCurrentState = DEVICE_PIN_BUTTON_DEFSTATE;

void setup() {
  if (DEBUG) {
    //SerialDEBUG->begin(DEBUG_BAUD, SERIAL_8N1, SERIAL_TX_ONLY); // for ESP8266
    SerialDEBUG->begin(DEBUG_BAUD, SERIAL_8N1); // for ESP32
    while (!SerialDEBUG);
    SerialDEBUG->println("Initializing..");
  }
  pinMode(DEVICE_PIN_BUTTON_INPUT,INPUT_PULLUP);
  pinMode(DEVICE_PIN_BUTTON_OUTPUT,OUTPUT);
  digitalWrite(DEVICE_PIN_BUTTON_OUTPUT, DEVICE_PIN_BUTTON_DEFSTATE);
  pinMode(DEVICE_PIN_RESET, FUNCTION_3);
  iot.pinReset = DEVICE_PIN_RESET;
  iot.prod = true;
  iot.cleanSession = true;
  iot.setWill = true;
  iot.apConfigTimeout = 300000;
  iot.wifiConnectTimeout = 120000;
  iot.devConfig(DEVELOPER_ROOT,DEVELOPER_USER,DEVELOPER_PASS);
  iot.wifiConfig(WIFI_DEFAULT_SSID,WIFI_DEFAULT_PASS);
  
  iot.onReceive = callbackOnReceive;
  iot.onConnect = callbackOnConnect;
  iot.onReconnect = callbackOnReconnect;
  iot.onAPConfigTimeout = callbackOnAPConfigTimeout;
  iot.onWiFiConnectTimeout = callbackOnWiFiConnectTimeout;  
  
  iot.init(DEBUG?SerialDEBUG:NULL);
  delay(10);
}

void loop() {
  iot.loop();
  checkBellButton();
}

void callbackOnReceive(char* topic, byte* payload, unsigned int length) {
  String strPayload;
  strPayload.reserve(length);
  for (uint32_t i = 0; i < length; i++) strPayload += (char)payload[i];

  if (DEBUG) {
    SerialDEBUG->print(F("Receiving topic: "));
    SerialDEBUG->println(topic);
    SerialDEBUG->print("With value: ");
    SerialDEBUG->println(strPayload);
  }
  if (strcmp(topic,iot.constructTopic("reset"))==0 && strPayload=="true") iot.reset();
  else if (strcmp(topic,iot.constructTopic("restart"))==0 && strPayload=="true") iot.restart();
  else if (strcmp(topic,iot.constructTopic("bell/button/set"))==0 && strPayload=="true") bellMe();
}

void callbackOnConnect() {
  initPublish();    
  initSubscribe();
}

void callbackOnReconnect() {
  initSubscribe();
}

void callbackOnAPConfigTimeout() {
  //ESP.deepSleep(300000000); // sleep for 5 minutes
  iot.restart();
}

void callbackOnWiFiConnectTimeout() {
  iot.restart();
  //ESP.deepSleep(300000000); // sleep for 5 minutes
}

void checkBellButton() {
  pinButtonCurrentState = digitalRead(DEVICE_PIN_BUTTON_INPUT);
  if (pinButtonLastState==DEVICE_PIN_BUTTON_DEFSTATE && pinButtonCurrentState!=pinButtonLastState) {    
    if (iot.mqtt->connected()) iot.mqtt->publish(iot.constructTopic("bell/button"), "true", false); 
    delay(5000);
  }
  pinButtonLastState = pinButtonCurrentState;  
}

void bellMe() {
  digitalWrite(DEVICE_PIN_BUTTON_OUTPUT, !DEVICE_PIN_BUTTON_DEFSTATE);
  delay(300);
  digitalWrite(DEVICE_PIN_BUTTON_OUTPUT, DEVICE_PIN_BUTTON_DEFSTATE);
  if (iot.mqtt->connected()) iot.mqtt->publish(iot.constructTopic("bell/button"), "true", false);   
}

void publishState(const char* state) {
  if (iot.mqtt->connected()) iot.mqtt->publish(iot.constructTopic("$state"), state, true);  
}

void initPublish() {
  if (iot.mqtt->connected()) {    
    iot.mqtt->publish(iot.constructTopic("$state"), "init", false);
    iot.mqtt->publish(iot.constructTopic("$sammy"), "1.0.0", false);
    iot.mqtt->publish(iot.constructTopic("$name"), "Wireless Bell", false);
    iot.mqtt->publish(iot.constructTopic("$model"), "SAM-WDB01", false);
    iot.mqtt->publish(iot.constructTopic("$mac"), WiFi.macAddress().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$localip"), WiFi.localIP().toString().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$fw/name"), "WDB01", false);
    iot.mqtt->publish(iot.constructTopic("$fw/version"), "1.00", false);    
    iot.mqtt->publish(iot.constructTopic("$reset"), "true", false);
    iot.mqtt->publish(iot.constructTopic("$restart"), "true", false);
    iot.mqtt->publish(iot.constructTopic("$nodes"), "bell", false);
  
  //define node "bell"
    iot.mqtt->publish(iot.constructTopic("bell/$name"), "Bell", false);
    iot.mqtt->publish(iot.constructTopic("bell/$type"), "Bell-01", false);
    iot.mqtt->publish(iot.constructTopic("bell/$properties"), "button", false);

    iot.mqtt->publish(iot.constructTopic("bell/button/$name"), "Bell Button", false);
    iot.mqtt->publish(iot.constructTopic("bell/button/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("bell/button/$retained"), "false", false);
    iot.mqtt->publish(iot.constructTopic("bell/button/$datatype"), "boolean", false);  
  }
}

void initSubscribe() {
  if (iot.mqtt->connected()) {
    iot.mqtt->subscribe(iot.constructTopic("reset"),1);  
    iot.mqtt->subscribe(iot.constructTopic("restart"),1);  
    iot.mqtt->subscribe(iot.constructTopic("bell/button/set"),1);  
  }
  publishState("ready");
}
