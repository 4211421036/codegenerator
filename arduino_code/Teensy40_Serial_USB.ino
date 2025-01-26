/****************************************************************************************************************************
 * Teensy40_Serial_USB.ino
 * For Teensy 4.0 using ESP8266 WiFi Shield
 *
 * Forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
 * Built by Khoi Hoang https://github.com/khoih-prog/Blynk_Teensy
 * Licensed under MIT license
 *****************************************************************************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT HWSERIAL

#define HWSERIAL        Serial1     // RX1: pin 0, TX1 : pin 1 on Tennsy 4.0
#define BLYNK_SERIAL    Serial      // USB port

#include <BlynkSimpleStream.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "your-token";

void setup()
{
  // Debug console if connected to HWSERIAL (Serial1 of Teensy 4.0)
  HWSERIAL.begin(115200);
  delay(2000);
  HWSERIAL.println("\nStart Teensy4");
  
  // Blynk will work through Serial <-> USB (/dev/ttyACM0)
  // Do not read or write this BLYNK_SERIAL manually in your sketch
  BLYNK_SERIAL.begin(115200);
  HWSERIAL.println("Start Blynk");
  delay(5000);
  Blynk.begin(BLYNK_SERIAL, auth);

  if (Blynk.connected())
  {
    HWSERIAL.println("Blynk connected");
  }
  else
  {
    HWSERIAL.println("Blynk not connected");
  }  
}

void loop()
{
  Blynk.run();
}
