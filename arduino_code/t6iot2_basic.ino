/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/

   Linux tools:
   - SSDP     : gssdp-discover -i eth0 -r 5
   - MDSN     : mdns-scan
   - Firewall : sudo tail -f /var/log/kern.log

   See Readme for compilation informations: https://github.com/mathcoll/t6iot/blob/master/README.md
*/

#include <t6iot.h>
#include "t6config.h"

int OTAcurrentVersion = 1;                     // The source version of the current file that can be updated OTA

t6iot t6client;                                // Create a new t6iot client
struct sAverage {
  int32_t blockSum;
  uint16_t numSamples;
};
struct sAverage sampleAve;

void setup() {
  Serial.begin(115200);
  Serial.flush();
  pinMode(PWR_PROBE, OUTPUT);
  delay(200);
  Serial.println("t6 > BOOT Object");
  t6client.debug(1);                          // Display additional debug information
  //t6client.lockSleep(t6Timeout);            // Prevent chip from deepsleep
  //t6client.unlockSleep();                   // Allow chip to deepsleep
  t6client.set_wifi(WIFI_SSID, WIFI_PASSWORD);// Connect to Wifi network

  if (T6_USE_CUSTOM_SERVER) {
    t6client.set_server(host, portHttp);      // Set custom server host, port
  } else {
    t6client.set_server();                    // Use host & port from default t6iot library
  }
  if (!useragent.isEmpty()) {
    t6client.set_useragent(useragent);        // Customize User-Agent
  }
  t6client.setSleepDuration(SLEEP_DURATION_SEC);// Set sleep duration
  t6client.set_key(api_key);                  // Required to identify yourself on t6
  t6client.set_secret(api_secret);            // Required to identify yourself on t6
  t6client.set_object_id(object_id);          // Required for websockets & encryption and used in the user-agent
  t6client.set_object_secret(object_secret);  // Required for websockets & encryption

  if (T6_FEAT_HTTP) {
    t6client.startHttp(80);                   // Load to serve Http files with a user interface
    t6client.addStaticRoutes();               // Load Http static routes on the ESP
    t6client.addDynamicRoutes();              // Load Http dynamic routes on the ESP
  }
  if (T6_FEAT_AUDIO) {
    t6client.startAudio();                    // Activate and start Audio (I2S audio)
    t6client.audioSetVol(5);                  // Set volume in a 1-10 range (I2S audio)
  }
  if (T6_FEAT_WEBSOCKETS) {
    if(T6_USE_CUSTOM_SERVER) {
      t6client.startWebsockets(host, portWs);
    } else {
      t6client.startWebsockets();             // Connect to t6 web-socket server
    }
  }
  if (T6_FEAT_OTA) {
    t6client.activateOTA(object_name);        // Activating Over The Air (OTA) update procedure
  }
  if (T6_FEAT_MDNS) {
    t6client.startMdns(object_name);          // Start MDNS
  }
  if (T6_FEAT_SSDP) {
    t6client.startSsdp(object_name);          // Start SSDP, suggested to start SSDP at last
  }

                                              // Run those tasks only once, and now
  t6client.scheduleOnce(0, checkOta,   TIME_SECONDS);
  t6client.scheduleOnce(0, readSample, TIME_SECONDS);

                                              // Schedule tasks
  if (T6_FEAT_OTA) {
    otaTask  = t6client.scheduleFixedRate(CHECKOTAINTERVAL, checkOta,   TIME_SECONDS);
  }
  readTask = t6client.scheduleFixedRate(READINTERVAL,     readSample, TIME_SECONDS);
  //t6client.cancelTask(readTask);            // Stop a task from executing again if it is a repeating task
}
void loop() {
  if(t6client._websockets_started && T6_FEAT_WEBSOCKETS) { t6client.webSockets_loop(); }
  if(t6client._audio_started && T6_FEAT_AUDIO)           { t6client.audio_loop(); }
  if(t6client._mdns_started && T6_FEAT_MDNS)             { t6client.mdns_loop(); }
  if(t6client._OTA_started && T6_FEAT_OTA)               { t6client.ota_loop(); }
  t6client.runLoop();                         // t6 TaskManager
  delay(5);
  //t6client.goToSleep(SLEEP_DURATION_SEC);
  //delay(SLEEP_DURATION_SEC);  // Use delay instead of deepSleep when HttpServer is enabled
}
int16_t addSampleToAverage(struct sAverage *ave, int16_t newSample) {
  ave->blockSum += newSample;
  ave->numSamples++;
  return newSample;
}
int16_t getAverage(struct sAverage *ave) {
  int16_t average = ave->blockSum / ave->numSamples;
  // get ready for the next block
  ave->blockSum = 0;
  ave->numSamples = 0;
  return average;
}
void goToSleep() {
  t6client.goToSleep();
}
void checkOta() {
  t6client.getOtaLatestVersion(object_id, OTAcurrentVersion);
}
void readSample() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(PWR_PROBE, HIGH); // Turn "On"
  t6client.lockSleep();
  delay(500);
  int count=1;
  Serial.println("t6 > Measuring:");
  do {
    sensorValue = constrain(analogRead(PIN_PROBE), 0.0, 10000.0);
    Serial.print("     * Measurement ");
    Serial.print(count);
    Serial.print("/");
    Serial.print(measurements);
    Serial.print(": ");
    Serial.println(sensorValue);
    
    if ( sensorValue != 0.0 && sensorValue != 10000.0 ) {
      addSampleToAverage(&sampleAve, sensorValue);
    } else {
      Serial.print("Weird sensorValue! Not added to average.");
      Serial.println(sensorValue);
    }
    count++;
    delay(500);
  } while (count <= measurements); // 10 valid measures expected
  delay(1000);
  Serial.println("------------------------------");
  digitalWrite(PWR_PROBE, LOW); // Turn "Off"
  digitalWrite(LED_BUILTIN, HIGH);

  if (getAverage(&sampleAve) > -1) {
    // Create a payload.
    DynamicJsonDocument payload(1024);  // 
    DynamicJsonDocument meta(64);       // 
    DynamicJsonDocument rules(1024);    // Rules require several nodes
    int index = 0;
    payload[index]["value"]        = sensorValue; // getAverage(&sampleAve);
    payload[index]["flow_id"]      = flow_id;
    payload[index]["mqtt_topic"]   = "t6/sensor/temperature";
    payload[index]["retention"]    = "retention1d";
    payload[index]["datatype_id"]  = "e7dbdc23-5fa8-4083-b3ec-bb99c08a2a35";
    payload[index]["object_id"]    = object_id;
    payload[index]["longitude"]    = -117.98329726717841;  // This can be dynamic and sent to t6 iot server
    payload[index]["latitude"]     = 35.06649625390798;    // This can be dynamic and sent to t6 iot server
    payload[index]["save"]         = false;
    payload[index]["publish"]      = true;

    meta[index]["sensor"]          = "sample sensor";
    /*
    rules[index][0]["conditions"]["all"][0]["fact"]        = "flow";
    rules[index][0]["conditions"]["all"][0]["operator"]    = "equal";
    rules[index][0]["conditions"]["all"][0]["value"]       = flow_id;
    rules[index][0]["conditions"]["all"][1]["fact"]        = "value";
    rules[index][0]["conditions"]["all"][1]["operator"]    = "distanceGreaterThan";
    rules[index][0]["conditions"]["all"][1]["value"]       = "1";
    rules[index][0]["event"]["type"]                       = "replaceWithDistance";
    rules[index][0]["priority"]                            = "1";
    rules[index][1]["conditions"]["all"][0]["fact"]        = "flow";
    rules[index][1]["conditions"]["all"][0]["operator"]    = "equal";
    rules[index][1]["conditions"]["all"][0]["value"]       = flow_id;
    rules[index][1]["conditions"]["all"][1]["fact"]        = "value";
    rules[index][1]["conditions"]["all"][1]["operator"]    = "distanceGreaterThan";
    rules[index][1]["conditions"]["all"][1]["value"]       = "50";
    rules[index][1]["event"]["type"]                       = "email";
    rules[index][1]["event"]["params"]["from"]             = "";
    rules[index][1]["event"]["params"]["bcc"]              = "";
    rules[index][1]["event"]["params"]["to"]               = "";
    rules[index][1]["event"]["params"]["subject"]          = "Geofencing > outside";
    rules[index][1]["event"]["params"]["text"]             = "Geofencing > outside by {distance} meter(s)";
    rules[index][1]["event"]["params"]["html"]             = "<h1>Geofencing</h1><br /> &gt; outside by {distance} meter(s)";
    rules[index][1]["priority"]                            = "2";
    */
    payload[index]["meta"]          = meta[index];
    payload[index]["rules"]         = rules[index];
    // payload[index]["unit"]          = "26a4be78-1c02-4a41-acc1-14d8e6b29a84";
    // payload[index]["timestamp"]  = millis(); // By default, timestamp will be handled by server
  
    /*
    index++;
    payload[index]["value"]        = sensorValue; // getAverage(&sampleAve);
    payload[index]["flow_id"]      = flow_id2;
    payload[index]["mqtt_topic"]   = "t6/sensor/temperature2";
    payload[index]["retention"]    = "retention1d";
    payload[index]["datatype_id"]  = "e7dbdc23-5fa8-4083-b3ec-bb99c08a2a35";
    payload[index]["object_id"]    = object_id;
    payload[index]["longitude"]    = -117.98329726717841;
    payload[index]["latitude"]     = 35.06649625390798;
    payload[index]["save"]         = true;
    payload[index]["publish"]      = true;

    meta[index]["sensor"]          = "sample sensor2";
    payload[index]["meta"]         = meta[index];
    // payload[index]["unit"]         = "26a4be78-1c02-4a41-acc1-14d8e6b29a84";
    // payload[index]["timestamp"]  = millis(); // By default, timestamp will be handled by server
    */

    // TODO signature
  
    // Send the payload to the t6iot API.
    int status = t6client.createDatapoint(payload);
    Serial.println("t6 > Result status " + String(status));
    t6client.unlockSleep();
    if (!T6_FEAT_HTTP && !T6_FEAT_MDNS && !T6_FEAT_SSDP && !T6_FEAT_WEBSOCKETS && !T6_FEAT_OTA) {
      sleepTask = t6client.scheduleFixedRate(0, goToSleep, TIME_SECONDS);
    }
  }
}
