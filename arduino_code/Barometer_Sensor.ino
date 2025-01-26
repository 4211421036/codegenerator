/*
    Barometer_Sensor.ino
    Example sketch for barometer

    Copyright (c) 2012 seeed technology inc.
    Website    : www.seeed.cc
    Author     : Jim Lindblom, LG
    Create Time:
    Change Log :

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/
#include "BMP085.h"
#include <Wire.h>
float temperature;
float pressure;
float atm;
float altitude;
BMP085 myBarometer;
void setup() {
    Serial.begin(9600);
    while (!Serial);
    myBarometer.init();

}

void loop() {
    temperature = myBarometer.bmp085GetTemperature(
                      myBarometer.bmp085ReadUT()); //Get the temperature, bmp085ReadUT MUST be called first
    pressure = myBarometer.bmp085GetPressure(myBarometer.bmp085ReadUP());//Get the temperature

    /*
        To specify a more accurate altitude, enter the correct mean sea level
        pressure level.  For example, if the current pressure level is 1019.00 hPa
        enter 101900 since we include two decimal places in the integer value。
    */
    altitude = myBarometer.calcAltitude(101900);

    atm = pressure / 101325;

    Serial.print("Temperature: ");
    Serial.print(temperature, 2); //display 2 decimal places
    Serial.println(" Celsius");

    Serial.print("Pressure: ");
    Serial.print(pressure, 0); //whole number only.
    Serial.println(" Pa");

    Serial.print("Ralated Atmosphere: ");
    Serial.println(atm, 4); //display 4 decimal places

    Serial.print("Altitude: ");
    Serial.print(altitude, 2); //display 2 decimal places
    Serial.println(" m");

    Serial.println();

    delay(1000); //wait a second and get values again.
}



