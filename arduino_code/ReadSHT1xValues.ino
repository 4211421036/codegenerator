/**
 * ReadSHT1xValues
 *
 * Read temperature and humidity values from an SHT1x-series (SHT10,
 * SHT11, SHT15) sensor.
 *
 * Copyright 2009 Jonathan Oxer <jon@oxer.com.au>
 * www.practicalarduino.com
 */

#include <Arduino.h>

#include <SHT1x-ESP.h>

// Specify data and clock connections and instantiate SHT1x object
#define dataPin  10
#define clockPin 13

// default to 5.0v boards, e.g. Arduino UNO
SHT1x sht1x(dataPin, clockPin);

// if 3.3v board is used
//SHT1x sht1x(dataPin, clockPin, SHT1x::Voltage::DC_3_3v);

void setup()
{
  Serial.begin(9600); // Open serial connection to report values to host
  Serial.println("Starting up");
}

void loop()
{
  float temp_c;
  float temp_f;
  float humidity;

  // Read values from the sensor
  temp_c = sht1x.readTemperatureC();
  temp_f = sht1x.readTemperatureF();
  humidity = sht1x.readHumidity();

  // Print the values to the serial port
  Serial.print("Temperature: ");
  Serial.print(temp_c, DEC);
  Serial.print("C / ");
  Serial.print(temp_f, DEC);
  Serial.print("F. Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  delay(2000);
}
