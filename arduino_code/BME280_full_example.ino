/* Forced-BMX280 Library
   soylentOrange - https://github.com/soylentOrange/Forced-BMX280

   forked from:
   Forced-BME280 Library
   Jochem van Kranenburg - jochemvk.duckdns.org - 9 March 2020
*/

#include <everytime.h>     // https://github.com/kfessel/everytime
#include <Wire.h>          // https://www.arduino.cc/reference/en/language/functions/communication/wire/
#include "forcedBMX280.h"  // https://github.com/soylentOrange/Forced-BMX280

/* Test-ino for BMX280 library
   Uses the full version of the library (integer and float result of temperature, pressure and humidity)
   Uses an Arduino Nano and a BME280 sensor connected to Pin A4/A5
   Note to myself: yellow wire is SDA, connected to pin A4
                   green wire is SCL, connected to pin A5
   Components are 5V tolerant (using an on-bord LDO)
    - https://www.amazon.de/dp/B0B1HXTS29
    - https://www.amazon.de/dp/B0B1DWSKF5
*/

// BMX280-sensor
#define BMX_CYCLE 2000  // frequency for BMX280 readout (in ms)

// // uncomment this block to use the minimum version of the sensor
// ForcedBMX280 climateSensor = ForcedBMX280();
// int32_t g_temperature;  // current temperature - value of 1234 would be 12.34 °C

// // uncomment this block to get temperature as integer and float
// ForcedBMX280Float climateSensor = ForcedBMX280Float();
// float g_temperatureFloat;    // current temperature

// uncomment this block to get temperature and pressure as integer
// ForcedBMP280 climateSensor = ForcedBMP280();
// int32_t g_temperature;  // current temperature
// uint32_t g_pressure;    // current pressure

// // uncomment this block to get temperature and pressure as integer and float
// ForcedBMP280Float climateSensor = ForcedBMP280Float();
// int32_t g_temperature;        // current temperature
// uint32_t g_pressure;          // current pressure
// float g_temperatureFloat;     // current temperature
// float g_pressureFloat;        // current pressure

// // uncomment this block to get temperature, pressure and humidity as integer
// ForcedBME280 climateSensor = ForcedBME280();
// int32_t g_temperature;  // current temperature
// uint32_t g_pressure;    // current pressure
// uint32_t g_humidity;    // current humidity

// uncomment this block to get temperature, pressure and humidity as integer and float
ForcedBME280Float climateSensor = ForcedBME280Float();
int32_t g_temperature;  // current temperature
uint32_t g_pressure;    // current pressure
uint32_t g_humidity;    // current humidity
float g_temperatureFloat;    // current temperature
float g_pressureFloat;       // current pressure
float g_humidityFloat;       // current humidity


// UART control interface
#define SERIAL_BAUD 115200
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
  every(BMX_CYCLE) {
    climateSensor.takeForcedMeasurement();
    Serial.println(" ");

    // // uncomment this block to print temperature - int32_t
    // g_temperature = climateSensor.getTemperatureCelsius();
    // Serial.print("Temperature: ");
    // Serial.print(g_temperature/100);
    // Serial.print(".");
    // Serial.print(g_temperature%100);
    // Serial.println(" °C");

    // uncomment this block to print temperature - float
    g_temperatureFloat = climateSensor.getTemperatureCelsiusAsFloat(true);
    Serial.print("Temperature: ");
    Serial.print(g_temperatureFloat);
    Serial.println(" °C");

    // // uncomment this block to print pressure - uint32_t
    // g_pressure = climateSensor.getPressure();
    // Serial.print("Pressure: ");
    // Serial.print(g_pressure/100);
    // Serial.print(".");
    // Serial.print(g_pressure%100);
    // Serial.println(" hPa");

    // uncomment this block to print pressure - float
    g_pressureFloat = climateSensor.getPressureAsFloat();
    Serial.print("Pressure: ");
    Serial.print(g_pressureFloat);
    Serial.println(" hPa");

    // // uncomment this block to print humidity - uint32_t
    // g_humidity = climateSensor.getRelativeHumidity();
    // Serial.print("Humidity: ");
    // Serial.print(g_humidity/100);
    // Serial.print(".");
    // Serial.print(g_humidity%100);
    // Serial.println(" %rh");

    // uncomment this block to print humidity - float
    g_humidityFloat = climateSensor.getRelativeHumidityAsFloat();
    Serial.print("Humidity: ");
    Serial.print(g_humidityFloat);
    Serial.println(" %rh");
  }
}
