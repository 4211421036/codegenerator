/*
   https://www.internetcollaboratif.info/features/sockets-connection/
   https://github.com/mathcoll/t6iot/blob/master/examples/nodeMCU-websockets-client/nodeMCU-websockets-client.ino

   Require
   Board ESP32>=2.0.6s
   CPU Frequency to at least 160MHz
   ESP32-WROOM-DA
   LITTLEFS ESP32 Sketch Data Upload https://github.com/lorol/arduino-esp32fs-plugin (or https://github.com/lorol/arduino-esp32littlefs-plugin)
   LITTLEFS ESP32 Sketch Data Upload at 115200bps

   Linux tools:
   - SSDP     : gssdp-discover -i eth0 -r 5
   - MDSN     : mdns-scan
   - Firewall : sudo tail -f /var/log/kern.log

*/
//#define ASYNC_TCP_SSL_ENABLED true
//#define _DISABLE_TLS_
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoJWT.h>
#include <WebSocketsClient.h>
#include <StreamString.h>
#include <base64.h>
#include <pins_arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266SAM.h>
#include <LittleFS.h>
#define FILEFS LittleFS

#include "Audio.h"
#include "mp3.h"
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioGeneratorMP3.h"
#include "AudioFileSourceID3.h"
#include "AudioOutputI2SNoDAC.h"
/*Audio audio;*/
AudioGeneratorWAV *wav;
AudioGeneratorMP3 *mp3;
AudioFileSourcePROGMEM *file;
/*AudioOutputI2SNoDAC *audioOutputi2s;*/
AudioFileSourceID3 *id3;

#define serial Serial
#include "config.h"
// Define I2S connections
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

// Define volume control pot connection
// ADC3 is GPIO 39
const int volControl = 39;

// Integer for volume level
int volume = 10;

bool WsConnected = false;
bool claimed = false;
bool reboot = false;
unsigned long lastUpdate = millis();
const int MAX_PAYLOAD_SIZE = 8 * 1024;
const int MAX_FILE_SIZE = 10240;
char file_buffer[MAX_FILE_SIZE];
int buffer_index = 0;

#ifdef ESP8266
  #define ESP_GETCHIPID ESP.getChipId()
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClientSecureBearSSL.h>
  #include <ESP8266mDNS.h>
  #include <ESP8266SSDP.h>
#elif ESP32
  #define ESP_GETCHIPID (uint32_t)ESP.getEfuseMac()
  #define LED_BUILTIN 2
  #include <WiFi.h>
  #include <HTTPClient.h>
  #include <ESPmDNS.h>
  #include <ESP32SSDP.h>
#endif

//ArduinoJWT jwt = ArduinoJWT("");
AsyncWebServer server(80);
WebSocketsClient webSocket;
const int BUFFER_SIZE = 1024;
unsigned char buffer[BUFFER_SIZE];

void claimObject() {
  serial.println("[WSc] claimObject to WS server. object_id: " + config.t6Object_id);
  String payload = "{\"object_id\": \"" + config.t6Object_id + "\"}";
  ArduinoJWT jwt = ArduinoJWT(config.t6ObjectSecretKey);
  DynamicJsonDocument json(256);
  String databuf;
  json["command"] = "claimObject";
  json["object_id"] = config.t6Object_id;
  json["signature"] = jwt.encodeJWT(payload);
  serializeJson(json, databuf);
  webSocket.sendTXT(databuf);
}

bool subscribe(String channel) {
  serial.println("[WSc] subscribe object to WS server. Channel: " + channel);
  if (claimed) {
    DynamicJsonDocument json(256);
    String databuf;
    json["command"] = "subscribe";
    json["channel"] = channel;
    serializeJson(json, databuf);
    webSocket.sendTXT(databuf);
    return true;
  } else {
    serial.println("[WSc] Error: Object is not yet claimed, can't subscribe.");
    return false;
  }
}

bool unsubscribe(String channel) {
  serial.println("[WSc] unsubscribe object to WS server. Channel: " + channel);
  if (claimed) {
    DynamicJsonDocument json(256);
    String databuf;
    json["command"] = "unsubscribe";
    json["channel"] = channel;
    serializeJson(json, databuf);
    webSocket.sendTXT(databuf);
    return true;
  } else {
    serial.println("[WSc] Error: Object is not yet claimed, can't unsubscribe.");
    return false;
  }
}

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
  const uint8_t* src = (const uint8_t*) mem;
  Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for (uint32_t i = 0; i < len; i++) {
    if (i % cols == 0) {
      Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    Serial.printf("%02X ", *src);
    src++;
  }
  Serial.printf("\n");
}

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string) {
  (void)cbData;
  Serial.printf("ID3 callback for: %s = '", type);

  if (isUnicode) {
    string += 2;
  }

  while (*string) {
    char a = *(string++);
    if (isUnicode) {
      string++;
    }
    Serial.printf("%c", a);
  }
  Serial.printf("'\n");
  Serial.flush();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  Serial.println("-----------------------------------");
  Serial.printf("[WSc] type: %d\n", type);
  //Serial.printf("[WSc] payload: %s\n", payload);
  switch (type) {
    case WStype_DISCONNECTED: {
        serial.printf("[WSc] Disconnected!\n");
        WsConnected = false;
        break;
      }
    case WStype_CONNECTED: {
        //serial.printf("[WSc] Connected to url: ws://%s:%s%s\n", String(config.wsHost), String(config.wsPort), String(config.wsPath));
        WsConnected = true;
        //serial.println("[WSc] SENT: Connected");
        break;
      }
    case WStype_TEXT: {
        serial.printf("[WSc] RESPONSE: %s\n", payload);
        StaticJsonDocument <256> jsonPayload;
        DeserializationError error = deserializeJson(jsonPayload, payload);
        if (error) {
          //serial.print(F("deserializeJson() failed: "));
          //serial.println(error.f_str());
          return;
        } else {
          const char* arduinoCommand = "";
          const char* val = "";
          const char* message = "";
          const char* socket_id = "";
          const char* measurement = "";
          int hallVal = 0;
          uint8_t pin = 0;

          if (jsonPayload["arduinoCommand"]) {
            arduinoCommand = jsonPayload["arduinoCommand"].as<const char*>();
          }
          if (jsonPayload["value"]) {
            val = jsonPayload["value"].as<const char*>();
          }
          if (jsonPayload["message"]) {
            val = jsonPayload["message"].as<const char*>();
          }
          if (jsonPayload["socket_id"]) {
            socket_id = jsonPayload["socket_id"].as<const char*>();
          }
          if (jsonPayload["pin"]) {
            pin = jsonPayload["pin"];
          }

          //serial.printf("payload: %s\n", jsonPayload);
          //serial.printf("[WSc] - arduinoCommand: %s\n", arduinoCommand);
          //serial.printf("[WSc] - pin: %d\n", pin);
          //serial.printf("[WSc] - value: %s\n", val);
          //serial.printf("[WSc] - channels: %s\n", channels);
          //serial.println();

          if (strcmp(arduinoCommand, "claimed") == 0) {
            serial.printf("[WSc] claimObject is accepted on WS server. socket_id: %s\n", socket_id);
            config.messageInterval = config.messageIntervalOnceClaimed;
            claimed = true;

            file = new AudioFileSourcePROGMEM( development_mp3, sizeof(development_mp3) );
            
            serial.println(">>>>>>>>>>>>>>>>>>>");
            serial.println(file_buffer);
            serial.println("<<<<<<<<<<<<<<<<<<<");
            AudioOutputI2SNoDAC *audioOutputi2s;
            audioOutputi2s = new AudioOutputI2SNoDAC(0);
            audioOutputi2s->SetGain(0.9);
            if(strcmp(config.audioEncoding, "MP3") == 0) {
              serial.println("AudioGeneratorMP3");
              mp3 = new AudioGeneratorMP3();
              //mp3->begin(file, audioOutputi2s);
              serial.println("AudioFileSourceID3");
              id3 = new AudioFileSourceID3(file);
              id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");
              serial.println("mp3->begin");
              mp3->begin(id3, audioOutputi2s);
              while(mp3->isRunning()) {
                if (!mp3->loop()) {
                  mp3->stop();
                  serial.println("mp3->stopped");
                }
              }
        
            } else if(strcmp(config.audioEncoding, "LINEAR16") == 0) {
              wav = new AudioGeneratorWAV();
              wav->begin(file, audioOutputi2s);
            }

            if ( subscribe("demo") ) {

            }

          } else if (strcmp(arduinoCommand, "info") == 0) {
            serial.printf("[WSc] - socket_id: %s\n", socket_id);
            serial.printf("[WSc] - message  : %s\n", message);

          } else if (strcmp(arduinoCommand, "listenTo") == 0) {
              serial.println("[WSc] listenTo");
              serial.printf("[WSc] - message  : %s\n", message);
              t6iotAudio.listenTo("https://frequence3.net-radio.fr/frequence3-128.mp3");

            } else if (strcmp(arduinoCommand, "claimRequest") == 0) {
            serial.println("[WSc] claimRequest");
            serial.printf("[WSc] - message  : %s\n", message);
            claimObject();

          } else if (strcmp(arduinoCommand, "analogWrite") == 0) {
            serial.println("[WSc] analogWrite");
            analogWrite(pin, atoi(val));

          } else if (strcmp(arduinoCommand, "digitalWrite") == 0) {
            serial.println("[WSc] digitalWrite");
            serial.printf("[WSc] value ==> %d\n", atoi(val));
            pinMode(pin, OUTPUT);
            if (atoi(val) == 0) {
              digitalWrite(pin, LOW);
            } else if (atoi(val) == 1) {
              digitalWrite(pin, HIGH);
            }

          } else if (strcmp(arduinoCommand, "analogRead") == 0) {
            String currentVal = String(analogRead(pin), DEC);
            serial.println("[WSc] analogRead: " + currentVal);
            webSocket.sendTXT( currentVal );

          } else if (strcmp(arduinoCommand, "digitalRead") == 0) {
            String currentVal = String(digitalRead(pin), DEC);
            serial.println("[WSc] digitalRead: " + currentVal);
            webSocket.sendTXT( currentVal );

          } else if (strcmp(arduinoCommand, "getPinMode") == 0) {
            serial.println("[WSc] getPinMode");
            //getPinMode(pin);

          } else if (strcmp(arduinoCommand, "setPinMode") == 0) {
            serial.println("[WSc] setPinMode");
            pinMode(pin, atoi(val));

          } else if (strcmp(arduinoCommand, "audioOutput") == 0) {
            serial.println("[WSc] audioOutput");
            /*
            AudioOutputI2SNoDAC *audioOutputi2s;
            audioOutputi2s = new AudioOutputI2SNoDAC();
            audioOutputi2s->begin();
            ESP8266SAM *sam = new ESP8266SAM;
            sam->Say(audioOutputi2s, val);
            delete sam;
            */
          } else if (strcmp(arduinoCommand, "tts") == 0) {
            serial.println("[WSc] tts audio file ---------------------------------<---"); // not being used
          } else if (strcmp(arduinoCommand, "measureRequest") == 0) {
            if (jsonPayload["measurement"]) {
              measurement = jsonPayload["measurement"].as<const char*>();
              if (strcmp(measurement, "hallRead") == 0) {
#ifdef ESP32
                hallVal = hallRead();
                const char* t6FlowId = "fake-flow-id-hallVal";
                Serial.print("[WSc] hallRead:");
                Serial.println(String(hallVal));
                DynamicJsonDocument payload(1024);
                payload[String("value")] = hallVal;
                payload[String("flow_id")] = t6FlowId;
                payload[String("mqtt_topic")] = "";
                payload[String("unit")] = "";
                payload[String("save")] = "false";
                payload[String("publish")] = "true";
                createDatapoint(t6FlowId, payload, false, config.t6wsKey, config.t6wsSecret);
#endif
              } else if (strcmp(measurement, "measurementConfig1") == 0) {
                Serial.println("[WSc] measurementConfig1:");

                DynamicJsonDocument payload(1024);
                const char* t6FlowId = "fake-flow-id-measurementConfig1";
                payload[String("value")] = 123456789; // TODO TODO TODO TODO TODO TODO TODO TODO TODO
                payload[String("flow_id")] = t6FlowId;
                payload[String("mqtt_topic")] = "";
                payload[String("unit")] = "";
                payload[String("save")] = "false";
                payload[String("publish")] = "true";
                createDatapoint(t6FlowId, payload, false, config.t6wsKey, config.t6wsSecret);
                triggerNextMeasure(10 * 60 * 1000, "remindMeToMeasure", "measurementConfig1", config.t6Object_id); // 10 minutes;

              } else if (strcmp(measurement, "measurementConfig2") == 0) {
                const char* t6FlowId = "fake-flow-id-measurementConfig2";
                Serial.print("[WSc] measurementConfig2:");
                Serial.println(t6FlowId);
                getSSL();

              } else if (strcmp(measurement, "measurementConfig3") == 0) {
                const char* t6FlowId = "fake-flow-id-measurementConfig3";
                Serial.print("[WSc] measurementConfig3:");
                Serial.println(t6FlowId);

              }
            }
          }
        }
        break;
      }
    case WStype_BIN: {
        serial.printf("[WSc] WStype_BIN get binary length: %u\n", length);
        //hexdump(payload, length); // https://github.com/timum-viw/socket.io-client/issues/15

        if (length == MAX_PAYLOAD_SIZE) {
          // Check if there is enough space in the buffer for the chunk
          if (buffer_index + MAX_PAYLOAD_SIZE <= MAX_FILE_SIZE) {
            // Append the chunk to the buffer
            memcpy(file_buffer + buffer_index, payload, MAX_PAYLOAD_SIZE);
            buffer_index += MAX_PAYLOAD_SIZE;
          } else {
            // Not enough space in the buffer for the chunk
            // Do something to handle the error, such as resetting the buffer
            buffer_index = 0;
          }
        } else {
          // This is the last chunk of the message
          // Append the chunk to the buffer and process the complete file
          int last_chunk_size = length;
          if (buffer_index + last_chunk_size <= MAX_FILE_SIZE) {
            memcpy(file_buffer + buffer_index, payload, last_chunk_size);
            buffer_index += last_chunk_size;
            /*
            file = new AudioFileSourcePROGMEM( file_buffer, sizeof(file_buffer) );
            AudioOutputI2SNoDAC *audioOutputi2s;
            audioOutputi2s = new AudioOutputI2SNoDAC();
            
            if(strcmp(config.audioEncoding, "MP3") == 0) {
              mp3 = new AudioGeneratorMP3();
              mp3->begin(file, audioOutputi2s);
            } else if(strcmp(config.audioEncoding, "LINEAR16") == 0) {
              wav = new AudioGeneratorWAV();
              wav->begin(file, audioOutputi2s);
            }
            */

            serial.println("[WSc] tts audio stored to file_buffer");

            // Reset the buffer index for the next message
            buffer_index = 0;
          } else {
            // Not enough space in the buffer for the last chunk
            // Do something to handle the error, such as resetting the buffer
            buffer_index = 0;
          }
        }
        break;
      }
    case WStype_PING: {
        serial.printf("[WSc] get ping\n"); // pong will be send automatically
        break;
      }
    case WStype_PONG: {
        serial.printf("[WSc] get pong\n"); // answer to a ping we send
        break;
      }
    case WStype_ERROR: {
        serial.printf("[WSc] WStype_ERROR\n");
        break;
      }
    case WStype_FRAGMENT_TEXT_START: {
        serial.printf("[WSc] WStype_FRAGMENT_TEXT_START\n");
        break;
      }
    case WStype_FRAGMENT_BIN_START: {
        serial.printf("[WSc] WStype_FRAGMENT_BIN_START\n");
        break;
      }
    case WStype_FRAGMENT: {
        serial.printf("[WSc] WStype_FRAGMENT\n");
        break;
      }
    case WStype_FRAGMENT_FIN: {
        serial.printf("[WSc] WStype_FRAGMENT_FIN\n");
        break;
      }
    default:
      break;
  }
}

String processor(const String& var) {
  serial.println(var);
  return var;
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
  for (int i = 0; i < static_cast<int>(str.length()); i++) {
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

String getSignedPayload(String& payload, String& objectId, String& secret) { // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
  ArduinoJWT jwt = ArduinoJWT(secret);
  String signedJson;
  String payloadString;
  String signedPayloadAsString;
  Serial.print(payload);
  Serial.print(objectId);
  /*
    payload.printTo(payloadString);
    signedJson = jwt.encodeJWT( payloadString );

    const int BUFFER_SIZE = JSON_OBJECT_SIZE(25);
    StaticJsonBuffer<BUFFER_SIZE> jsonBufferSigned;
    JsonObject& signedPayload = jsonBufferSigned.createObject();
    signedPayload["signedPayload"] = signedJson;
    signedPayload["object_id"] = objectId;
    signedPayload.prettyPrintTo(Serial);

    signedPayload.printTo(signedPayloadAsString);
  */
  return signedPayloadAsString;
}
void triggerNextMeasure(int ttl, const char* command, const char* measurement, String t6Object_id) {
  Serial.println("[t6IoT] Rescheduling next measurement in " + String(ttl / 1000) + "s");
  DynamicJsonDocument json(256);
  String databuf;
  json["command"] = command;
  json["measurement"] = measurement;
  json["object_id"] = t6Object_id;
  json["delay"] = ttl;
  serializeJson(json, databuf);
  webSocket.sendTXT(databuf);
}

void createDatapoint(const char* flowId, DynamicJsonDocument& payload, bool useSignature, String t6wsKey, String t6wsSecret) {
  Serial.println("[t6IoT] Creating datapoint to t6:");
  Serial.print(t6wsKey);
  Serial.print(t6wsSecret);

  String payloadStr;
  serializeJson(payload, payloadStr);
  if (useSignature) {
    payloadStr = getSignedPayload(payloadStr, config.t6Object_id, config.t6ObjectSecretKey);
  }

  if (config.apiScheme == "https://") {
    Serial.println("[t6IoT] Using SSL certificate");
#ifdef ESP8266
    BearSSL::WiFiClientSecure client;
#elif ESP32
    WiFiClientSecure client;
    client.setCACert(rootCACertificate);
#endif
    int conn = client.connect(String(config.apiHost).c_str(), config.apiPort);
    if (conn == 1) {
      client.println("POST " + String( config._urlDataPoint + String(flowId) ) + " HTTP/1.1");
      //Headers
      client.print("Host: "); client.println(String(config.apiHost).c_str());
      client.print("User-Agent: "); client.println("t6iot-library " + String(config.friendlyName));
      client.print("Accept: "); client.println("application/json");
      client.print("Content-Type: "); client.println("application/json");
      client.print("Cache-Control: "); client.println("no-cache");
      client.print("Accept-Encoding: "); client.println("gzip, deflate, br");
      client.print("x-api-key: "); client.println(config.t6wsKey);
      client.print("x-api-secret: "); client.println(config.t6wsSecret);
      client.print("Content-Length: "); client.println(String((payloadStr).length()));
      client.println("Connection: Close");
      client.println();
      client.println(payloadStr); //Body
      client.println();

      //Wait for server response
      while (client.available() == 0);
      //Print Server Response
      while (client.available()) {
        char c = client.read();
        Serial.write(c);
      }
    } else {
      client.stop();
      Serial.println("[t6IoT] Connection Failed");
    }
  } else {
    Serial.println("[t6IoT] Not using SSL certificate");
    WiFiClient client;
    HTTPClient http;
    http.begin(client, String(config.apiScheme) + String(config.apiHost).c_str() + ":" + String(config.apiPort).c_str() + String( config._urlDataPoint + String(flowId) ));
    http.addHeader("User-Agent", "t6iot-library " + String(config.friendlyName));
    http.addHeader("Accept", "application/json");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Cache-Control", "no-cache");
    http.addHeader("Accept-Encoding", "gzip, deflate, br");
    http.addHeader("x-api-key", config.t6wsKey);
    http.addHeader("x-api-secret", config.t6wsSecret);
    http.addHeader("Content-Length", String((payloadStr).length()));
    http.addHeader("Connection", "Close");
    int httpResponseCode = http.POST(payloadStr); //Body
    Serial.print("[t6IoT] Response Code:");
    Serial.println(httpResponseCode);
    http.end();
  }
}

void getSSL() {
#ifdef ESP32
  WiFiClientSecure *client = new WiFiClientSecure;
  client->setInsecure();
  HTTPClient https;
  if (!https.begin(*client, config.apiScheme + config.apiHost )) {
    Serial.println("[t6IoT] HTTPS setup failed");
    return;
  };
  https.setTimeout(5000);
  int httpCode = https.GET();
  if (httpCode != 200) {
    Serial.print("[t6IoT] Connect failed: ");
    Serial.println(https.errorToString(httpCode));
    return;
  }
  const mbedtls_x509_crt* peer = client->getPeerCertificate();
  // Show general output / certificate information
  char buf[1024];
  int l = mbedtls_x509_crt_info (buf, sizeof(buf), "", peer);
  if (l <= 0) {
    Serial.println("Peer conversion to printable buffer failed");
    return;
  };
  Serial.println();
  Serial.println(buf);
  uint8_t fingerprint_remote[32];
  if (!client->getFingerprintSHA256(fingerprint_remote)) {
    Serial.println("[t6IoT] Failed to get the fingerprint");
    return;
  }
  Serial.println("[t6IoT] Expecting Fingerprint (SHA256): " + String(fingerprint));
  Serial.print(  "[t6IoT]  Received Fingerprint (SHA256): ");
  for (int i = 0; i < 32; i++) {
    Serial.print(fingerprint_remote[i], HEX);
    Serial.print(" ");
  };
  Serial.println("");
#endif
};

void setup() {
  Serial.begin(115200);
  //serial.setDebugOutput(true);
  Serial.flush();
  Serial.println("[t6IoT] BOOT");

  if (!FILEFS.begin()) {
    serial.println("[t6IoT] An Error has occurred while mounting FS");
    return;
  } else {
    serial.println("[t6IoT] FS mounted");
    loadConfiguration(configFilename, config);
  }
  delay(250);
  if ( config.ssid && config.password ) {
    serial.println("[t6IoT] Wifi activated");
    WiFi.begin(config.ssid, config.password);
    while ( WiFi.status() != WL_CONNECTED ) {
      delay(1000);
    }
    WiFi.setSleep(false);
    serial.print("[t6IoT] Local IP:");
    serial.println(WiFi.localIP());
    serial.print("[t6IoT] Wifi SSID:");
    serial.println(config.ssid);
    serial.print("[t6IoT] Wifi RRSI:");
    serial.println(WiFi.RSSI());
    serial.println("[t6IoT] Wifi started");
  } else {
    serial.println("[t6IoT] Wifi not configured");
  }

  if ( config.ServiceStatusAudio == true ) {
    //audioOutput = new AudioOutputI2SNoDAC();
    //audioOutput->begin();
    //audioOutput.SetPinout(26, 25, 22); //set the pinout for your board
    //audioOutput->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    //audioOutput->SetGain(0.125); //set the output gain
    
    // Connect MAX98357 I2S Amplifier Module
    //audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    //audio.setVolume(volume);
    //audio.connecttohost("0n-80s.radionetz.de:8000/0n-70s.mp3");
    
    serial.println("[t6IoT] audioOutput started");
  } else {
    serial.println("[t6IoT] audioOutput not activated");
  }

  if ( config.ServiceStatusMdns == true ) {
    MDNS.begin(String(config.friendlyName).c_str());
    MDNS.addService("http", "tcp", config.localPortMDNS);
    serial.println("[t6IoT] MDNS started");
  } else {
    serial.println("[t6IoT] MDNS not activated");
  }

  if ( config.ServiceStatusSockets == true ) {
    webSocket.begin(config.wsHost, config.wsPort, config.wsPath);
    webSocket.onEvent(webSocketEvent);
    webSocket.setAuthorization(( String("Basic " + base64::encode(String(config.t6wsKey + ":" + config.t6wsSecret).c_str())) ).c_str());
    webSocket.setReconnectInterval(config.reconnectInterval);
    webSocket.enableHeartbeat(config.messageInterval, config.timeoutInterval, config.disconnectAfterFailure);
    serial.print("[t6IoT] WebSockets connecting to ");
    serial.print(config.wsHost);
    serial.print(":");
    serial.print(config.wsPort);
    serial.println(config.wsPath);
    serial.println("[t6IoT] WebSockets started");
  } else {
    serial.println("[t6IoT] WebSockets not activated");
  }

  if ( config.ServiceStatusHttp == true ) {
    server.on("/digitalWrite", HTTP_GET, [](AsyncWebServerRequest * request) {
      String _pin = ( request->getParam( (config.PARAM_INPUT_PIN) )->value() );
      String _value = ( request->getParam( (config.PARAM_INPUT_VALUE) )->value() );
      if (_pin && _value) {
        Serial.print("pinMode: ");
        pinMode(_pin.toInt(), OUTPUT);
        Serial.println("OUTPUT");
        if (_value.toInt() == 0) {
          digitalWrite(_pin.toInt(), LOW);
        } else if (_value.toInt() == 1) {
          digitalWrite(_pin.toInt(), HIGH);
        }
      }
      Serial.print("GPIO: "); Serial.print(_pin);
      Serial.print(" is set to: "); Serial.println(_value);
      request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \"" + String(_pin) + "\", \"value\": \"" + String(_value) + "\"}");
    });
    server.on("/analogWrite", HTTP_GET, [](AsyncWebServerRequest * request) {
      String _pin = ( request->getParam( (config.PARAM_INPUT_PIN) )->value() );
      String _value = ( request->getParam( (config.PARAM_INPUT_VALUE) )->value() );
      if (_pin && _value) {
        Serial.print("pinMode: ");
        pinMode(_pin.toInt(), OUTPUT);
        Serial.println("OUTPUT");
        analogWrite(_pin.toInt(), _value.toInt());
      }
      Serial.print("GPIO: "); Serial.print(_pin);
      Serial.print(" is set to: "); Serial.println(_value);
      request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \"" + String(_pin) + "\", \"value\": \"" + String(_value) + "\"}");
    });
    server.on("/setPinMode", HTTP_GET, [](AsyncWebServerRequest * request) {
      String _pin = ( request->getParam( (config.PARAM_INPUT_PIN) )->value() );
      String _mode = ( request->getParam( (config.PARAM_INPUT_MODE) )->value() );
      if (_pin && _mode) {
        if (_mode == "INPUT") {
          pinMode(_pin.toInt(), INPUT);
          request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \"" + String(_pin) + "\", \"value\": \"INPUT\"}");
        } else if (_mode == "OUTPUT") {
          pinMode(_pin.toInt(), OUTPUT);
          request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \"" + String(_pin) + "\", \"value\": \"OUTPUT\"}");
        } else {
          pinMode(_pin.toInt(), OUTPUT);
          request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \"" + String(_pin) + "\", \"value\": \"OUTPUT\"}");
        }
      }
    });
    server.on("/analogRead", HTTP_GET, [](AsyncWebServerRequest * request) {
      String _pin = ( request->getParam( (config.PARAM_INPUT_PIN) )->value() );
      String currentVal = "";
      int analogValue = 0;
      Serial.print(analogValue);
      if (_pin) {
#ifdef ESP32
        //set the resolution to 12 bits (0-4096)
        analogReadResolution(12);
        analogValue = analogReadMilliVolts(_pin.toInt());
#elif ESP8266
        int analogValue = analogRead(_pin.toInt());
#endif
        currentVal = String(analogValue, DEC);
      }
      serial.println("analogRead: " + currentVal);
      request->send(200, "application/json", "{\"status\": \"OK\", \"pin\": \"" + _pin + "\", \"value\": \"" + currentVal + "\"}");
    });
    server.on("/digitalRead", HTTP_GET, [](AsyncWebServerRequest * request) {
      String _pin = ( request->getParam( (config.PARAM_INPUT_PIN) )->value() );
      String currentVal;
      if (_pin) {
        currentVal = String(digitalRead(_pin.toInt()), DEC);
      }
      serial.println("digitalRead: " + currentVal);
      request->send(200, "application/json", "{\"status\": \"OK\", \"pin\": \"" + _pin + "\", \"value\": \"" + currentVal.toInt() + "\"}");
    });
    server.on("/audioOutput", HTTP_GET, [](AsyncWebServerRequest * request) {
      String _value = ( request->getParam( (config.PARAM_INPUT_VALUE) )->value() );
      String message_decoded;
      if (_value) {
        /*
        AudioOutputI2SNoDAC *audioOutputi2s;
        audioOutputi2s = new AudioOutputI2SNoDAC();
        ESP8266SAM *sam = new ESP8266SAM;
        message_decoded = urldecode(_value);
        const char* inputMessage2 = message_decoded.c_str();
        sam->Say(audioOutputi2s, inputMessage2); // String(inputMessage2).c_str()
        delete sam;
        */
      }
      request->send(201, "application/json", "{\"status\": \"OK\", \"snack\": \"Message played " + message_decoded + "\"}");
    });
    server.on("/getValues", HTTP_GET, [](AsyncWebServerRequest * request) {
      String _pin = ( request->getParam( (config.PARAM_INPUT_PIN) )->value() );
      String currentVal;
      String output;
      const byte allPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39};
      const byte COUNT = sizeof(allPins);
      if (_pin) {
        StaticJsonDocument<4096> jsonValues;
        JsonArray pins = jsonValues.createNestedArray("pins");
        for (int p = 0; p < COUNT; p++) {
          JsonObject npin = pins[p].createNestedObject("pin" + String(allPins[p]));
          npin["mode"] = getPinMode(allPins[p]);
          npin["type"] = "digital";
          npin["value"] = String(digitalRead((allPins[p])), DEC);
        }
        serializeJson(jsonValues, output);
        serial.println("");
        serial.println(output);
      }
      request->send(200, "application/json", output);
    });
    server.on("/subscribe", HTTP_GET, [](AsyncWebServerRequest * request) {
      String _channel = ( request->getParam( (config.PARAM_INPUT_CHANNEL) )->value() );
      if (_channel) {
        if ( subscribe(_channel) ) {
          request->send(201, "application/json", "{\"status\": \"OK\", \"subscribe\": \"" + _channel + "\", \"subscriptions\": \"undefined\", \"snack\": \"Subscribed to channel '" + _channel + "'\"}");
        } else {
          request->send(412, "application/json", "{\"status\": \"NOT\", \"subscribe\": \"" + _channel + "\", \"subscriptions\": \"undefined\", \"snack\": \"Failed to subscribe to channel '" + _channel + "'\"}");
        }
      }
    });
    server.on("/heap", HTTP_GET, [](AsyncWebServerRequest * request) {
      /*
        int headers = request->headers();
        int i;
        for(i=0;i<headers;i++){
        AsyncWebHeader* h = request->getHeader(i);
        Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
        }
      */
      String heap = String(ESP.getFreeHeap());
      request->send(200, "application/json", "{\"status\": \"OK\", \"heap\": \"" + heap + "\", \"snack\": \"Heap: " + heap + "\"}");
    });
    server.on("/unsubscribe", HTTP_GET, [](AsyncWebServerRequest * request) {
      String _channel = ( request->getParam( (config.PARAM_INPUT_CHANNEL) )->value() );
      if (_channel) {
        if ( unsubscribe(_channel) ) {
          request->send(201, "application/json", "{\"status\": \"OK\", \"unsubscribe\": \"" + _channel + "\", \"subscriptions\": \"undefined\", \"snack\": \"Unsubscribed from channel '" + _channel + "'\"}");
        } else {
          request->send(412, "application/json", "{\"status\": \"NOT\", \"unsubscribe\": \"" + _channel + "\", \"subscriptions\": \"undefined\", \"snack\": \"Failed to unsubscribe to channel '" + _channel + "'\"}");
        }
      }
    });
    server.on("/config", HTTP_PUT, [](AsyncWebServerRequest * request) {
      Serial.println("handling HTTP_PUT");
    },
    [](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
      Serial.println("handling fileUploadCB");
    },
    [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
      Serial.println("handling bodyRequestCB"); // (char*)data
      StaticJsonDocument<4096> body;
      DeserializationError error = deserializeJson(body, data);
      if (error) {
        Serial.println("[t6IoT] Failed to load body");
        Serial.println(error.f_str());
        request->send(412, "application/json", "{\"status\": \"NOK\", \"snack\": \"Configuration has not been saved! (1)\"}");
      } else {
        if ( body["wifi"]["ssid"].as<const char*>() ) {
          config.ssid = body["wifi"]["ssid"].as<const char*>();
        }
        if ( body["wifi"]["password"].as<const char*>() ) {
          config.password = body["wifi"]["password"].as<const char*>();
        }

        if ( body["t6"]["servicesStatus"]["http"].as<String>() == "true" ) {
          config.ServiceStatusHttp = true;
        } else if ( body["t6"]["servicesStatus"]["http"].as<String>() == "false" ) {
          config.ServiceStatusHttp = false;
        }
        if ( body["t6"]["servicesStatus"]["sockets"].as<String>() == "true" ) {
          config.ServiceStatusSockets = true;
        } else if ( body["t6"]["servicesStatus"]["sockets"].as<String>() == "false" ) {
          config.ServiceStatusSockets = false;
        }
        if ( body["t6"]["servicesStatus"]["mdns"].as<String>() == "true" ) {
          config.ServiceStatusMdns = true;
        } else if ( body["t6"]["servicesStatus"]["mdns"].as<String>() == "false" ) {
          config.ServiceStatusMdns = false;
        }
        if ( body["t6"]["servicesStatus"]["ssdp"].as<String>() == "true" ) {
          config.ServiceStatusSsdp = true;
        } else if ( body["t6"]["servicesStatus"]["ssdp"].as<String>() == "false" ) {
          config.ServiceStatusSsdp = false;
        }
        if ( body["t6"]["servicesStatus"]["audio"].as<String>() == "true" ) {
          config.ServiceStatusAudio = true;
        } else if ( body["t6"]["servicesStatus"]["audio"].as<String>() == "false" ) {
          config.ServiceStatusAudio = false;
        }

        if ( body["t6"]["t6Object_id"].as<const char*>() ) {
          config.t6Object_id = body["t6"]["t6Object_id"].as<String>();
        }
        if ( body["t6"]["t6ObjectSecretKey"].as<const char*>() ) {
          config.t6ObjectSecretKey = body["t6"]["t6ObjectSecretKey"].as<String>();
        }
        if ( body["t6"]["host"].as<const char*>() ) {
          config.apiHost = body["t6"]["host"].as<String>();
        }
        if ( body["t6"]["port"].as<const char*>() ) {
          config.apiPort = body["t6"]["port"].as<uint16_t>();
        }
        if ( body["t6"]["scheme"].as<const char*>() ) {
          config.apiScheme = body["t6"]["scheme"].as<String>();
        }

        if ( body["t6"]["websockets"]["host"].as<const char*>() ) {
          config.wsHost = body["t6"]["websockets"]["host"].as<String>();
        }
        if ( body["t6"]["websockets"]["path"].as<const char*>() ) {
          config.wsPath = body["t6"]["websockets"]["path"].as<String>();
        }
        if ( body["t6"]["websockets"]["port"].as<const char*>() ) {
          config.wsPath = body["t6"]["websockets"]["port"].as<uint16_t>();
        }
        if ( body["t6"]["websockets"]["t6wsKey"].as<const char*>() ) {
          config.t6wsKey = body["t6"]["websockets"]["t6wsKey"].as<String>();
        }
        if ( body["t6"]["websockets"]["t6wsSecret"].as<const char*>() ) {
          config.t6wsSecret = body["t6"]["websockets"]["t6wsSecret"].as<String>();
        }
        if ( body["t6"]["websockets"]["expiration"].as<const char*>() ) {
          config.expiration = body["t6"]["websockets"]["expiration"].as<unsigned long>();
        }
        if ( body["t6"]["websockets"]["messageInterval"].as<const char*>() ) {
          config.messageInterval = body["t6"]["websockets"]["messageInterval"].as<unsigned long>();
        }
        if ( body["t6"]["websockets"]["messageIntervalOnceClaimed"].as<const char*>() ) {
          config.messageIntervalOnceClaimed = body["t6"]["websockets"]["messageIntervalOnceClaimed"].as<unsigned long>();
        }
        if ( body["t6"]["websockets"]["reconnectInterval"].as<const char*>() ) {
          config.reconnectInterval = body["t6"]["websockets"]["reconnectInterval"].as<unsigned long>();
        }
        if ( body["t6"]["websockets"]["timeoutInterval"].as<const char*>() ) {
          config.timeoutInterval = body["t6"]["websockets"]["timeoutInterval"].as<unsigned long>();
        }
        if ( body["t6"]["websockets"]["disconnectAfterFailure"].as<const char*>() ) {
          config.disconnectAfterFailure = body["t6"]["websockets"]["disconnectAfterFailure"].as<int>();
        }

        if ( body["t6"]["ssdp"]["localPort"].as<unsigned int>() ) {
          config.localPortSSDP = body["t6"]["ssdp"]["localPort"].as<unsigned int>();
        }
        if ( body["t6"]["ssdp"]["advertiseInterval"].as<unsigned int>() ) {
          config.advertiseInterval = body["t6"]["ssdp"]["advertiseInterval"].as<unsigned int>();
        }
        if ( body["t6"]["ssdp"]["presentationURL"].as<const char*>() ) {
          config.presentationURL = body["t6"]["ssdp"]["presentationURL"].as<String>();
        }
        if ( body["t6"]["ssdp"]["friendlyName"].as<const char*>() ) {
          config.friendlyName = body["t6"]["ssdp"]["friendlyName"].as<String>();
        }
        if ( body["t6"]["ssdp"]["modelName"].as<const char*>() ) {
          config.modelName = body["t6"]["ssdp"]["modelName"].as<String>();
        }
        if ( body["t6"]["ssdp"]["modelNumber"].as<const char*>() ) {
          config.modelNumber = body["t6"]["ssdp"]["modelNumber"].as<String>();
        }
        if ( body["t6"]["ssdp"]["deviceType"].as<const char*>() ) {
          config.deviceType = body["t6"]["ssdp"]["deviceType"].as<String>();
        }
        if ( body["t6"]["ssdp"]["modelURL"].as<const char*>() ) {
          config.modelURL = body["t6"]["ssdp"]["modelURL"].as<String>();
        }
        if ( body["t6"]["ssdp"]["manufacturer"].as<const char*>() ) {
          config.manufacturer = body["t6"]["ssdp"]["manufacturer"].as<String>();
        }
        if ( body["t6"]["ssdp"]["manufacturerURL"].as<const char*>() ) {
          config.manufacturerURL = body["t6"]["ssdp"]["manufacturerURL"].as<String>();
        }

        if ( body["t6"]["http"]["localPort"].as<unsigned int>() ) {
          config.localPortHTTP = body["t6"]["http"]["localPort"].as<unsigned int>();
        }
        if ( body["t6"]["http"]["localPortHTTPS"].as<unsigned int>() ) {
          config.localPortHTTPS = body["t6"]["http"]["localPortHTTPS"].as<unsigned int>();
        }

        if ( body["t6"]["mdns"]["localPort"].as<unsigned int>() ) {
          config.localPortMDNS = body["t6"]["mdns"]["localPort"].as<unsigned int>();
        }

        if ( saveConfiguration(configFilename, config) ) {
          request->send(200, "application/json", "{\"status\": \"OK\", \"snack\": \"Configuration has been saved, please restart ESP.\"}");
        } else {
          request->send(412, "application/json", "{\"status\": \"NOK\", \"snack\": \"Configuration has not been saved! (2)\"}");
        }
      }
    });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/index.html", "text/html");
    });
    server.on("/index", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/index.html", "text/html");
    });
    server.on("/sw.js", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/sw.js", "text/javascript");
    });
    server.on("/fonts/Material-Icons.woff2", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/fonts/Material-Icons.woff2", "font/woff2");
    });
    server.on("/t6show.js", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/t6show.js", "text/javascript");
    });
    server.on("/t6show-min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/t6show-min.js", "text/javascript");
    });
    server.on("/t6show-min.js.map", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/t6show-min.js.map", "application/json");
    });
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/style.css", "text/css");
    });
    server.on("/t6app.min.css", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/t6app.min.css", "text/css");
    });
    server.on("/object.css", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/object.css", "text/css");
    });
    server.on("/ui.js", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/ui.js", "application/javascript");
    });
    server.on("/robots.txt", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/robots.txt", "plain/text");
    });
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/favicon.ico", "image/x-icon");
    });
    server.on("/icon-16x16.png", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/icon-16x16.png", "image/x-png");
    });
    server.on("/icon-32x32.png", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/icon-32x32.png", "image/x-png");
    });
    server.on("/config.json", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/config.json", "application/json");
    });
    if ( config.ServiceStatusSsdp == true ) {
      server.on("/description.xml", HTTP_GET, [](AsyncWebServerRequest * request) {
        StreamString output;
        if (output.reserve(512)) {
          output.printf(ssdpTemplate,
                        WiFi.localIP().toString().c_str(),
                        String(config.deviceType).c_str(),
                        String(config.friendlyName).c_str(),
                        String(config.presentationURL).c_str(),
                        String(config.t6Object_id + "").c_str(),
                        String(config.modelName).c_str(),
                        String(config.modelNumber).c_str(),
                        String(config.modelURL).c_str(),
                        String(config.manufacturer).c_str(),
                        String(config.manufacturerURL).c_str(),
                        (uint8_t) ((ESP_GETCHIPID >> 16) & 0xff),
                        (uint8_t) ((ESP_GETCHIPID >>  8) & 0xff),
                        (uint8_t)   ESP_GETCHIPID        & 0xff
                       );
          request->send(200, "text/xml", (String)output);
        } else {
          request->send(500);
        }
      });
    }
    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(FILEFS, "/reboot.html", "text/html");
      reboot = true;
    });
    server.onNotFound([] (AsyncWebServerRequest * request) {
      AsyncWebServerResponse* response = request->beginResponse(FILEFS, "/404.html", "text/html");
      response->setCode(404);
      request->send(response);
    });
    server.begin();
    serial.println("[t6IoT] HTTP listening to:");
    serial.print("[t6IoT] http://");
    serial.print(WiFi.localIP().toString().c_str());
    serial.print(":");
    serial.println(config.localPortHTTP);
    serial.println("[t6IoT] HTTP started");
  } else {
    serial.println("[t6IoT] HTTP not activated");
  }

  if ( config.ServiceStatusSsdp == true ) {
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(config.localPortSSDP);
    SSDP.setDeviceType(config.deviceType);
    SSDP.setName(config.friendlyName);
    SSDP.setSerialNumber(String(ESP_GETCHIPID));
    SSDP.setModelName(config.modelName);
    SSDP.setModelNumber(config.modelNumber);
    SSDP.setModelURL(config.modelURL);
    SSDP.setManufacturer(config.manufacturer);
    SSDP.setManufacturerURL(config.manufacturerURL);
    SSDP.setInterval(config.advertiseInterval);
    SSDP.setURL("/");
    SSDP.begin();
    serial.println("[t6IoT] SSDP started");
  } else {
    serial.println("[t6IoT] SSDP not activated");
  }

  /*
    tmrpcm.speakerPin = 9;
    tmrpcm.quality(1);
    tmrpcm.setVolume(5);
  */
  serial.println("[t6IoT] READY");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  if ( reboot == true ) {
    delay(2000);
    reboot = false;
    ESP.restart();
  }
  if ( config.ServiceStatusSockets == true ) {
    webSocket.loop();
    if ( WsConnected && lastUpdate + config.messageInterval < millis() ) {
      //claimObject();
      lastUpdate = millis();
    }
  }
  delay(250);
}
