// This example is supposed to work on all boards that use the WiFiNINA library.

#include <WiFiNINA.h>
#include <Arduino_MultiWiFi.h>

MultiWiFi multi;

void setup() {
    // Wait until the serial port is connected
    Serial.begin(9600);
    while (!Serial) {}

    // Configure the known networks (first one gets higher priority)
    multi.add("myssid", "mypassphrase");
    multi.add("myssid_2", "mypassphrase_2");

    // Connect to the first available network
    Serial.println("Looking for a network...");
    if (multi.run() == WL_CONNECTED) {
        Serial.print("Successfully connected to network: ");
        Serial.println(WiFi.SSID());
    } else {
        Serial.println("Failed to connect to a WiFi network");
    }
}

void loop() {}
