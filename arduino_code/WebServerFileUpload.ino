#include <ESP8266WiFi.h>
#include <WebServerFileUpload.h>
#include <ESPAsyncWebServer.h>


const char* ssid = "your-ssid";
const char* password = "your-password";

AsyncWebServer server(80); // port 80
WebServerFileUpload serverInstance;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  
  serverInstance.begin(server);
}

void loop() {
}
