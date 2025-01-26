// This program sets up an Ethernet connection using the NativeEthernet library and obtains the local IP address, subnet mask, gateway IP, and DNS server IP.
// It continuously checks the Ethernet link status and prints the connection information if the Ethernet is connected, otherwise it indicates that the Ethernet is not connected and repeats the check every 10 seconds.

#include <NativeEthernet.h>
#include <IPAddress.h>

// Ethernet settings
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup()
{
    // Start the serial communication
    Serial.begin(9600);

    // Wait for the serial monitor to be opened
    while (!Serial)
        ;

    // Start the Ethernet connection
    Serial.println("Starting Ethernet connection...");
    if (Ethernet.begin(mac) == 0)
    {
        Serial.println("Failed to configure Ethernet using DHCP");
        while (1)
            ;
    }

    // Print the local IP address
    Serial.print("Local IP: ");
    Serial.println(Ethernet.localIP());
}

void loop()
{
    // Check Ethernet link status
    if (Ethernet.linkStatus() == LinkON)
    {
        Serial.println("Ethernet is connected.");

        // MAC address
        Serial.print("MAC address: ");
        for (int i = 0; i < 6; ++i)
        {
            Serial.print(mac[i], HEX);
            if (i < 5)
                Serial.print(":");
        }
        Serial.println();

        // IP, subnet mask, gateway, and DNS
        IPAddress localIP = Ethernet.localIP();
        IPAddress subnet = Ethernet.subnetMask();
        IPAddress gateway = Ethernet.gatewayIP();
        IPAddress dns = Ethernet.dnsServerIP();

        Serial.print("Local IP: ");
        Serial.println(localIP);
        Serial.print("Subnet Mask: ");
        Serial.println(subnet);
        Serial.print("Gateway IP: ");
        Serial.println(gateway);
        Serial.print("DNS Server IP: ");
        Serial.println(dns);
    }
    else
    {
        Serial.println("Ethernet is not connected.");
    }

    // Wait 5 seconds before checking again
    delay(10000);
}
