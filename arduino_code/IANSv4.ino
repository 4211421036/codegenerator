// libraries
#include <WiFi.h>

// Wifi credentials
const char* ssid = "Galaxy A32AA2A";
const char* password = "fkhw8785";

// Soil sensor pin
const int soilSensorPin = 34;

// Initialize Wi-Fi client
WiFiClient client;

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set soil sensor pin as input
  pinMode(soilSensorPin, INPUT);
}

void loop() {
  // Read soil sensor value
  int soilMoisture = analogRead(soilSensorPin);

  // Convert sensor value to percentage
  int soilMoisturePercentage = map(soilMoisture, 4095, 1500, 0, 100);

  // Print soil moisture percentage to serial monitor
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisturePercentage);
  Serial.println("%");

  // Wait for 10 seconds before taking the next reading
  delay(10000);
}
