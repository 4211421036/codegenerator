#include <WiFi.h>
#include <WebServer.h>
#include "Ultrasonik.h"

const int trigPin = 12;
const int echoPin = 13;

long duration;
float distanceCm;

Ultrasonic ult;

// Replace with your network credentials
#define WIFI_SSID "Galaxy"
#define WIFI_PASSWORD "fkhw8785"

WebServer server(80);

String readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  ult.read()
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * 0.0344 / 2;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  
  // Return the distance as a string
  return String(distanceCm);
}

void handleRoot() {
  String distanceStr = readDistance();
  server.send(200, "text/plain", distanceStr);
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}
