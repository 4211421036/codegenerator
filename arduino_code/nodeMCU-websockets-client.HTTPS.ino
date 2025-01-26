/*
 * https://www.internetcollaboratif.info/features/sockets-connection/
 * https://github.com/mathcoll/t6iot/blob/master/examples/nodeMCU-websockets-client/nodeMCU-websockets-client.ino
 * 
 * openssl
 * req -x509 -newkey rsa:1024 -sha256 -keyout key.txt -out cert.txt -days 365 -nodes -subj "/C=FR/ST=P/L=Paris/O=InternetCollaboratif [FR]/OU=t6IoT/CN=esp8266.local" -addext subjectAltName=DNS:esp8266.local,IP:192.168.0.19
 *
 * Set CPU Frequency to at least 240MHz
 * https://github.com/lorol/arduino-esp32littlefs-plugin
 * ESP32-WROOM-DA
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoJWT.h>
#include <WebSocketsClient.h>
#include <StreamString.h>
#include <base64.h>
#include <pins_arduino.h>
#include <ESP8266SAM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <FS.h>

#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>

#include "AudioOutputI2SNoDAC.h"
#include <LittleFS.h>
#define FILEFS LittleFS
#define ASYNC_TCP_SSL_ENABLED true
#define serial Serial

#ifdef ESP8266
  #define ESP_GETCHIPID ESP.getChipId()
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include <ESP8266SSDP.h>
#elif ESP32
  #define ESP_GETCHIPID (uint32_t)ESP.getEfuseMac()
  #define LED_BUILTIN 2
  #include <WiFi.h>
  #include <ESPmDNS.h>
  #include <ESP32SSDP.h>
#endif

// Wifi
const char *ssid                = "";
const char *password            = "";

// t6
char wsHost[]                   = "ws.internetcollaboratif.info"; //"";
uint16_t wsPort                 = 80; //80;
char wsPath[]                   = "/ws";
String t6wsKey                  = "";
String t6wsSecret               = "";
long expiration                 = 1695124232506;
const char * t6Object_id        = "";
const char * t6ObjectSecretKey  = "";

// t6 web sockets
bool sockets_active             = true;
unsigned long messageInterval   = 15000; //  15 secs // 10000 * 60 * 10; // 10 mins
              // Once Claimed, reschedule next message not before a long time
unsigned long messageIntervalOnceClaimed   = 60000 * 10; //  15 secs // 60000 * 10; // 10 mins
unsigned long reconnectInterval = 5000;  //  5  secs
unsigned long timeoutInterval   = 3000;  //  3  secs
int disconnectAfterFailure      = 2;     // consider connection disconnected if pong is not received 2 times
const char* PARAM_INPUT_1       = "pin";
const char* PARAM_INPUT_2       = "value";

// MDNS
bool mdns_active                = false;

// SSDP & HTTP & HTTPS
bool ssdp_active                = false;
int localPortHTTP               = 80;
int localPortHTTPS              = 443;
int advertiseInterval           = 60 * 10;
const char * presentationURL    = "/";
const char * friendlyName       = "t6Object";
const char * modelName          = "t6 IoT";
const char * modelNumber        = "1.0.0";
const char * deviceType         = "upnp:rootdevice";
const char * modelURL           = "https://github.com/mathcoll/t6iot";
const char * manufacturer       = "Internet Collaboratif";
const char * manufacturerURL    = "https://www.internetcollaboratif.info";

// Audio Output
bool audio_active               = true;
AudioOutputI2SNoDAC *audioOutput = NULL;

bool connected = false;
bool claimed = false;
unsigned long lastUpdate = millis();

String numbers[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "height", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "heighteen", "nineteen", "twenty", "twenty one"};
WebSocketsClient webSocket;
ArduinoJWT jwt = ArduinoJWT(t6ObjectSecretKey);
BearSSL::ESP8266WebServerSecure serverHTTPS(localPortHTTPS);
BearSSL::ServerSessions serverCache(5);

File fileIndex;

static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC5jCCAk+gAwIBAgIUGPnNqtBVqMioe5XvJnt61GvTJ+cwDQYJKoZIhvcNAQEL
BQAwdTELMAkGA1UEBhMCRlIxCjAIBgNVBAgMAVAxDjAMBgNVBAcMBVBhcmlzMSIw
IAYDVQQKDBlJbnRlcm5ldENvbGxhYm9yYXRpZiBbRlJdMQ4wDAYDVQQLDAV0Nklv
VDEWMBQGA1UEAwwNZXNwODI2Ni5sb2NhbDAeFw0yMjEwMjMxMjA5MjNaFw0yMzEw
MjMxMjA5MjNaMHUxCzAJBgNVBAYTAkZSMQowCAYDVQQIDAFQMQ4wDAYDVQQHDAVQ
YXJpczEiMCAGA1UECgwZSW50ZXJuZXRDb2xsYWJvcmF0aWYgW0ZSXTEOMAwGA1UE
CwwFdDZJb1QxFjAUBgNVBAMMDWVzcDgyNjYubG9jYWwwgZ8wDQYJKoZIhvcNAQEB
BQADgY0AMIGJAoGBAME8hsBdoQBdzshIuQw+lEX7hZvO/0yesBvQksrRGj2SlXN/
0hJicpVNZkLH4UhJ0wXMbf/08h+LLWUaNEAd0gtJETA9VHnLOJkp7L3LYyJsJ1z4
rVJHwT8Roi/+P7gAT96KdeqmqETtOir54G6DbA3KyoCSg7XnLY0kMSlYdueNAgMB
AAGjczBxMB0GA1UdDgQWBBR5soPG9v1fTlLQ0wI7UK8IQURQXzAfBgNVHSMEGDAW
gBR5soPG9v1fTlLQ0wI7UK8IQURQXzAPBgNVHRMBAf8EBTADAQH/MB4GA1UdEQQX
MBWCDWVzcDgyNjYubG9jYWyHBMCoABMwDQYJKoZIhvcNAQELBQADgYEAPxBLl+BB
KSMYK/Cv1Jo5iHBGFe5HSAY6QkLW4PXyHASLJ3OVY/g8esU3QFNz0caXCZaIy+a2
Dxer/Qau6+83njPlavSWtChf1n3D5prxZXGZr4ttJYp5TgkoL4q7kUEldleQJW5A
0ngxyvaiNqfOf+E3a7PtJ31xi3bmXzZ1Wbs=
-----END CERTIFICATE-----
)EOF";

static const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN PRIVATE KEY-----
MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAME8hsBdoQBdzshI
uQw+lEX7hZvO/0yesBvQksrRGj2SlXN/0hJicpVNZkLH4UhJ0wXMbf/08h+LLWUa
NEAd0gtJETA9VHnLOJkp7L3LYyJsJ1z4rVJHwT8Roi/+P7gAT96KdeqmqETtOir5
4G6DbA3KyoCSg7XnLY0kMSlYdueNAgMBAAECgYEArF0q7YgWxtbmTizgMxXW+J2k
zhhUDao2+HiRl6vxNh78hGvtnEyIXLkPkF3ui6aePrvdGsUI/C4PLfGzzcVLxwq2
iuRiEDURamGA6dd/ej/rStAlAQZ5YlH4QkrGJpBQATqcGysSojjkmpwCuaC/kiOv
yUDc0gyf/J45PQ3jroECQQD0ZwdTPCoOPzEEOmZqw6J/QHaHM+bdNfb2PRggoirV
Sd/bOEwq605QQ0fupvtSqLl1bm1uGNr34rqr2dqZcX1xAkEAymfwaOdtmXXiTAbP
eJ2HuRkOdEcdZf8ktVJZW04/LlciifEE1EKp7Suh3qWeO0/IzmSFxQmvbOERRpyA
mBzt3QJATUyozHKi+o6R+jxeWDY4FvdUhb6srhp7WbZJA0lHMt6CpttPe+OgiVI+
VgUKSEKFJdcfVUsXNn/HesMUmKOEgQJAcICDTRvNfG3coZtreEQ/Ky/1K8ZXXdkK
Q0qJZknArnY6OWfg4kZHgw8sgr+ViqwXFjkjbVirzB3xZ0kX5PS3dQJBALwrpgYr
xWfBpFarFSe6h4MGIQRPt+PqnRxiKoPEUaA80qE6h+cycnrD0RlM5kZ3Kqd3Vjd/
v0USLNzQPzPb/Fs=
-----END PRIVATE KEY-----
)EOF";

static const char* configTemplate =
  "let config={"
  "  \"object_ip\": \"%s\","
  "  \"object_port\": \"%s\","
  "  \"wsHost\": \"%s\","
  "  \"wsPort\": \"%s\","
  "  \"wsPath\": \"%s\","
  "  \"t6Object_id\": \"%s\","
  "  \"token\": \"%s\""
  "};\r\n\r\n";

static const char* ssdpTemplate =
  "<?xml version=\"1.0\"?>"
  "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
  "  <specVersion>"
  "    <major>1</major>"
  "    <minor>0</minor>"
  "  </specVersion>"
  "  <URLBase>https://%s/</URLBase>"
  "  <device>"
  "    <deviceType>%s</deviceType>"
  "    <friendlyName>%s</friendlyName>"
  "    <presentationURL>%s</presentationURL>"
  "    <serialNumber>%u</serialNumber>"
  "    <modelName>%s</modelName>"
  "    <modelNumber>%s</modelNumber>"
  "    <modelURL>%s</modelURL>"
  "    <manufacturer>%s</manufacturer>"
  "    <manufacturerURL>%s</manufacturerURL>"
  "    <UDN>uuid:a774f8f2-c180-4e26-8544-cda0e6%02x%02x%02x</UDN>"
  "  </device>"
  "</root>\r\n\r\n";

void claimObject(const char * id) {
  serial.println("[WSc] claimObject to WS server. object_id: " + String(id));
  DynamicJsonDocument json(256);
  String databuf;
  json["command"] = "claimObject";
  json["object_id"] = String(id);
  String payload = "{\"object_id\": \""+String(t6Object_id)+"\"}";
  String signature = jwt.encodeJWT(payload);
  json["signature"] = signature;
  serializeJson(json, databuf);
  webSocket.sendTXT(databuf);
}

void subscribe(const char * channel) {
  serial.println("[WSc] subscribe object to WS server. " + String(channel));
  DynamicJsonDocument json(256);
  String databuf;
  json["command"] = "subscribe";
  json["channel"] = String(channel);
  serializeJson(json, databuf);
  webSocket.sendTXT(databuf);
}

void unsubscribe(const char * channel) {
  serial.println("[WSc] unsubscribe object to WS server. " + String(channel));
  DynamicJsonDocument json(256);
  String databuf;
  json["command"] = "unsubscribe";
  json["channel"] = String(channel);
  serializeJson(json, databuf);
  webSocket.sendTXT(databuf);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED: {
              serial.printf("[WSc] Disconnected!\n");
              connected = false;
          }
          break;
        case WStype_CONNECTED: {
            serial.printf("[WSc] Connected to url: %s\n", payload);
            connected = true;
            // send message to server when Connected
            //serial.println("[WSc] SENT: Connected");
            //webSocket.sendTXT("Connected");
          }
          break;
        case WStype_TEXT: {
          serial.printf("[WSc] RESPONSE: %s\n", payload);
          StaticJsonDocument <256> jsonPayload;
          DeserializationError error = deserializeJson(jsonPayload, payload);
            if (error) {
              //Serial.print(F("deserializeJson() failed: "));
              //Serial.println(error.f_str());
              return;
            } else {
              const char* arduinoCommand;
              const char* val = "";
              const char* message = "";
              const char* socket_id = "";
              uint8_t pin;

              if (jsonPayload["arduinoCommand"]) {
                arduinoCommand = jsonPayload["arduinoCommand"].as<char*>();
              }
              if (jsonPayload["value"]) {
                val = jsonPayload["value"].as<char*>();
              }
              if (jsonPayload["message"]) {
                val = jsonPayload["message"].as<char*>();
              }
              if (jsonPayload["socket_id"]) {
                socket_id = jsonPayload["socket_id"].as<char*>();
              }
              if (jsonPayload["pin"]) {
                pin = jsonPayload["pin"];
              }

              serial.printf("payload: %s\n", jsonPayload);
              serial.printf("- arduinoCommand: %s\n", arduinoCommand);
              serial.printf("- pin: %d\n", pin);
              serial.printf("- value: %s\n", val);
              serial.println();

              if (strcmp(arduinoCommand, "claimed") == 0) {
                  serial.printf("[WSc] claimObject is accepted on WS server. socket_id: %s\n", socket_id);
                  messageInterval = messageIntervalOnceClaimed;
                  claimed = true;

              } else if (strcmp(arduinoCommand, "info") == 0) {
                serial.println("info ===================");
                serial.printf("- pin: %d\n", pin);
                serial.printf("- value: %s\n", val);
                serial.printf("- message: %s\n", message);
                serial.printf("- socket_id: %s\n", socket_id);
                serial.println("info ===================");

              } else if (strcmp(arduinoCommand, "claimRequest") == 0) {
                serial.println("claimRequest");
                claimObject( t6Object_id );
                
                subscribe("demo");
                //subscribe("channel_1");
                //subscribe("channel_2");
                //subscribe("channel_3");
                //unsubscribe("channel_1");
                //unsubscribe("channel_2");
                //unsubscribe("channel_3");

              } else if (strcmp(arduinoCommand, "analogWrite") == 0) {
                serial.println("analogWrite");
                analogWrite(pin, atoi(val));

              } else if (strcmp(arduinoCommand, "digitalWrite") == 0) {
                serial.println("digitalWrite");
                serial.printf("value ==> %d\n", atoi(val));
                digitalWrite(pin, atoi(val));

              } else if (strcmp(arduinoCommand, "analogRead") == 0) {
                String currentVal = String(analogRead(pin), DEC);
                serial.println("analogRead: "+currentVal);
                webSocket.sendTXT( currentVal );

              } else if (strcmp(arduinoCommand, "digitalRead") == 0) {
                String currentVal = String(digitalRead(pin), DEC);
                serial.println("digitalRead: "+currentVal);
                webSocket.sendTXT( currentVal );

              } else if (strcmp(arduinoCommand, "getPinMode") == 0) {
                serial.println("getPinMode");
                //getPinMode(pin);
                
              } else if (strcmp(arduinoCommand, "setPinMode") == 0) {
                serial.println("setPinMode");
                pinMode(pin, atoi(val));
                
              } else if (strcmp(arduinoCommand, "audioOutput") == 0) {
                serial.println("audioOutput");
                ESP8266SAM *sam = new ESP8266SAM;
                sam->Say(audioOutput, val);
                delete sam;   
              }
            }
          }
          break;
        case WStype_BIN: {
            serial.printf("[WSc] get binary length: %u\n", length);
            //hexdump(payload, length); // https://github.com/timum-viw/socket.io-client/issues/15
          }
          break;
        case WStype_PING: {
            serial.printf("[WSc] get ping\n"); // pong will be send automatically
          }
          break;
        case WStype_PONG: {
            serial.printf("[WSc] get pong\n"); // answer to a ping we send
          }
          break;
        default:
          break;
    }
}

String processor(const String& var) {
  serial.println(var);
}

String getPinMode(int pin) {
  uint32_t bit = digitalPinToBitMask(pin);
  uint32_t port = digitalPinToPort(pin);
  volatile uint32_t *reg = portModeRegister(port);
  if (*reg & bit) {
      return "OUTPUT";
  } else {
      return "INPUT";
  }
}

unsigned char h2int(char c) {
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

String urldecode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == '+') {
      encodedString += ' ';
    } else if (c == '%') {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (h2int(code0) << 4) | h2int(code1);
      encodedString += c;
    } else {
      encodedString += c;
    }
    yield();
  }
  return encodedString;
}

void secureRedirect() {
  //serverHTTP.sendHeader("Location", String("https://192.168.0.19"), true);
  //serverHTTP.send(301, "text/plain", "");
}

String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpeg")) return "image/jpeg";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".json")) return "application/json";
  else if(filename.endsWith(".map")) return "application/json";
  else if(filename.endsWith(".woff2")) return "font/woff2";
  else if(filename.endsWith(".txt")) return "text/plain";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    size_t sent = serverHTTPS.streamFile(file, contentType);
    file.close();
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;
}

void webSocketEventaaaaaaaaaaaa(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: {
        serial.printf("[WSc] Disconnected!\n");
        connected = false;
      }
      break;
    case WStype_CONNECTED: {
        serial.printf("[WSc] Connected to url: %s\n", payload);
        connected = true;
        // send message to server when Connected
        //serial.println("[WSc] SENT: Connected");
        //webSocket.sendTXT("Connected");
      }
      break;
    case WStype_TEXT: {
      serial.printf("[WSc] RESPONSE: %s\n", payload);
      StaticJsonDocument <128> jsonPayload;
      DeserializationError error = deserializeJson(jsonPayload, payload);
        if (error) {
          //Serial.print(F("deserializeJson() failed: "));
          //Serial.println(error.f_str());
          return;
        } else {
          const char* arduinoCommand;
          const char* val = "";
          const char* message = "";
          const char* socket_id = "";
          uint8_t pin;

          if (jsonPayload["arduinoCommand"]) {
            arduinoCommand = jsonPayload["arduinoCommand"].as<char*>();
          }
          if (jsonPayload["value"]) {
            val = jsonPayload["value"].as<char*>();
          }
          if (jsonPayload["message"]) {
            val = jsonPayload["message"].as<char*>();
          }
          if (jsonPayload["socket_id"]) {
            socket_id = jsonPayload["socket_id"].as<char*>();
          }
          if (jsonPayload["pin"]) {
            pin = jsonPayload["pin"];
          }

          serial.printf("payload: %s\n", jsonPayload);
          serial.printf("- arduinoCommand: %s\n", arduinoCommand);
          serial.printf("- pin: %d\n", pin);
          serial.printf("- value: %s\n", val);
          serial.println();

          if (strcmp(arduinoCommand, "claimed") == 0) {
              serial.printf("[WSc] claimObject is accepted on WS server. socket_id: %s\n", socket_id);
              messageInterval = messageIntervalOnceClaimed;
              claimed = true;

          } else if (strcmp(arduinoCommand, "info") == 0) {
            serial.println("info ===================");
            serial.printf("- pin: %d\n", pin);
            serial.printf("- value: %s\n", val);
            serial.printf("- message: %s\n", message);
            serial.printf("- socket_id: %s\n", socket_id);
            serial.println("info ===================");

          } else if (strcmp(arduinoCommand, "claimRequest") == 0) {
            serial.println("claimRequest");
            claimObject( t6Object_id );
            
            subscribe("demo");
            //subscribe("channel_1");
            //subscribe("channel_2");
            //subscribe("channel_3");
            //unsubscribe("channel_1");
            //unsubscribe("channel_2");
            //unsubscribe("channel_3");

          } else if (strcmp(arduinoCommand, "analogWrite") == 0) {
            serial.println("analogWrite");
            analogWrite(pin, atoi(val));

          } else if (strcmp(arduinoCommand, "digitalWrite") == 0) {
            serial.println("digitalWrite");
            serial.printf("value ==> %d\n", atoi(val));
            digitalWrite(pin, atoi(val));

          } else if (strcmp(arduinoCommand, "analogRead") == 0) {
            String currentVal = String(analogRead(pin), DEC);
            serial.println("analogRead: "+currentVal);
            webSocket.sendTXT( currentVal );

          } else if (strcmp(arduinoCommand, "digitalRead") == 0) {
            String currentVal = String(digitalRead(pin), DEC);
            serial.println("digitalRead: "+currentVal);
            webSocket.sendTXT( currentVal );

          } else if (strcmp(arduinoCommand, "getPinMode") == 0) {
            serial.println("getPinMode");
            //getPinMode(pin);
            
          } else if (strcmp(arduinoCommand, "setPinMode") == 0) {
            serial.println("setPinMode");
            pinMode(pin, atoi(val));
            
          } else if (strcmp(arduinoCommand, "audioOutput") == 0) {
            if(audio_active) {
              serial.println("audioOutput");
              ESP8266SAM *sam = new ESP8266SAM;
              sam->Say(audioOutput, val);
              delete sam;
            } else {
              serial.println("audioOutput disabled");
            }
          }
        }
      }
      break;
    case WStype_BIN: {
        serial.printf("[WSc] get binary length: %u\n", length);
        hexdump(payload, length);
      }
      break;
    case WStype_PING: {
        serial.printf("[WSc] get ping\n"); // pong will be send automatically
      }
      break;
    case WStype_PONG: {
        serial.printf("[WSc] get pong\n"); // answer to a ping we send
      }
      break;
    default:
      break;
  }
}
 
void setup() {
    serial.begin(115200);
    serial.flush();
    serial.println("[t6IoT] BOOT");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);

    if(!SPIFFS.begin()){
      serial.println("[SPIFFS] An Error has occurred while mounting SPIFFS");
      return;
    }
 
    WiFi.begin(ssid, password);
    while ( WiFi.status() != WL_CONNECTED ) {
      delay(500);
      Serial.print(".");
    }
    serial.print("[WiFi] Local IP: ");
    serial.println(WiFi.localIP());

    serverHTTPS.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
    serverHTTPS.getServer().setCache(&serverCache);
    /*
    serverHTTPS.onNotFound([](){
      if (!handleFileRead(serverHTTP.uri())) {
        serial.println("[Https] Serving /404.html");
        File file404 = SPIFFS.open("/404.html", "r");
        serverHTTPS.streamFile(file404, getContentType("/404.html"));
      }
    });
    */

/* ************************ */
/* ***** STATIC FILES ***** */
/* ************************ */
    serverHTTPS.on("/", [](){
      File fileIndex = SPIFFS.open("/index.html", "r");
      serial.println("[Https] Serving /index.html");
      serverHTTPS.send(200, getContentType(".html"), (String)fileIndex.readString());
      fileIndex.close();
    });
    serverHTTPS.on("/index.html", [](){
      File fileIndex = SPIFFS.open("/index.html", "r");
      serial.println("[Https] Serving /index.html");
      serverHTTPS.send(200, getContentType(".html"), (String)fileIndex.readString());
      fileIndex.close();
    });
    serverHTTPS.on("/t6app.min.css", [](){
      File filet6appCss = SPIFFS.open("/t6app.min.css", "r");
      serial.println("[Https] Serving /t6app.min.css");
      serverHTTPS.send(200, getContentType(".css"), (String)filet6appCss.readString());
      filet6appCss.close();
    });
    serverHTTPS.on("/object.css", [](){
      File fileObjectCss = SPIFFS.open("/object.css", "r"); 
      serial.println("[Https] Serving /object.css");
      serverHTTPS.send(200, getContentType(".css"), (String)fileObjectCss.readString());
      fileObjectCss.close();
    });
    serverHTTPS.on("/ui.js", [](){
      File fileUi = SPIFFS.open("/ui.js", "r");
      serial.println("[Https] Serving /ui.js");
      serverHTTPS.send(200, getContentType(".js"), (String)fileUi.readString());
      fileUi.close();
    });
    serverHTTPS.on("/sw.js", [](){
      File fileSW = SPIFFS.open("/sw.js", "r");
      serial.println("[Https] Serving /sw.js");
      serverHTTPS.send(200, getContentType(".js"), (String)fileSW.readString());
      fileSW.close();
    });
    serverHTTPS.on("/t6show.js", [](){
      File filet6Js = SPIFFS.open("/t6show.js", "r");
      serial.println("[Https] Serving /t6show.js");
      serverHTTPS.send(200, getContentType(".js"), (String)filet6Js.readString());
      filet6Js.close();
    });
    serverHTTPS.on("/t6show-min.js", [](){
      File filet6MinJs = SPIFFS.open("/t6show-min.js", "r");
      serial.println("[Https] Serving /t6show-min.js");
      serverHTTPS.send(200, getContentType(".js"), (String)filet6MinJs.readString());
      filet6MinJs.close();
    });
    serverHTTPS.on("/t6show-min.js.map", [](){
      File file6MinJsMap = SPIFFS.open("/t6show-min.js.map", "r");
      serial.println("[Https] Serving /t6show-min.js.map");
      serverHTTPS.send(200, getContentType(".map"), (String)file6MinJsMap.readString());
      file6MinJsMap.close();
    });
    serverHTTPS.on("/icon-16x16.png", [](){
      File fileIcon16 = SPIFFS.open("/icon-16x16.png", "r");
      serial.println("[Https] Serving /icon-16x16.png");
      serverHTTPS.send(200, getContentType(".png"), (String)fileIcon16.readString());
      fileIcon16.close();
    });
    serverHTTPS.on("/icon-32x32.png", [](){
      File fileIcon32 = SPIFFS.open("/icon-32x32.png", "r");
      serial.println("[Https] Serving /icon-32x32.png");
      serverHTTPS.send(200, getContentType(".png"), (String)fileIcon32.readString());
      fileIcon32.close();
    });
    serverHTTPS.on("/robots.txt", [](){
      File fileRobots = SPIFFS.open("/robots.txt", "r");
      serial.println("[Https] Serving /robots.txt");
      serverHTTPS.send(200, getContentType(".txt"), (String)fileRobots.readString());
      fileRobots.close();
    }); 
    serverHTTPS.on("/fonts/Material-Icons.woff2", [](){
      File fileMdlIcons = SPIFFS.open("/fonts/Material-Icons.woff2", "r");
      serial.println("[Https] Serving /fonts/Material-Icons.woff2");
      serverHTTPS.send(200, getContentType(".woff2"), (String)fileMdlIcons.readString());
      fileMdlIcons.close();
    });

/* ************************ */
/* **** DYNAMIC FILES ***** */
/* ************************ */
    serverHTTPS.on("/object-conf-old.js", [](){
     serial.println("[Https] Serving /object-conf.js");
     serverHTTPS.send(200, getContentType(".js"), (String)"let config={\"object_ip\": \""+WiFi.localIP().toString().c_str()+"\",\"object_port\": \""+String(localPortHTTPS)+"\",\"wsHost\": \""+String(wsHost).c_str()+"\",\"wsPort\": \""+String(wsPort)+"\",\"wsPath\": \""+wsPath+"\",\"t6Object_id\": \""+t6Object_id+"\",\"token\": \""+String(t6wsKey+":"+t6wsSecret).c_str()+"\"};");
    });
    serverHTTPS.on("/object-conf.js", [](){
     serial.println("[Https] Serving /object-conf.js");
     StreamString output;
     serial.println("[Https] Serving ...");
     output.printf(configTemplate,
       WiFi.localIP().toString().c_str(),
       String(localPortHTTPS),
       String(wsHost).c_str(),
       String(wsPort),
       wsPath,
       t6Object_id,
       String(t6wsKey+":"+t6wsSecret).c_str()
     );
     serverHTTPS.send(200, getContentType(".js"), (String)output);
    });
    serverHTTPS.on("/digitalWrite", [](){
      serial.println("[Https] Serving /digitalWrite");
      String inputMessage1;
      String inputMessage2;
      // GET input value on <ESP_IP>/digitalWrite?pin=<inputMessage1>&value=<inputMessage2>
      if (serverHTTPS.hasArg(PARAM_INPUT_1)) {
        inputMessage1 = serverHTTPS.arg(PARAM_INPUT_1);
      }
      if (serverHTTPS.hasArg(PARAM_INPUT_2)) {
        inputMessage2 = serverHTTPS.arg(PARAM_INPUT_2);
      }
      digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
      Serial.print("GPIO: ");
      Serial.print(inputMessage1);
      Serial.print(" - Set to: ");
      Serial.println(inputMessage2.toInt());
      serverHTTPS.send(201, getContentType(".json"), "{\"status\": \"OK\", \"pin\": \""+ String(inputMessage1.toInt()) +"\", \"value\": \""+String(inputMessage2.toInt())+"\"}");
    });

    serverHTTPS.on("/digitalRead", [](){
      serial.println("[Https] Serving /digitalRead");
      String inputMessage1;
      String currentVal;
      // GET input value on <ESP_IP>/digitalRead?pin=<inputMessage1>
      if (serverHTTPS.hasArg(PARAM_INPUT_1)) {
        inputMessage1 = serverHTTPS.arg(PARAM_INPUT_1);
        currentVal = String(digitalRead(inputMessage1.toInt()), DEC);
      }
      serial.println("digitalRead: "+currentVal);
      Serial.print("GPIO: ");
      Serial.print(inputMessage1);
      Serial.print(" - Get: ");
      Serial.println(currentVal);
      serverHTTPS.send(200, getContentType(".json"), "{\"status\": \"OK\", \"pin\": \""+ String(inputMessage1.toInt()) +"\", \"value\": \""+String(currentVal.toInt())+"\"}");
    });

    serverHTTPS.on("/analogWrite", [](){
      serial.println("[Https] Serving /analogWrite");
      String inputMessage1;
      String inputMessage2;
      // GET input value on <ESP_IP>/analogWrite?pin=<inputMessage1>&value=<inputMessage2>
      if (serverHTTPS.hasArg(PARAM_INPUT_1)) {
        inputMessage1 = serverHTTPS.arg(PARAM_INPUT_1);
      }
      if (serverHTTPS.hasArg(PARAM_INPUT_2)) {
        inputMessage2 = serverHTTPS.arg(PARAM_INPUT_2);
      }
      analogWrite(inputMessage1.toInt(), inputMessage2.toInt());
      Serial.print("GPIO: ");
      Serial.print(inputMessage1);
      Serial.print(" - Set to: ");
      Serial.println(inputMessage2.toInt());
      serverHTTPS.send(201, getContentType(".json"), "{\"status\": \"OK\", \"pin\": \""+ String(inputMessage1.toInt()) +"\", \"value\": \""+String(inputMessage2.toInt())+"\"}");
    });

    serverHTTPS.on("/analogRead", [](){
      serial.println("[Https] Serving /analogRead");
      String inputMessage1;
      String currentVal;
      // GET input value on <ESP_IP>/analogRead?pin=<inputMessage1>
      if (serverHTTPS.hasArg(PARAM_INPUT_1)) {
        inputMessage1 = serverHTTPS.arg(PARAM_INPUT_1);
        currentVal = String(analogRead(inputMessage1.toInt()), DEC);
      }
      serial.println("analogRead: "+currentVal);
      Serial.print("GPIO: ");
      Serial.print(inputMessage1);
      Serial.print(" - Get: ");
      Serial.println(currentVal);
      serverHTTPS.send(200, getContentType(".json"), "{\"status\": \"OK\", \"pin\": \""+ String(inputMessage1.toInt()) +"\", \"value\": \""+String(currentVal)+"\"}");
    });

    serverHTTPS.on("/setPinModeOutput", []() {
      serial.println("[Https] Serving /setPinModeOutput");
      String inputMessage1;
      String inputMessage2;
      // GET input value on <ESP_IP>/setPinModeOutput?pin=<inputMessage1>&value=<inputMessage2>
      if (serverHTTPS.hasArg(PARAM_INPUT_1)) {
        inputMessage1 = serverHTTPS.arg(PARAM_INPUT_1);
        pinMode(inputMessage1.toInt(), OUTPUT);
      }
      serverHTTPS.send(201, getContentType(".json"), "{\"status\": \"OK\", \"pin\": \""+ String(inputMessage1.toInt()) +"\", \"value\": \"OUTPUT\"}");
    });

    serverHTTPS.on("/setPinModeInput", []() {
      serial.println("[Https] Serving /setPinModeInput");
      String inputMessage1;
      String inputMessage2;
      // GET input value on <ESP_IP>/setPinModeInput?pin=<inputMessage1>&value=<inputMessage2>
      if (serverHTTPS.hasArg(PARAM_INPUT_1)) {
        inputMessage1 = serverHTTPS.arg(PARAM_INPUT_1);
        pinMode(inputMessage1.toInt(), INPUT);
      }
      serverHTTPS.send(201, getContentType(".json"), "{\"status\": \"OK\", \"pin\": \""+ String(inputMessage1.toInt()) +"\", \"value\": \"INPUT\"}");
    });

    serverHTTPS.on("/audioOutput", []() {
      if(audio_active) {
        serial.println("[Https] Serving /audioOutput");
        String inputMessage2;
        // GET input1 value on <ESP_IP>/audioOutput?value=<inputMessage2>
        if (serverHTTPS.hasArg(PARAM_INPUT_2)) {
          inputMessage2 = serverHTTPS.arg(PARAM_INPUT_2);
          ESP8266SAM *sam = new ESP8266SAM;
          String message_decoded = urldecode(inputMessage2);
          const char* inputMessage2 = message_decoded.c_str();
          sam->Say(audioOutput, inputMessage2); // String(inputMessage2).c_str()
          delete sam;
        }
        serverHTTPS.send(201, getContentType(".json"), "{\"status\": \"OK\", \"value\": \""+String(inputMessage2)+"\"}");
      } else {
        serial.println("[Https] Serving /audioOutput - disabled");
        serverHTTPS.send(412, getContentType(".json"), "{\"status\": \"NOK\"}");
      }
    });
    
    serverHTTPS.on("/getValues", []() {
      serial.println("[Https] Serving /getValues");
      String inputMessage1;
      String currentVal;
      String output;
      const byte allPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
      const byte COUNT = sizeof(allPins);
      // GET input1 value on <ESP_IP>/getValues?pin=<inputMessage1>
      if (serverHTTPS.hasArg(PARAM_INPUT_1)) {
        inputMessage1 = serverHTTPS.arg(PARAM_INPUT_1);
        StaticJsonDocument<1024> jsonValues;
        JsonArray pins = jsonValues.createNestedArray("pins");
        for (int p = 0; p < COUNT; p++) {
          JsonObject npin = pins[p].createNestedObject("pin"+String(allPins[p]));
          npin["mode"] = getPinMode(allPins[p]);
          npin["type"] = "digital";
          npin["value"] = String(digitalRead((allPins[p])), DEC);
        }
        serializeJson(jsonValues, output);
        serial.println("");
        serial.println(output);
      }
      serverHTTPS.send(200, getContentType(".json"), output);
    });

    serverHTTPS.on("/description.xml", []() {
      serial.println("[Https] Serving /description.xml");
      StreamString output;
      if(output.reserve(512)) {
        uint32_t chipId = ESP.getChipId();
        output.printf(ssdpTemplate,
          WiFi.localIP().toString().c_str(),
          deviceType,
          friendlyName,
          presentationURL,
          String(ESP.getChipId()),
          modelName,
          modelNumber,
          modelURL,
          manufacturer,
          manufacturerURL,
          (uint8_t) ((chipId >> 16) & 0xff),
          (uint8_t) ((chipId >>  8) & 0xff),
          (uint8_t)   chipId        & 0xff
        );
        serverHTTPS.send(200, getContentType("description.xml"), (String)output);
      } else {
        serverHTTPS.send(500);
      }
    });


/* ************************ */
/* ***** TEST FILES ******* */
/* ************************ */
    serverHTTPS.on("/test1", [](){
     serial.println("[Https] Serving /t6show-min.js from SPIFFS open");
      File file = SPIFFS.open("/t6show-min.js", "r");
      file.close();
    });
    serverHTTPS.on("/test2", [](){
      serial.println("[Https] Serving /t6show-min.js from SPIFFS w/ available");
      File file = SPIFFS.open("/t6show-min.js", "r");
      while(file.available()){
        serverHTTPS.streamFile(file, getContentType("/t6show-min.js"));
      }
      file.close();
    });
    serverHTTPS.on("/test3", [](){
      serial.println("[Https] Serving /t6show-min.js from SPIFFS w/ available");
      File file = SPIFFS.open("/t6show-min.js", "r");
      if (serverHTTPS.streamFile(file, getContentType("/t6show-min.js")) != file.size()) {
        serial.println("[Https] != file.size()");
      }
      file.close();
    });
/* ************************ */
/* ************************ */
/* ************************ */

    //serverHTTP.begin();
    //serial.println("[Http] Server began");

    serverHTTPS.begin();
    serverHTTPS.enableCORS(true);
    serial.println("[Https] Server began");

    if( audio_active ) {
      audioOutput = new AudioOutputI2SNoDAC();
      audioOutput->begin();
      serial.println("[Audio] Server began");
    }

    if( mdns_active ) {
      MDNS.begin(friendlyName);
      MDNS.addService("http", "tcp", localPortHTTP);
      serial.println("[Mdns] Server began");
    }

    if( ssdp_active ) {
      SSDP.setSchemaURL("description.xml");
      SSDP.setHTTPPort(localPortHTTP);
      SSDP.setDeviceType(deviceType);
      SSDP.setName(friendlyName);
      SSDP.setSerialNumber(String(ESP.getChipId()));
      SSDP.setModelName(modelName);
      SSDP.setModelNumber(modelNumber);
      SSDP.setModelURL(modelURL);
      SSDP.setManufacturer(manufacturer);
      SSDP.setManufacturerURL(manufacturerURL);
      SSDP.setURL("/");
      SSDP.setInterval(advertiseInterval);
      SSDP.begin();
      serial.println("[Ssdp] Server began");
    }
    
    if( sockets_active ) {
      webSocket.begin(wsHost, wsPort, wsPath);
      webSocket.onEvent(webSocketEvent);
      basic_token = String("Basic " + base64::encode(String(t6wsKey+":"+t6wsSecret).c_str()));
      t6wsBase64Auth = basic_token.c_str();
      webSocket.setAuthorization(t6wsBase64Auth);
      webSocket.setReconnectInterval(reconnectInterval);
      webSocket.enableHeartbeat(messageInterval, timeoutInterval, disconnectAfterFailure);
      serial.println("[Wss] Server began");
    }

    serial.println("[t6IoT] READY");
}

void loop() {
  //serverHTTP.handleClient();
  serverHTTPS.handleClient();
  if( sockets_active ) {
    webSocket.loop();
  }
  if ( connected && lastUpdate+messageInterval<millis() ) {
    claimObject( t6Object_id );
    lastUpdate = millis();
  }
}
