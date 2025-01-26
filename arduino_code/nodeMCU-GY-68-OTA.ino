
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <t6iot.h>
#include <SFE_BMP180.h>

// t6 Server
char* t6HttpHost = "api.internetcollaboratif.info";                             // t6 server IP Address
int t6HttpPort = 3000;                                         //
int t6Timeout = 1000;                                          //
#define SLEEP_DELAY_IN_SECONDS  1800                           // 1800 => 30 minutes
#define SLEEP_TIMEOUT  60                                      // 60 => 1 minutes

// t6 JWT Authentication
const char* t6Username = "";  // 
const char* t6Password = "";                           //
const char* t6Key = "";                                        // Your t6 Key
const char* t6Secret = "";                                     // Your t6 Secret

// t6 Object
const char* secret = "";                                       // The current object secret for signature
char* t6ObjectId = "";     //nodeMCU.18
char* objectId = t6ObjectId;
char* t6UserAgent = "nodeMCU";                              //
char* currentVersion = "9";                                    // the OTA source version

// t6 Flow for Pressure
char* t6FlowId = "";       //
char* t6Mqtt_topic = "";                                       //
char* t6Unit = "mBar";                                         //
char* t6Save = "true";                                         //
char* t6Publish = "true";                                      //
#define ALTITUDE 30.0                                          // Altitude in meters, used for Pressure calculation

const char* ssid = "";                                 //
const char* password = ""; //

const char* ssidAP = t6ObjectId;                               //
const char* passwordAP = "";                               //

const char *OTAName = t6ObjectId;                              // Name for the OTA service
const char *OTAPassword = "";                                  // Password for the OTA service

unsigned long previousMillis = 0;
const long interval = 1000;  // interval at which to blink (milliseconds)
const long otaTimeout = 180000;  // interval at which to blink (milliseconds) - 3 minutes
int ledState = LOW;  // ledState used to set the LED
const char* mdnsName = "esp8266"; // Domain name for the mDNS responder
bool OTA_IN_Progress = false;
unsigned long startMillis = millis();
unsigned long currentMillis = millis();
struct sAverage { int32_t blockSum; uint16_t numSamples; };
struct sAverage sampleAve;
int16_t sensorPValue = 0;
String responseA; // for authentication
String responseD; // for datapoints
String responseDeploy; // for OTA Deploy
String responseO; // for ota
const char* objectExpectedVersion;
const char* sourceId;
const char* sourceLatestVersion;
const char* buildVersions;

t6iot t6Client;
SFE_BMP180 pressure;
ESP8266WiFiMulti wifiMulti;       // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80);       // Create a webserver object that listens for HTTP request on port 80

/* 
  (WARNING: do not connect + to 5V or the sensor will be damaged!)
  
  Hardware connections:
  NodeMCU     BMP180
  3.3V         VIN
  GND          GND
  D1           SCL          
  D2           SDA            
*/


void setup() {
  Serial.println("Booting");
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(100);
  //startWiFi();
  startWiFi_STA();
  startOTA();
  //startMDNS();
  startServer();
  startSensor();
  startt6IOT();
  startMillis = millis();
}

void loop() {
  currentMillis = millis();
  ArduinoOTA.handle();
  server.handleClient();
  readSample();
  if (sensorPValue) {
    const int BUFFER_SIZE = JSON_OBJECT_SIZE(6);
    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
    JsonObject& payload = jsonBuffer.createObject();
    payload["value"] = sensorPValue;
    payload["flow_id"] = t6FlowId;
    payload["mqtt_topic"] = t6Mqtt_topic;
    payload["unit"] = t6Unit;
    payload["save"] = t6Save;
    payload["publish"] = t6Publish;
    payload.prettyPrintTo(Serial);
    t6Client.createDatapoint(t6FlowId, payload, false, &responseD);
    handleDatapointResponse();
  }
  if(!OTA_IN_Progress && currentMillis - startMillis > SLEEP_TIMEOUT * 1000) {
    pleaseGoToBed();
  } else {
    Serial.println("t6 Auth");
    t6Authenticate();
    Serial.println("Calling Api to get latest version");
    t6Client.getOtaLatestVersion(t6ObjectId, &responseO);
    handleOTALatestVersionResponse();
    if ( sourceId ) {
      if ( String(objectExpectedVersion) != String(currentVersion) ) {
        //if ( newVersionStatus === "200 Ready to deploy" ) { // Commented as we consider the Build is already OK on t6 server
          unsigned long currentMillis = millis();
          while (currentMillis - previousMillis >= interval && currentMillis < otaTimeout) {
            Serial.println(currentMillis);
            previousMillis = currentMillis;
            ledState = not(ledState);
            digitalWrite(LED_BUILTIN, ledState);
            currentMillis = millis();
          }
          // call OTA Deploy
          t6Client.otaDeploy(sourceId, t6ObjectId, &responseDeploy);
          handleOTADeployResponse();
        //}
      } else {
        Serial.println("No OTA needed, going to sleep now.");
      }
    } else {
      Serial.println("No sourceId detected!");
    }
  }
  delay(1000);
}

void readSample() {
  char status;
  double T,P,p0,a;
  Serial.println();
  Serial.print("provided altitude: ");
  Serial.print(ALTITUDE,0);
  Serial.print(" meters, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println(" feet");
  
  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0) {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0) {
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" deg F");
      
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0) {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0) {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");
          
          sensorPValue = P;

          // The pressure sensor returns absolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sea level function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          a = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(a,0);
          Serial.print(" meters, ");
          Serial.print(a*3.28084,0);
          Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");

  Serial.println("------------------------------");
}
int16_t getAverage(struct sAverage *ave) {
  int16_t average = ave->blockSum / ave->numSamples;
  // get ready for the next block
  ave->blockSum = 0;
  ave->numSamples = 0;
  return average;
}
void startt6IOT() {
  t6Client.begin(t6HttpHost, t6HttpPort, t6UserAgent, t6Timeout);
}
void t6Authenticate() {
  t6Client.authenticate(t6Username, t6Password, &responseA);
  handleAuthenticateResponse();
}
void startWiFi_STA() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("WIFI ready");
}
void startWiFi() {
  WiFi.softAP(ssidAP, passwordAP);
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started\r\n");

  wifiMulti.addAP(ssid, password);   // add Wi-Fi networks you want to connect to
  //wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  //wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  Serial.println("Connecting");
  while (wifiMulti.run() != WL_CONNECTED && WiFi.softAPgetStationNum() < 1) {  // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print('.');
  }
  Serial.println("\r\n");
  if(WiFi.softAPgetStationNum() == 0) {      // If the ESP is connected to an AP
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());             // Tell us what network we're connected to
    Serial.print("IP address:\t");
    Serial.print(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer
  } else {                                   // If a station is connected to the ESP SoftAP
    Serial.print("Station connected to ESP8266 AP");
  }
  Serial.println("WIFI ready");
}
void startOTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(OTAName);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 
  // ArduinoOTA.setPasswordHash("");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    OTA_IN_Progress = true;
    digitalWrite(LED_BUILTIN, LOW);
  });
  ArduinoOTA.onEnd([]() {
    digitalWrite(LED_BUILTIN, HIGH);
    OTA_IN_Progress = false;
    Serial.println("\r\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready");
  Serial.print("listening to: ");
  Serial.println(WiFi.localIP());
}
void startMDNS() {
  MDNS.begin(mdnsName);
  Serial.print("mDNS responder started: http://");
  Serial.print(mdnsName);
  Serial.println(".local");
  Serial.println("MDNS ready");
}
void startSensor() {
  if (pressure.begin()) {
    Serial.println("BMP180 ready");
  } else {
    Serial.println("BMP180 not ready");
    //while(1); // Pause forever.
  }
}
void startServer() {
  server.on("/favicon.ico", []() {
    static const uint8_t gif[] PROGMEM = {
      0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x10, 0x00, 0x10, 0x00, 0x80, 0x01,
      0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x00, 0x10, 0x00, 0x00, 0x02, 0x19, 0x8c, 0x8f, 0xa9, 0xcb, 0x9d,
      0x00, 0x5f, 0x74, 0xb4, 0x56, 0xb0, 0xb0, 0xd2, 0xf2, 0x35, 0x1e, 0x4c,
      0x0c, 0x24, 0x5a, 0xe6, 0x89, 0xa6, 0x4d, 0x01, 0x00, 0x3b
    };
    char gif_colored[sizeof(gif)];
    memcpy_P(gif_colored, gif, sizeof(gif));
    // Set the background to a random set of colors
    gif_colored[16] = millis() % 256;
    gif_colored[17] = millis() % 256;
    gif_colored[18] = millis() % 256;
    server.send(200, "image/gif", gif_colored, sizeof(gif_colored));
  });
  server.on("/", []() {
    Serial.println("HTTP /");
    server.send(200, "text/html", "<a href='./on'>ON</a> - <a href='./off'>OFF</a>");
  });
  server.on("/on", []() {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("HTTP LED ON");
    server.send(201, "text/html", "<a href='./on'>ON</a> - <a href='./off'>OFF</a>");
  });
  server.on("/off", []() {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("HTTP LED OFF");
    server.send(201, "text/html", "<a href='./on'>ON</a> - <a href='./off'>OFF</a>");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP ready");
}
void handleNotFound() {
  Serial.println("HTTP 404");
  server.send(404, "text/plain", "404: File Not Found");
}
void handleAuthenticateResponse() {
  const int A_BUFFER_SIZE = JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer A_jsonRequestBuffer(A_BUFFER_SIZE);
  JsonObject& authenticate = A_jsonRequestBuffer.parseObject(responseA);
  if (!authenticate.success()) {
    Serial.println("Failure on parsing json.");
    Serial.println(responseA);
  } else {
    const char* Aerror = authenticate["error"];
    const char* Atoken = authenticate["token"];
    const char* Astatus = authenticate["status"];
    const char* Arefresh_token = authenticate["refresh_token"];
    const char* ArefreshTokenExp = authenticate["refreshTokenExp"];
    if ( Aerror ) {
      Serial.println("Failure on:");
      Serial.println(responseA);
    }
    Serial.println();
    Serial.print("\tToken: ");
    Serial.println( Atoken );
    Serial.print("\tStatus: ");
    Serial.println( Astatus );
    Serial.print("\tRefresh Token: ");
    Serial.println( Arefresh_token );
    Serial.print("\tRefresh Token Exp: ");
    Serial.println( ArefreshTokenExp );
    Serial.println();
  }
}
void handleDatapointResponse() {
  const int D_BUFFER_SIZE = JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer D_jsonRequestBuffer(D_BUFFER_SIZE);
  JsonObject& datapoint = D_jsonRequestBuffer.parseObject(responseD);
  if (!datapoint.success()) {
    Serial.println("Failure on parsing json.");
    Serial.println(responseD);
  } else {
    const char* Derror = datapoint["error"];
    if ( Derror ) {
      Serial.println("Failure on:");
      Serial.println(responseD);
    }
    Serial.println();
    Serial.println();
  }
}
void handleOTALatestVersionResponse() {
  const int O_BUFFER_SIZE = JSON_OBJECT_SIZE(64);
  DynamicJsonBuffer O_jsonRequestBuffer(O_BUFFER_SIZE);
  JsonObject& ota = O_jsonRequestBuffer.parseObject(responseO);
  if (!ota.success()) {
    Serial.println("Failure on parsing json.");
    Serial.println(responseO);
  } else {
    objectExpectedVersion = ota["objectExpectedVersion"];
    sourceLatestVersion = ota["sourceLatestVersion"];
    buildVersions = ota["buildVersions"];
    sourceId = ota["source_id"];
    Serial.println();
    Serial.print("\tcurrentVersion: ");
    Serial.println( currentVersion );
    Serial.print("\tobjectExpectedVersion: ");
    Serial.println( objectExpectedVersion );
    Serial.print("\tsourceLatestVersion: ");
    Serial.println( sourceLatestVersion );
    Serial.print("\tsourceId: ");
    Serial.println( sourceId );
    Serial.println();
  }
}
void handleOTADeployResponse() {
  const int O2_BUFFER_SIZE = JSON_OBJECT_SIZE(32);
  DynamicJsonBuffer O2_jsonRequestBuffer(O2_BUFFER_SIZE);
  JsonObject& otadeploy = O2_jsonRequestBuffer.parseObject(responseDeploy);
  if (!otadeploy.success()) {
    Serial.println("Failure on parsing json.");
    Serial.println(responseDeploy);
  } else {
    const char* data = otadeploy["data"];
    Serial.println();
    Serial.print("\tdata: ");
    Serial.println( data );
    Serial.println();
  }
}
void pleaseGoToBed() {
    Serial.println();
    Serial.println();
    Serial.println("Sleeping in few seconds...");
    delay(500);
    ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);
}
