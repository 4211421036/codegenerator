/*
 * SparkFun WebServer ESP32 W5500 Example1 : serving a web page using ESP32_W5500 and standard Ethernet
 * 
 * This code demonstrates how you can swap between standard Arduino Ethernet and the ESP32_W5500 class.
 * The ESP32_W5500 code uses me-no-dev's ESPAsyncWebServer to serve the page.
 * Standard Ethernet does it manually with client.connected.
 * 
 * ESP32_W5500 can not co-exist with the standard Arduino core for ESP32. The SPI and interrupt drivers fight.
 * To resolve this, the code boots into either ESP32_W5500 mode or standard Ethernet mode.
 * ESP.restart() restarts the code to change mode.
 * The code creates or deletes a file in LittleFS to indicate which mode should be used next time.
 * 
 * Licence: please see LICENSE.md for more details.
 */

#include <Arduino.h>

#include "FS.h"
#include <LittleFS.h> //We use LittleFS to control the Ethernet mode

#include <SPI.h> //Needed for SPI to W5500

#include <Esp.h>

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Pin definitions

int pin_PICO = 23;      //The digital pin for SPI PICO
int pin_POCI = 19;      //The digital pin for SPI POCI
int pin_SCK = 18;       //The digital pin for SPI SCK
int pin_W5500_CS = 27;  //The digital pin for the W5500 Chip Select. Change this if required
int pin_W5500_INT = 33; //The digital pin for the W5500 Interrupt. Change this if required

int pin_POWER_CONTROL = 32; //If your board has a power enable pin, define it here. Set to -1 if not needed.

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// ESP32_W5500 WebServer

#include <SparkFun_WebServer_ESP32_W5500.h> //http://librarymanager/All#SparkFun_WebServer_ESP32_W5500

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// ESPAsyncWebServer

#include <ESPAsyncWebServer.h> //https://github.com/me-no-dev/ESPAsyncWebServer

AsyncWebServer *asyncWebServer;
AsyncWebSocket *asyncWebSocket;

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Arduino Ethernet

#include <Ethernet.h>

EthernetServer *ethernetServer;

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

void setup()
{
  //Set up the W5500 Chip Select
  pinMode(pin_W5500_CS, OUTPUT);
  digitalWrite(pin_W5500_CS, HIGH);

  //Set up the W5500 interrupt pin
  pinMode(pin_W5500_INT, INPUT_PULLUP);
  
  //Check if the board has a power enable pin. Configure it if required
  if (pin_POWER_CONTROL >= 0)
  {
    pinMode(pin_POWER_CONTROL, OUTPUT);
    digitalWrite(pin_POWER_CONTROL, HIGH);
  }
  
  delay(1000);

  Serial.begin(115200);
  Serial.println("SparkFun WebServer ESP32 W5500 Example");

  //Start LittleFS
  if(!LittleFS.begin(true))
  {
    Serial.println("LittleFS mount failed");
    return;
  }

  //Empty the serial buffer
  while (Serial.available())
    Serial.read();

  Serial.println();
  Serial.println("Press any key (send any character) to change modes");
  Serial.println();
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

void loop()
{
  // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //If useArduinoEthernet.txt does not exist in LittleFS, use ESP32_W5500 and ESPAsyncWebServer

  if (!LittleFS.exists("/useArduinoEthernet.txt"))
  {
  
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // ESP32_W5500

    //Create useArduinoEthernet.txt so the code will use Arduino Ethernet next time
    File file = LittleFS.open("/useArduinoEthernet.txt", FILE_WRITE);
    file.close();

    Serial.println("Using ESP32_W5500:");
    Serial.println();

    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Configure the W5500
  
    //Must be called before ETH.begin()
    ESP32_W5500_onEvent();
  
    //Start the ethernet connection
    ETH.begin( pin_POCI, pin_PICO, pin_SCK, pin_W5500_CS, pin_W5500_INT ); //Use default clock speed and SPI Host
  
    ESP32_W5500_waitForConnect();
  
    // Print your local IP address:
    Serial.print("My IP address: ");
    Serial.println(ETH.localIP());
    Serial.println();
  
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Configure the Web Server
  
    Serial.println("Starting Web Server");

    asyncWebServer = new AsyncWebServer(80); //Instantiate the web server. Use port 80
    asyncWebSocket = new AsyncWebSocket("/ws"); //Instantiate the web socket

    asyncWebSocket->onEvent(onWsEvent);
    asyncWebServer->addHandler(asyncWebSocket);

    static char webPage[100];
    snprintf(webPage, sizeof(webPage), "<!DOCTYPE html>\r\n<html>\r\n<h2>I am an ESP32_W5500 web page</h2>\r\n<br>\r\n</html>\r\n");
    
    asyncWebServer->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/html", webPage);
      });
    
    asyncWebServer->onNotFound(notFound);

    asyncWebServer->begin();

    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Loop until the user presses a key

    while(1)
    {
      if (Serial.available()) //Check if the user has pressed a key
      {
        ETH.end(); //End ESP32_W5500. This is _really_ important!
        
        ESP.restart(); //Restart the ESP32
      }
      delay(1);
    }
  }

  else
  {
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Ethernet

    //Delete useArduinoEthernet.txt so the code will use ESP32_W5500 next time
    LittleFS.remove("/useArduinoEthernet.txt");

    Serial.println("Using Arduino Ethernet:");
    Serial.println();

    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Configure the W5500

    Ethernet.init(pin_W5500_CS); //Set the chip select pin

    //Get MAC address
    uint8_t ethernetMACAddress[6];
    esp_read_mac(ethernetMACAddress, ESP_MAC_WIFI_STA);
    ethernetMACAddress[5] += 3; //Convert WiFi MAC address to Ethernet MAC (add 3)
    
    Ethernet.begin(ethernetMACAddress); //Use default mac and IP

    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
      Serial.println("W5500 was not found.  Sorry, can't run without hardware. :(");
      while (true)
      {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }

    // Print your local IP address:
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
    Serial.println();  

    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Configure the Web Server
  
    Serial.println("Starting Web Server");

    ethernetServer = new EthernetServer(80); //Instantiate the web server. Use port 80

    ethernetServer->begin(); //Start the server

    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Loop until the user presses a key

    while(1)
    {
      // listen for incoming clients
      EthernetClient client = ethernetServer->available();
      if (client) {
        Serial.println("New client conected");
        // an http request ends with a blank line
        bool currentLineIsBlank = true;
        while (client.connected())
        {
          if (client.available())
          {
            char c = client.read();
            // if you've gotten to the end of the line (received a newline
            // character) and the line is blank, the http request has ended,
            // so you can send a reply
            if (c == '\n' && currentLineIsBlank) {
              // send a standard http response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              client.println("<!DOCTYPE HTML>");
              client.println("<html>");
              client.println("<h2>I am an Ethernet web page</h2>");
              client.println("</html>");
              break;
            }
            
            if (c == '\n')
            {
              // you're starting a new line
              currentLineIsBlank = true;
            }
            else if (c != '\r')
            {
              // you've gotten a character on the current line
              currentLineIsBlank = false;
            }
          }
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();
        Serial.println("Client disconnected");
      }

      if (Serial.available()) //Check if the user has pressed a key
      {
        ESP.restart(); //Restart the ESP32
      }
    }
  }
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT) {
    Serial.println("Websocket client connected");
  }
  else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Websocket client disconnected");
  }
}
