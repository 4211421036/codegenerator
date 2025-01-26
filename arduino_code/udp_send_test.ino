// This program sends UDP packets over Ethernet, using a specific MAC address and a user - defined remote IP.
// It repeatedly constructs a JSON message with device details and a greeting, then sends this message every 5 seconds to the specified IP address and port.
// COSMOS Web listens on a network port, so this test can be used to verify that functionality works.

#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <ArduinoJson.h>

// Teensy 4.1 MAC address, keep this the same.
uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// UDP settings
const IPAddress remoteIP(000, 000, 000, 000); // EDIT THIS TO YOUR COMPUTER'S IP ADDRESS
const unsigned int remotePort = 8888;

EthernetUDP Udp;

void setup()
{
    Serial.begin(9600);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB
    }
    Ethernet.begin(mac);
    // Ethernet.begin(mac, localIP, gateway, subnet);
    delay(1000);

    unsigned int localPort = 8888;
    Udp.begin(localPort);
}

void loop()
{
    // Create JSON object
    StaticJsonDocument<500> jsonDoc;
    jsonDoc["device"] = "Teensy 4.1";
    jsonDoc["Subject"] = "Test";
    jsonDoc["Message"] = "Hello World";

    // Convert JSON object to string
    char jsonBuffer[500];
    serializeJson(jsonDoc, jsonBuffer);

    // Send JSON string over UDP
    Udp.beginPacket(remoteIP, remotePort);
    Udp.write(jsonBuffer);
    Udp.endPacket();

    // Wait 5 seconds before sending the next JSON string
    delay(5000);
}

