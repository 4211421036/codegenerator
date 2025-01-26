/****************************************************************************************************************************
  AsyncWebClientRepeating.ino

  For RP2040W with CYW43439 WiFi

  AsyncHTTPRequest_RP2040W is a library for the RP2040W with CYW43439 WiFi

  Based on and modified from asyncHTTPrequest Library (https://github.com/boblemaire/asyncHTTPrequest)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W

  Copyright (C) <2018>  <Bob Lemaire, IoTaWatt, Inc.>
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
  as published bythe Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *****************************************************************************************************************************/

#include "defines.h"

#define ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN_TARGET      "AsyncHTTPRequest_RP2040W v1.2.2"
#define ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN             1002002

// Uncomment for certain HTTP site to optimize
//#define NOT_SEND_HEADER_AFTER_CONNECTED        true

// Level from 0-4
#define ASYNC_HTTP_DEBUG_PORT     Serial
#define _ASYNC_HTTP_LOGLEVEL_     2

// Select a test server address
const char GET_ServerAddress[] = "arduino.tips";

// GET location
String GET_Location = "/asciilogo.txt";

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include <AsyncHTTPRequest_RP2040W.h>         // https://github.com/khoih-prog/AsyncHTTPRequest_RP2040W

AsyncHTTPRequest request;

int status = WL_IDLE_STATUS;

void sendRequest()
{
  static bool requestOpenResult;

  if (request.readyState() == readyStateUnsent || request.readyState() == readyStateDone)
  {
    requestOpenResult = request.open("GET", (GET_ServerAddress + GET_Location).c_str());

    if (requestOpenResult)
    {
      // Only send() if open() returns true, or crash
      request.send();
    }
    else
    {
      Serial.println("Can't send bad request");
    }
  }
  else
  {
    Serial.println("Can't send request");
  }
}

void requestCB(void *optParm, AsyncHTTPRequest *request, int readyState)
{
  (void) optParm;

  if (readyState == readyStateDone)
  {
    AHTTP_LOGWARN(F("\n**************************************"));
    AHTTP_LOGWARN1(F("Response Code = "), request->responseHTTPString());

    if (request->responseHTTPcode() == 200)
    {
      Serial.println(F("\n**************************************"));
      Serial.println(request->responseText());
      Serial.println(F("**************************************"));
    }
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("Local IP Address: ");
  Serial.println(ip);
}

void setup()
{
  Serial.begin(115200);

  while (!Serial && millis() < 5000);

  Serial.print("\nStart AsyncWebClientRepeating on ");
  Serial.println(BOARD_NAME);
  Serial.println(ASYNCTCP_RP2040W_VERSION);
  Serial.println(ASYNC_HTTP_REQUEST_RP2040W_VERSION);

#if defined(ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN)

  if (ASYNC_HTTP_REQUEST_RP2040W_VERSION_INT < ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN)
  {
    Serial.print("Warning. Must use this example on Version equal or later than : ");
    Serial.println(ASYNC_HTTP_REQUEST_RP2040W_VERSION_MIN_TARGET);
  }

#endif

  ///////////////////////////////////

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");

    // don't continue
    while (true);
  }

  Serial.print(F("Connecting to SSID: "));
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);

  delay(1000);

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED)
  {
    delay(500);

    // Connect to WPA/WPA2 network
    status = WiFi.status();
  }

  printWifiStatus();

  ///////////////////////////////////

  request.setDebug(false);

  request.onReadyStateChange(requestCB);
}

void sendRequestRepeat()
{
  static unsigned long sendRequest_timeout = 0;

#define SEND_REQUEST_INTERVAL     60000L

  // sendRequest every SEND_REQUEST_INTERVAL (60) seconds: we don't need to sendRequest frequently
  if ((millis() > sendRequest_timeout) || (sendRequest_timeout == 0))
  {
    sendRequest();

    sendRequest_timeout = millis() + SEND_REQUEST_INTERVAL;
  }
}

void loop()
{
  sendRequestRepeat();
}
