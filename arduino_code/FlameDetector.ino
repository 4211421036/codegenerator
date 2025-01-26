/* 
 *  Gas Leakage Alarm example with ESP8266-01:
 *  
 *  Publish message "/sensor/lpg" with value "true" whenever GPIO0 (board pin 5) triggered with low signal.
 *  
 */
 
#include "M1128.h"

#define DEBUG true //true if you want to debug.
#define DEBUG_BAUD 9600 //debug baud rate

#define DEVELOPER_ROOT "1"
#define DEVELOPER_USER "dmI0OkvoFRLRzHu3J3tEWQbIXQwDeF9q"
#define DEVELOPER_PASS "dyUiAb1cjkS8FRrokTXxtY1s4DUmOJsa"

#define WIFI_DEFAULT_SSID "FlameAlarm"
#define WIFI_DEFAULT_PASS "abcd1234"

#define DEVICE_PIN_RESET 3

#define DEVICE_PIN_DEFSTATE HIGH //initial/default pin state
#define DEVICE_PIN_INPUT 0 //GPIO pin input
#define DEVICE_PIN_OUTPUT 2 // pin GPIO2 for output

HardwareSerial *SerialDEBUG = &Serial;
M1128 iot;

bool pinLastState = DEVICE_PIN_DEFSTATE;
bool pinCurrentState = DEVICE_PIN_DEFSTATE;

void setup() {
  if (DEBUG) {
    //SerialDEBUG->begin(DEBUG_BAUD, SERIAL_8N1, SERIAL_TX_ONLY); // for ESP8266
    SerialDEBUG->begin(DEBUG_BAUD, SERIAL_8N1); // for ESP32
    while (!SerialDEBUG);
    SerialDEBUG->println("Initializing..");
  }
  pinMode(DEVICE_PIN_INPUT,INPUT);
  pinMode(DEVICE_PIN_OUTPUT,OUTPUT);
  digitalWrite(DEVICE_PIN_OUTPUT, DEVICE_PIN_DEFSTATE);
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
  checkSensor();
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
}

void checkSensor() {
  pinCurrentState = digitalRead(DEVICE_PIN_INPUT);
  if (pinLastState==!DEVICE_PIN_DEFSTATE && pinCurrentState!=pinLastState) { // alarm ON   
    digitalWrite(DEVICE_PIN_OUTPUT, !DEVICE_PIN_DEFSTATE); // make noisy board alarm
    if (iot.mqtt->connected()) iot.mqtt->publish(iot.constructTopic("sensor/flame"), "true", true); 
  } else if (pinLastState==DEVICE_PIN_DEFSTATE && pinCurrentState!=pinLastState) {  // alarm OFF  
    digitalWrite(DEVICE_PIN_OUTPUT, DEVICE_PIN_DEFSTATE); // turn off board alarm
    if (iot.mqtt->connected()) iot.mqtt->publish(iot.constructTopic("sensor/flame"), "false", true); 
  }
  pinLastState = pinCurrentState;  
}

void publishState(const char* state) {
  if (iot.mqtt->connected()) iot.mqtt->publish(iot.constructTopic("$state"), state, true);  
}

void initPublish() {
  if (iot.mqtt->connected()) {     
    iot.mqtt->publish(iot.constructTopic("$state"), "init", false);
    iot.mqtt->publish(iot.constructTopic("$sammy"), "1.0.0", false);
    iot.mqtt->publish(iot.constructTopic("$name"), "Flame Alarm", false);
    iot.mqtt->publish(iot.constructTopic("$model"), "SAM-FLA01", false);
    iot.mqtt->publish(iot.constructTopic("$mac"), WiFi.macAddress().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$localip"), WiFi.localIP().toString().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$fw/name"), "FLA01", false);
    iot.mqtt->publish(iot.constructTopic("$fw/version"), "1.00", false);    
    iot.mqtt->publish(iot.constructTopic("$reset"), "true", false);
    iot.mqtt->publish(iot.constructTopic("$restart"), "true", false);
    iot.mqtt->publish(iot.constructTopic("$nodes"), "sensor", false);
  
  //define node "sensor"
    iot.mqtt->publish(iot.constructTopic("sensor/$name"), "Sensor", false);
    iot.mqtt->publish(iot.constructTopic("sensor/$type"), "Sensor-01", false);
    iot.mqtt->publish(iot.constructTopic("sensor/$properties"), "flame", false);

    iot.mqtt->publish(iot.constructTopic("sensor/flame/$name"), "FLAME", false);
    iot.mqtt->publish(iot.constructTopic("sensor/flame/$settable"), "false", false);
    iot.mqtt->publish(iot.constructTopic("sensor/flame/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor/flame/$datatype"), "boolean", false);  

  // set device to ready
    iot.mqtt->publish(iot.constructTopic("$state"), "ready", false);  
  }
}

void initSubscribe() {
  if (iot.mqtt->connected()) { 
    iot.mqtt->subscribe(iot.constructTopic("reset"),1);  
    iot.mqtt->subscribe(iot.constructTopic("restart"),1);  
  }
  publishState("ready");
}
