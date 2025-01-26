/*


    1M - FS 64kb / OTA 470kb
    1M - FS 256kb / OTA 374kb
    2M - FS 512kb / OTA 768kb
   openssl
   req -x509 -newkey rsa:1024 -sha256 -keyout key.txt -out cert.txt -days 365 -nodes -subj "/C=FR/ST=CA/L=Paris/O=ESP [FR]/OU=ESP/CN=esp8266.local" -addext subjectAltName=DNS:esp8266.local,IP:192.168.0.28


*/
#include <ESP8266WiFi.h>
#include <t6iot.h>

// t6 Server
char* t6HttpHost = "api.internetcollaboratif.info";               // t6 server IP Address, actually, it can't be updated only from this parameter, require SSL fingerprint in the library too
int t6HttpPort = 443;                                             // t6 port
int t6Timeout = 3000;                                             // t6 timeout to get an answer from server

// t6 JWT Authentication
const char* t6Username = "demo";                                  // Your t6 Username
const char* t6Password = "?[W{7kG'X63-e0N";                       // Your t6 Password
// or :
const char* t6Key = "";                                           // Your t6 Key
const char* t6Secret = "";                                        // Your t6 Secret

// t6 Object
char* secret = "";                                                // The object secret, for signature
char* t6ObjectId = "092579ba-3dd6-4c03-982e-0ecc66033609";        // The Object uuid-v4 in t6
char* t6UserAgent = "nodeMCU.28";                                 // The userAgent used when calling t6 api
const char* wwwUsername = "";                                     // Optional Username to call Object Api, set to "" to disable this authentication
const char* wwwPassword = "";                                     // Optional Password to call Object Api, set to "" to disable this authentication

// t6 Flow container for Sensor data
char* t6FlowId = "7774c70a-551a-4f1c-b78c-efa836835b14";          //
char* t6Mqtt_topic = "";                                          //
char* t6Unit = "%";                                               //
char* t6Save = "false";                                           //
char* t6Publish = "false";                                        //

const char* ssid PROGMEM = "";                                            // Your own Wifi ssid to connect to
const char* password PROGMEM = "";                                        // Your wifi password

const long READInterval PROGMEM = 3 * 60;                                 // Interval between each READ -> 3 minutes
const long JWTRefreshInterval PROGMEM = 4 * 60;                           // Should be less than server -> 4 minutes are fair
const long POSTInterval PROGMEM = 30 * 60;                                // Interval between each POST -> 30 minutes
const long POLLInterval PROGMEM = 100;                                    // Interval for polling ; it should be short -> 100ms
float sensorValue = -1.0;                                         // Value read from the sensor

uint8_t jwtrTask, pollTask, readTask, postTask;                   // All tasks that can be cancelled

String html = "<html>\n"
              "<head></head>\n"
              "<body>\n"
              "<h1>ESP demo FROM ino file</h1>\n"
              "<ul>\n"
              "<li><a href='/'>Home</a></li>\n"
              "<li><a href='/open'>open</a></li>\n"
              "<li><a href='/close'>close</a></li>\n"
              "<li><a href='/getVal'>getVal</a></li>\n"
              "<!--<li><a href='/setVal'>setVal</a></li>-->\n"
              "<li><a href='/on'>on</a></li>\n"
              "<li><a href='/off'>off</a></li>\n"
              "<li><a href='/upper'>upper</a></li>\n"
              "<li><a href='/lower'>lower</a></li>\n"
              "<li><a href='/true'>True</a></li>\n"
              "<li><a href='/false'>False</a></li>\n"
              "<li><a href='/refresh'>Refresh UI</a></li>\n"
              "<li><a href='/upgrade'>Upgrade OTA</a></li>\n"
              "</ul>\n"
              "</body>\n"
              "</html>\n";

static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDDDCCAnWgAwIBAgIUX1K4eXt2f611tVzU41xhF7Q5bhMwDQYJKoZIhvcNAQEL
BQAwgYcxCzAJBgNVBAYTAkZSMQswCQYDVQQIDAJDQTEOMAwGA1UEBwwFUGFyaXMx
IzAhBgNVBAoMGkludGVybmV0IENvbGxhYm9yYXRpZiBbRlJdMR4wHAYDVQQLDBVJ
bnRlcm5ldCBDb2xsYWJvcmF0aWYxFjAUBgNVBAMMDWVzcDgyNjYubG9jYWwwHhcN
MjAwNjIxMTM1NzI3WhcNMjEwNjIxMTM1NzI3WjCBhzELMAkGA1UEBhMCRlIxCzAJ
BgNVBAgMAkNBMQ4wDAYDVQQHDAVQYXJpczEjMCEGA1UECgwaSW50ZXJuZXQgQ29s
bGFib3JhdGlmIFtGUl0xHjAcBgNVBAsMFUludGVybmV0IENvbGxhYm9yYXRpZjEW
MBQGA1UEAwwNZXNwODI2Ni5sb2NhbDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkC
gYEAxrTYiPXKvnXIvXpPZNpjewXwocdSkIZZrl3UjinSj+BKNzUjDJ90qiS5D5hp
qc7tkIbC/XRm8XiiwGrbfalTYiG/QktyuJETUDYAUbsgAU9XCUx/IPu5tyIYacYd
vlyjK1ziswICFyt+qeyTSNFVFeiRaCD+BzfF6sJ8GnN7HkUCAwEAAaNzMHEwHQYD
VR0OBBYEFJTlkmBa3+TJDIDirw7XtyYgqvtaMB8GA1UdIwQYMBaAFJTlkmBa3+TJ
DIDirw7XtyYgqvtaMA8GA1UdEwEB/wQFMAMBAf8wHgYDVR0RBBcwFYINZXNwODI2
Ni5sb2NhbIcEwKgAHDANBgkqhkiG9w0BAQsFAAOBgQC2tmTOFGOmWW0msc0nJOaH
TbDicznr1LQ+0/HrliFkMd+zhVD+bv31Us+OUIvH5YLDd1YgGdXOYwev1jsFQ/b9
3li61MrdC8tGdg2IgR0MMwm0YFBo7UMHSINsbQQJNn2kvEYd1SQXW0tnLHCB6B/5
E0JSXTdcu5KoXcyi6Fs6SA==
-----END CERTIFICATE-----
)EOF";

static const char serverKey[] PROGMEM = R"EOF(
-----BEGIN PRIVATE KEY-----
MIICeAIBADANBgkqhkiG9w0BAQEFAASCAmIwggJeAgEAAoGBAMa02Ij1yr51yL16
T2TaY3sF8KHHUpCGWa5d1I4p0o/gSjc1IwyfdKokuQ+YaanO7ZCGwv10ZvF4osBq
232pU2Ihv0JLcriRE1A2AFG7IAFPVwlMfyD7ubciGGnGHb5coytc4rMCAhcrfqns
k0jRVRXokWgg/gc3xerCfBpzex5FAgMBAAECgYEAl2hbiljHaML097PH5VAm9ym6
hnsLpYZJtXxXpw+lGu5Lfq+likvCKJcc0A5RjQtYp4SX9WqJujsywC1sG9824bjQ
2KSI3zVTL1YGhjT2rJ/hit/6UyNJ0NlE4tBC4kcdMTiQJcXACRhU9tqpB2+XxAvd
dWR3mQcwfx7hgs5W7aECQQD555gy3S9YStOIbvjBFPuf5H8tCYjad98A1VCabYEI
W18fKP1qsI4XYPXb1+bCOGBzTYdgfwdCkq1o0w+jLdttAkEAy42Rwj8tUEqEKqti
N5zI8bdfrvKrRNsG/D7L+xYFzKcGdAxy3EkmPRItmvpQoYnFXgt5rgKX8BtdODEJ
3wVvOQJBANHIR2yTXpYoojBKr8kt+xi7MMM22MpEjFBfwX0RZZsFUPGQcENqmK0p
eNonAJCT0OuZ7oXjvM124dk4LE1YRnUCQBYgaXvIClD0CLHqt158OIut90S5NEzj
0jqRSPMeTbsVXOo4gDPZ78Iru7FhDGrC4yV58dwsg/+hudxl2EDaAmECQQDIdY0m
nq6N58GZS17fI5oizTTabZ4g6MtLZS4rzA5yXI6c8ih9cNOQbTGlXHm+JmM0TwBJ
ex3TqsepYTCaDVbE
-----END PRIVATE KEY-----
)EOF";

T6iot t6Client;                                                 // Init T6iot Client

void setup() {
  Serial.println("Booting ESP..");
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);                                 // Turn the LED on ...
  startWiFi(ssid, password);                            	  // Obviously, the wifi initialization :-)
  printIPAddressOfHost(t6HttpHost);                               // Some custom wifi logs :-)
  digitalWrite(LED_BUILTIN, HIGH);                                // ... and then, wifi is set, turn light off

  t6Client.DEBUG = false;                                         // Activate or disable DEBUG mode
  t6Client.LOGS = true;                                           // Activate or disable LOGS on Serial
  t6Client.timestamp = 1593370431;                                // 
  t6Client.init(t6HttpHost, t6HttpPort, t6UserAgent, t6Timeout);  // This will initialize the t6 Client according to server
  t6Client.initObject(t6ObjectId, secret);                        // Initialize t6 Object with its uuid-v4, it's secret if you need to sign payload

  //t6Client.getStatus();                                           // Get t6 Status
  
  t6Client.setCredentials(t6Username, t6Password);                // This will define your own personal username/password to connect to t6
  t6Client.authenticate();                                        // Generate a JWT from your personnal credential on t6 server

  //t6Client.setHtml(html);                                         // Set html into the Object
  t6Client.setHtml();                                             // Or Fetch it from t6 api

  //t6Client.activateOTA();                                         // Activating Over The Air (OTA) update procedure

  t6Client.setWebServerCredentials(                               // Define credentials for webserver on the Object
    wwwUsername,
    wwwPassword
  );
  t6Client.startWebServer(serverCert, serverKey);                 // Starting to listen from the Object on Http Api

  jwtrTask = t6Client.scheduleFixedRate(JWTRefreshInterval, refreshToken, TIME_SECONDS);   // Refresh JWT and must be after an authenticate
  pollTask = t6Client.scheduleFixedRate(POLLInterval, doServerQueries, TIME_MILLIS);       // Polling library, using milliseconds as TimerUnit
  readTask = t6Client.scheduleFixedRate(READInterval, readSample, TIME_SECONDS);           // Read sensor Value regularly
  postTask = t6Client.scheduleFixedRate(POSTInterval, postSample, TIME_SECONDS);           // POST Value regularly
  t6Client.scheduleOnce(0, setupComplete, TIME_SECONDS); // Run only once the setup Complete method
}

void loop() {
  t6Client.runLoop();                                             // t6 TaskManager
  t6Client.handleClient();                                        // Handling t6 Object http connexion, only when WebServer is activated
}

void refreshToken() {
  Serial.println("refreshToken() called");
  t6Client.refreshToken();
}

void setOn() {
  Serial.println("setOn() called");
  jwtrTask = t6Client.scheduleFixedRate(JWTRefreshInterval, refreshToken, TIME_SECONDS);   // Refresh JWT and must be after an authenticate
  pollTask = t6Client.scheduleFixedRate(POLLInterval, doServerQueries, TIME_MILLIS);       // Polling library, using milliseconds as TimerUnit
  readTask = t6Client.scheduleFixedRate(READInterval, readSample, TIME_SECONDS);           // Read sensor Value regularly
  postTask = t6Client.scheduleFixedRate(POSTInterval, postSample, TIME_SECONDS);           // POST Value regularly
}

void setOff() {
  Serial.println("setOff() called");
  t6Client.cancelTask(postTask);                                // Stop a task from executing again if it is a repeating task
  t6Client.cancelTask(jwtrTask);                                // Stop a task from executing again if it is a repeating task
}

void readSample() {
  Serial.println("readSample() called");
  sensorValue++;
  Serial.println(String("Updating sensorValue to: ")+sensorValue);
  t6Client.setValue(sensorValue);                               // Updating t6 with the sensor value
}

void postSample() {
  Serial.println(String("Posting sensorValue to t6: ")+sensorValue);
  readSample();
  if (sensorValue > -1) {
    t6Client.lockSleep(t6Timeout);                              // Lock the sleep, so the Object can't get into deep sleep mode when posting
    
    DynamicJsonDocument payload(1024);                          // Building payload to post
    payload[String("value")] = sensorValue;
    payload[String("flow_id")] = t6FlowId;
    payload[String("mqtt_topic")] = t6Mqtt_topic;
    payload[String("unit")] = t6Unit;
    payload[String("save")] = t6Save;
    payload[String("publish")] = t6Publish;
    t6Client.createDatapoint(t6FlowId, payload);                  // Create a datapoint on t6
    t6Client.unlockSleep();                                       // Unlock the sleep mode
  }
}

void doServerQueries() {                                          // This section contains the triggers on www events
  String messageArrived = t6Client.pollWebServer();               // Poll Web Server to check if something happened
  String parameterArrived = t6Client._parameterArrived;
  if(messageArrived == "on") {                                    // Trigger if message is on
    //postSample();                                                 // Trigger postSample()
    setOn();

  } else if(messageArrived == "off") {                            // Trigger if message is off
    Serial.println("Set OFF and cancel all t6 api calls");        // But still readTask remain active so we can turn it on again
    setOff();

  } else if(messageArrived == "open") {                           // Trigger if message is open
    Serial.println("Opening something");

  } else if(messageArrived == "close") {                          // Trigger if message is close
    Serial.println("Closing something");

  } else if(messageArrived == "upper") {                          // Trigger if message is upper
    Serial.println("Increase volume");

  } else if(messageArrived == "lower") {                          // Trigger if message is lower
    Serial.println("Decrease volume");

  } else if(messageArrived == "true") {                           // Trigger if message is true
    digitalWrite(LED_BUILTIN, LOW);                               // Turn the LED on
    Serial.println("Boolean value is Activated");

  } else if(messageArrived == "false") {                          // Trigger if message is false
    digitalWrite(LED_BUILTIN, HIGH);                              // Turn the LED off
    Serial.println("Boolean value is Disabled");

  } else if(messageArrived == "setLight") {
    Serial.println("setLight ---->");
    Serial.println(parameterArrived);
    //analogWrite(LED_BUILTIN, parameterArrived.toInt());
    
  } else if(messageArrived != "") {
    Serial.print("UKN message arrived: ");
    Serial.println(messageArrived);
  }
  
  // reset value
  t6Client._messageArrived = "";
  t6Client._parameterArrived = "";
}

void setupComplete() {
  Serial.println("Ready.");
}

void printIPAddressOfHost(const char* host) {
  IPAddress resolvedIP;
  if (!WiFi.hostByName(host, resolvedIP)) {
    Serial.print(F("Host lookup failed for "));
    Serial.println(host);
  } else {
    Serial.print(F("Host lookup success for "));
    Serial.println(host);
  }
  Serial.print(F("Host: "));
  Serial.print(host);
  Serial.print(", IP: ");
  Serial.println(resolvedIP.toString().c_str());
}

void startWiFi(const char* ssid, const char* pswd) {
  Serial.println();
  Serial.print("Connecting to Wifi SSID:");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pswd);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    Serial.print(".");
    //ESP.restart();
  } else {
    Serial.println("WiFi Connected...");
    Serial.print("WiFi.getPhyMode()=");
    Serial.println(WiFi.getPhyMode());
    delay(1000);
  }
}
