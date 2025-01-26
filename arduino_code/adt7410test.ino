
/**************************************************************************/
/*!
This is a demo for the Adafruit ADT7410 breakout
----> http://www.adafruit.com/products/4089
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
*/
/**************************************************************************/

#include <Wire.h>
#include "Adafruit_ADT7410.h"

// Create the ADT7410 temperature sensor object
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();

void setup() {
  Serial.begin(115200);
  Serial.println("ADT7410 demo");

  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x49) for example
  if (!tempsensor.begin()) {
    Serial.println("Couldn't find ADT7410!");
    while (1);
  }

  // sensor takes 250 ms to get first readings
  delay(250);

  // ** Optional **
  // Can set ADC resolution
  // ADT7410_13BIT = 13 bits (default)
  // ADT7410_16BIT = 16 bits
  tempsensor.setResolution(ADT7410_16BIT);
  Serial.print("Resolution = ");
  switch (tempsensor.getResolution()) {
    case ADT7410_13BIT:
      Serial.print("13 ");
      break;
    case ADT7410_16BIT:
      Serial.print("16 ");
      break;
    default:
      Serial.print("??");
  }
  Serial.println("bits");
}

void loop() {
  // Read and print out the temperature, then convert to *F
  float c = tempsensor.readTempC();
  float f = c * 9.0 / 5.0 + 32;
  Serial.print("Temp: "); Serial.print(c); Serial.print("*C\t");
  Serial.print(f); Serial.println("*F");

  delay(1000);
}
