/* 
 *  Smart relay example with ESP8266-01:
 *  
 *  1. Receive message "/relay/onoff/set" with value "true" or "false".
 *  2. When "true" it will trigger LOW to GPIO2, otherwise trigger HIGH.
 *  
 */

#include "M1128.h"

#define DEBUG true
#define DEBUG_BAUD 9600

#define DEVELOPER_ROOT "1"
#define DEVELOPER_USER "dmI0OkvoFRLRzHu3J3tEWQbIXQwDeF9q"
#define DEVELOPER_PASS "dyUiAb1cjkS8FRrokTXxtY1s4DUmOJsa"

#define WIFI_DEFAULT_SSID "SmartRelay"
#define WIFI_DEFAULT_PASS "abcd1234"

#define DEVICE_PIN_RESET 3

#define DEVICE_PIN_BUTTON_DEFSTATE HIGH // default is HIGH, it means active LOW.
#define DEVICE_PIN_BUTTON_OUTPUT 2 // pin GPIO2 for output

HardwareSerial *SerialDEBUG = &Serial;
M1128 iot;

void setup() {
  if (DEBUG) {
    //SerialDEBUG->begin(DEBUG_BAUD, SERIAL_8N1, SERIAL_TX_ONLY); // for ESP8266
    SerialDEBUG->begin(DEBUG_BAUD, SERIAL_8N1); // for ESP32
    while (!SerialDEBUG);
    SerialDEBUG->println("Initializing..");
  }
  pinMode(DEVICE_PIN_BUTTON_OUTPUT,OUTPUT);
  //digitalWrite(DEVICE_PIN_BUTTON_OUTPUT, DEVICE_PIN_BUTTON_DEFSTATE);
  pinMode(DEVICE_PIN_RESET, FUNCTION_3);
  iot.pinReset = DEVICE_PIN_RESET;
  iot.prod = true;
  iot.cleanSession = false;
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
  else if (strcmp(topic,iot.constructTopic("relay/onoff/set"))==0 && strPayload=="true") switchMe(!DEVICE_PIN_BUTTON_DEFSTATE,true);
  else if (strcmp(topic,iot.constructTopic("relay/onoff/set"))==0 && strPayload=="false") switchMe(DEVICE_PIN_BUTTON_DEFSTATE,true);
  else if (strcmp(topic,iot.constructTopic("relay/onoff"))==0 && strPayload=="true") switchMe(!DEVICE_PIN_BUTTON_DEFSTATE,false);
  else if (strcmp(topic,iot.constructTopic("relay/onoff"))==0 && strPayload=="false") switchMe(DEVICE_PIN_BUTTON_DEFSTATE,false);  
}

void callbackOnConnect() {
  initPublish();    
  initSubscribe();
}

void callbackOnReconnect() {
  initSubscribe();
}

void callbackOnAPConfigTimeout() {
  iot.restart();
}

void callbackOnWiFiConnectTimeout() {
  iot.restart();
  //ESP.deepSleep(300000000); // sleep for 5 minutes
}

void switchMe(bool sm, bool publish) {
  digitalWrite(DEVICE_PIN_BUTTON_OUTPUT, sm);
  if (publish && iot.mqtt->connected()) iot.mqtt->publish(iot.constructTopic("relay/onoff"), sm?"false":"true", true);   
}

void publishState(const char* state) {
  if (iot.mqtt->connected()) iot.mqtt->publish(iot.constructTopic("$state"), state, true);  
}

void initPublish() {
  if (iot.mqtt->connected()) {    
    iot.mqtt->publish(iot.constructTopic("$state"), "init", false);
    iot.mqtt->publish(iot.constructTopic("$sammy"), "1.0.0", false);
    iot.mqtt->publish(iot.constructTopic("$name"), "Smart Relay", false);
    iot.mqtt->publish(iot.constructTopic("$model"), "SAM-SMR01", false);
    iot.mqtt->publish(iot.constructTopic("$mac"), WiFi.macAddress().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$localip"), WiFi.localIP().toString().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$fw/name"), "WDB01", false);
    iot.mqtt->publish(iot.constructTopic("$fw/version"), "1.00", false);    
    iot.mqtt->publish(iot.constructTopic("$reset"), "true", false);
    iot.mqtt->publish(iot.constructTopic("$restart"), "true", false);
    iot.mqtt->publish(iot.constructTopic("$nodes"), "relay", false);
  
  //define node "relay"
    iot.mqtt->publish(iot.constructTopic("relay/$name"), "Relay", false);
    iot.mqtt->publish(iot.constructTopic("relay/$type"), "Relay-01", false);
    iot.mqtt->publish(iot.constructTopic("relay/$properties"), "onoff", false);

    iot.mqtt->publish(iot.constructTopic("relay/onoff/$name"), "Relay On Off", false);
    iot.mqtt->publish(iot.constructTopic("relay/onoff/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("relay/onoff/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("relay/onoff/$datatype"), "boolean", false);  
  }
}

void initSubscribe() {
  if (iot.mqtt->connected()) {
    // subscribe listen
    iot.mqtt->subscribe(iot.constructTopic("reset"),1);  
    iot.mqtt->subscribe(iot.constructTopic("restart"),1);  
    iot.mqtt->subscribe(iot.constructTopic("relay/onoff"),1);  
    iot.mqtt->subscribe(iot.constructTopic("relay/onoff/set"),1);  
  }
  publishState("ready");
}
