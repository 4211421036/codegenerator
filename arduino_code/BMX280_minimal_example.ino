/* Forced-BMX280 Library
   soylentOrange - https://github.com/soylentOrange/Forced-BMX280

   forked from:
   Forced-BME280 Library
   Jochem van Kranenburg - jochemvk.duckdns.org - 9 March 2020
*/

#include <Wire.h>          // https://www.arduino.cc/reference/en/language/functions/communication/wire/
#include "forcedBMX280.h"  // https://github.com/soylentOrange/Forced-BMX280

/* Test-ino for BMX280 library
   Uses the minmal version of the library (int32_t result of temperature)
   Uses an Arduino Nano and a BME280/BMP280 sensor connected to Pin A4/A5
   Note to myself: yellow wire is SDA, connected to pin A4
                   green wire is SCL, connected to pin A5
   Components are 5V tolerant (using an on-bord LDO)
    - https://www.amazon.de/dp/B0B1HXTS29
    - https://www.amazon.de/dp/B0B1DWSKF5
*/

// BMX280-sensor
ForcedBMX280 climateSensor = ForcedBMX280();
int32_t g_temperature;  // current temperature - value of 1234 would be 12.34 °C

// UART control interface
#define SERIAL_BAUD 9600
static const char dashLine[] = "=======================================";

void setup() {
  // set LED as output and turn it off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // initialize serial pport
  Serial.begin(SERIAL_BAUD);

  // Wait for serial port to settle
  while (!Serial) {
    delay(10);
  }

  // start I2C and BME sensor
  Wire.begin();
  while (climateSensor.begin()) {
    Serial.println("Waiting for sensor...");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(1000);
  }

  // Turn LED on if everything is fine
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(dashLine);
  Serial.println("BMX280 ready");
  Serial.print("\tChipID: 0x");
  Serial.println(climateSensor.getChipID(), HEX);
  Serial.println(dashLine);
}

void loop() {
  // get Temperature
  delay(2000);
  climateSensor.takeForcedMeasurement();
  g_temperature = climateSensor.getTemperatureCelsius();
  Serial.println(" ");
  Serial.print("Temperature: ");
  Serial.print(g_temperature/100);
  Serial.print(".");
  Serial.print(g_temperature%100);
  Serial.println(" °C");
}
