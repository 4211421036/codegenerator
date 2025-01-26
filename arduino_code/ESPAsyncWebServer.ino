#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "TemplateTango.h"

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

AsyncWebServer server(80);

// Functions to simulate sensor readings
string readTemperature() { return "25.5"; }
string readSensor1() { return "100.1"; }
string readSensor2() { return "200.2"; }

void setup() {
    Serial.begin(115200);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Define the template string
    string templateStr = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <title>ESPAsyncWebServer Example</title>
        </head>
        <body>
            <h1>Sensor Data</h1>
            <p>Current Temperature: {{temperature}} °C</p>
            <p>Temperature in Fahrenheit: {{(temperature * 9 / 5) + 32}} °F</p>
            <p>Sensor 1 Reading: {{sensor1_reading}}</p>
            <p>Sensor 2 Reading: {{sensor2_reading}}</p>
        </body>
        </html>
    )";

    // Define the route to handle the main page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Collect the variable values
        std::map<string, string> variables = {
            {"temperature", readTemperature()},
            {"sensor1_reading", readSensor1()},
            {"sensor2_reading", readSensor2()}
        };

        // Render the template with the collected variables
        string processedTemplate = TemplateTango::render(templateStr, variables);
        request->send(200, "text/html", processedTemplate.c_str());
    });

    // Start server
    server.begin();
}

void loop() {
    // Main loop
}