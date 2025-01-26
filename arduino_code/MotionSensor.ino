/* 
 *  Motion sensor example with ESP8266-01:
 *  
 *  1. Publish message "/sensor/motion" with value "true" whenever sensor triggered.
 *  2. When sensor is being triggered, it must send low signal to pin restart ESP (Board pin 6)
 *  3. ESP will immidiately publish all neccessary messages on bootup and followed publish "/sensor/motion"
 *  4. ESP will then goes to deep sleep to keep the power low.
 *  
 */

#include "M1128.h"

#define DEBUG true
#define DEBUG_BAUD 9600

#define DEVELOPER_ROOT "1"
#define DEVELOPER_USER "dmI0OkvoFRLRzHu3J3tEWQbIXQwDeF9q"
#define DEVELOPER_PASS "dyUiAb1cjkS8FRrokTXxtY1s4DUmOJsa"

#define DEVICE_PIN_RESET 3

#define WIFI_DEFAULT_SSID "SmartMotion"
#define WIFI_DEFAULT_PASS "abcd1234"

HardwareSerial *SerialDEBUG = &Serial;
M1128 iot;

void setup() {
  if (DEBUG) {
    //SerialDEBUG->begin(DEBUG_BAUD, SERIAL_8N1, SERIAL_TX_ONLY); // for ESP8266
    SerialDEBUG->begin(DEBUG_BAUD, SERIAL_8N1); // for ESP32
    while (!SerialDEBUG);
    SerialDEBUG->println("Initializing..");
  }
  pinMode(DEVICE_PIN_RESET, FUNCTION_3);
  iot.pinReset = DEVICE_PIN_RESET;
  iot.prod = true;
  iot.cleanSession = true;
  iot.setWill = false; //don't use lwt
  iot.apConfigTimeout = 300000;
  iot.wifiConnectTimeout = 120000;
  iot.devConfig(DEVELOPER_ROOT,DEVELOPER_USER,DEVELOPER_PASS);
  iot.wifiConfig(WIFI_DEFAULT_SSID,WIFI_DEFAULT_PASS);
  
  iot.onConnect = callbackOnConnect;
  iot.onAPConfigTimeout = callbackOnAPConfigTimeout;
  iot.onWiFiConnectTimeout = callbackOnWiFiConnectTimeout;  
  iot.init(DEBUG?SerialDEBUG:NULL);
  delay(10);
}

void loop() {
  yield();
  if (!iot.isReady) iot.loop();
}

void callbackOnConnect() {
  iot.mqtt->publish(iot.constructTopic("sensor/motion"), "true", false); 
  initPublish();    
  iot.mqtt->publish(iot.constructTopic("$state"), "sleeping", true);
  delay(3000);
  ESP.deepSleep(0);
}

void callbackOnAPConfigTimeout() {
  ESP.deepSleep(0); // going to deep sleep forever
}

void callbackOnWiFiConnectTimeout() {
  ESP.deepSleep(0); // going to deep sleep forever
}

void initPublish() { 
  if (iot.mqtt->connected()) {    
    iot.mqtt->publish(iot.constructTopic("$state"), "init", false);
    iot.mqtt->publish(iot.constructTopic("$sammy"), "1.0.0", false);
    iot.mqtt->publish(iot.constructTopic("$name"), "Motion Sensor", false);
    iot.mqtt->publish(iot.constructTopic("$model"), "SAM-MS01", false);
    iot.mqtt->publish(iot.constructTopic("$mac"), WiFi.macAddress().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$localip"), WiFi.localIP().toString().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$fw/name"), "MS01", false);
    iot.mqtt->publish(iot.constructTopic("$fw/version"), "1.00", false);    
    iot.mqtt->publish(iot.constructTopic("$nodes"), "sensor", false);
  
  //define node "bell"
    iot.mqtt->publish(iot.constructTopic("sensor/$name"), "Sensor", false);
    iot.mqtt->publish(iot.constructTopic("sensor/$type"), "Sensor-01", false);
    iot.mqtt->publish(iot.constructTopic("sensor/$properties"), "motion", false);

    iot.mqtt->publish(iot.constructTopic("sensor/motion/$name"), "Motion Sensor", false);
    iot.mqtt->publish(iot.constructTopic("sensor/motion/$settable"), "false", false);
    iot.mqtt->publish(iot.constructTopic("sensor/motion/$retained"), "false", false);
    iot.mqtt->publish(iot.constructTopic("sensor/motion/$datatype"), "boolean", false);  

  // set device to ready
    iot.mqtt->publish(iot.constructTopic("$state"), "ready", false);  
  }
}
