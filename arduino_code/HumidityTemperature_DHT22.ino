/* 
 *  DHT22 sensor example with ESP32:
 *  
 *  1. Publish message every sensorDelayMS to "/sensor/temp" and "/sensor/humid" for temperature and humidity.
 *  2. Connect DHT22 data out to GPIO0.
 *  3. This example use Adafruit unified sensor library for DHT22 reading, hence you must install it first.
 *  
 */
#include "M1128.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DEBUG true
#define DEBUG_BAUD 9600

#define DEVELOPER_ROOT "1"
#define DEVELOPER_USER "dmI0OkvoFRLRzHu3J3tEWQbIXQwDeF9q"
#define DEVELOPER_PASS "dyUiAb1cjkS8FRrokTXxtY1s4DUmOJsa"

#define WIFI_DEFAULT_SSID "SmartDHT22"
#define WIFI_DEFAULT_PASS "abcd1234"

#define DEVICE_PIN_RESET 3
#define SEND_INTERVAL     60000     // send data to mqtt broker interval
#define DHTPIN            0         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22     // DHT 22 (AM2302)

HardwareSerial *SerialDEBUG = &Serial;
M1128 iot;

DHT_Unified dht(DHTPIN, DHTTYPE);
unsigned int sensorDelayMS = 0;
unsigned int sensorCurMillis = 0;
unsigned int sensorPrevMillis = 0;
char resultT[7]; // Buffer big enough for 6-character float
char resultH[7]; // Buffer big enough for 6-character float  

void setup() {
  if (DEBUG) {
    //SerialDEBUG->begin(DEBUG_BAUD, SERIAL_8N1, SERIAL_TX_ONLY); // for ESP8266
    SerialDEBUG->begin(DEBUG_BAUD, SERIAL_8N1); // for ESP32
    while (!SerialDEBUG);
    SerialDEBUG->println(F("Initializing.."));
  }
  pinMode(DHTPIN,INPUT_PULLUP);
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
  
  initSensors();
  iot.init(DEBUG?SerialDEBUG:NULL);
  delay(10);
}

void loop() {
  iot.loop();
  if (iot.isReady) {
    measureSensors();
    sendData();
  }
}

void initSensors() {
  // Print temperature sensor details.
  dht.begin();
  sensorPrevMillis = 0;
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  SerialDEBUG->println(F("------------------------------------"));
  SerialDEBUG->println(F("Temperature"));
  SerialDEBUG->print  (F("Sensor:       ")); SerialDEBUG->println(sensor.name);
  SerialDEBUG->print  (F("Driver Ver:   ")); SerialDEBUG->println(sensor.version);
  SerialDEBUG->print  (F("Unique ID:    ")); SerialDEBUG->println(sensor.sensor_id);
  SerialDEBUG->print  (F("Max Value:    ")); SerialDEBUG->print(sensor.max_value); SerialDEBUG->println(F("°C"));
  SerialDEBUG->print  (F("Min Value:    ")); SerialDEBUG->print(sensor.min_value); SerialDEBUG->println(F("°C"));
  SerialDEBUG->print  (F("Resolution:   ")); SerialDEBUG->print(sensor.resolution); SerialDEBUG->println(F("°C"));  
  SerialDEBUG->println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  SerialDEBUG->println(F("------------------------------------"));
  SerialDEBUG->println(F("Humidity"));
  SerialDEBUG->print  (F("Sensor:       ")); SerialDEBUG->println(sensor.name);
  SerialDEBUG->print  (F("Driver Ver:   ")); SerialDEBUG->println(sensor.version);
  SerialDEBUG->print  (F("Unique ID:    ")); SerialDEBUG->println(sensor.sensor_id);
  SerialDEBUG->print  (F("Max Value:    ")); SerialDEBUG->print(sensor.max_value); SerialDEBUG->println(F("%"));
  SerialDEBUG->print  (F("Min Value:    ")); SerialDEBUG->print(sensor.min_value); SerialDEBUG->println(F("%"));
  SerialDEBUG->print  (F("Resolution:   ")); SerialDEBUG->print(sensor.resolution); SerialDEBUG->println(F("%"));  
  SerialDEBUG->println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  sensorDelayMS = sensor.min_delay / 1000;
  //sensorDelayMS = 60000;
}

void measureSensors() {
  delay(sensorDelayMS);
  sensors_event_t event;
  // Get temperature event and print its value.
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    SerialDEBUG->println(F("Error reading temperature!"));
  }
  else {
    dtostrf(event.temperature, 4, 2, resultT);
    SerialDEBUG->print(F("Temperature: "));
    SerialDEBUG->print(resultT);
    SerialDEBUG->println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    SerialDEBUG->println(F("Error reading humidity!"));
  }
  else {
    dtostrf(event.relative_humidity, 4, 2, resultH);
    SerialDEBUG->print(F("Humidity: "));
    SerialDEBUG->print(resultH);
    SerialDEBUG->println(F("%"));
  }
}

void sendData() {
  sensorCurMillis = millis();
  int32_t tframe = sensorCurMillis - sensorPrevMillis;
  if (tframe > SEND_INTERVAL || tframe == 0 || sensorPrevMillis==0) {
    sensorPrevMillis = sensorCurMillis;
    if (iot.mqtt->connected()) {
      iot.mqtt->publish(iot.constructTopic("sensor/temp"), resultT, true);
      iot.mqtt->publish(iot.constructTopic("sensor/humid"), resultH, true);
    }
  }
}

void callbackOnReceive(char* topic, byte* payload, unsigned int length) {
  String strPayload;
  strPayload.reserve(length);
  for (uint32_t i = 0; i < length; i++) strPayload += (char)payload[i];

  if (DEBUG) {
    SerialDEBUG->print(F("Receiving topic: "));
    SerialDEBUG->println(topic);
    SerialDEBUG->print(F("With value: "));
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
  //ESP.deepSleep(300000000); // sleep for 5 minutes
}

void publishState(const char* state) {
  if (iot.mqtt->connected()) iot.mqtt->publish(iot.constructTopic("$state"), state, true);  
}

void initPublish() {
  if (iot.mqtt->connected()) {    
    iot.mqtt->publish(iot.constructTopic("$state"), "init", false);
    iot.mqtt->publish(iot.constructTopic("$sammy"), "1.0.0", false);
    iot.mqtt->publish(iot.constructTopic("$name"), "Smart DHT22", false);
    iot.mqtt->publish(iot.constructTopic("$model"), "SAM-DHT22", false);
    iot.mqtt->publish(iot.constructTopic("$mac"), WiFi.macAddress().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$localip"), WiFi.localIP().toString().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$fw/name"), "DHT22", false);
    iot.mqtt->publish(iot.constructTopic("$fw/version"), "1.00", false);    
    iot.mqtt->publish(iot.constructTopic("$reset"), "true", false);
    iot.mqtt->publish(iot.constructTopic("$restart"), "true", false);
    iot.mqtt->publish(iot.constructTopic("$nodes"), "sensor", false);
  
  //define node "sensor"
    iot.mqtt->publish(iot.constructTopic("sensor/$name"), "Sensor", false);
    iot.mqtt->publish(iot.constructTopic("sensor/$type"), "Sensor-01", false);
    iot.mqtt->publish(iot.constructTopic("sensor/$properties"), "temp,humid", false);

    iot.mqtt->publish(iot.constructTopic("sensor/temp/$name"), "Temperature", false);
    iot.mqtt->publish(iot.constructTopic("sensor/temp/$settable"), "false", false);
    iot.mqtt->publish(iot.constructTopic("sensor/temp/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor/temp/$datatype"), "float", false);  
    iot.mqtt->publish(iot.constructTopic("sensor/temp/$unit"), "°C", false);  
    iot.mqtt->publish(iot.constructTopic("sensor/temp/$format"), "-40:125", false);  

    iot.mqtt->publish(iot.constructTopic("sensor/humid/$name"), "Humidity", false);
    iot.mqtt->publish(iot.constructTopic("sensor/humid/$settable"), "false", false);
    iot.mqtt->publish(iot.constructTopic("sensor/humid/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor/humid/$datatype"), "float", false);    
    iot.mqtt->publish(iot.constructTopic("sensor/humid/$unit"), "%", false);  
    iot.mqtt->publish(iot.constructTopic("sensor/humid/$format"), "0:100", false);  
  }
}

void initSubscribe() {
  if (iot.mqtt->connected()) { 
    iot.mqtt->subscribe(iot.constructTopic("reset"),1);  
    iot.mqtt->subscribe(iot.constructTopic("restart"),1);  
  }
  publishState("ready");
}
