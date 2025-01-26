/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Arduino WS2812 RGB LED helderheid test
*
* Created by       :        Arnold Velzel
* Created on       :        08-10-2021
*
*******************************************************************************/

// Definities
#define DEBUG_ENABLED           false // Output info via de seriële poort

// Analoge ingang voor het lezen van de draaiknop (potentiometer)
// De ESP8266 heeft slechts één analoge ingang A0
#define ANALOG_INPUT            A0
// De maximale waarde van de nalage ingang uitlezen (10bit => 1023)
#define MAX_ANALOG_VALUE        1023

#include <avision_debug.h>
#include <avision_RGBleds.h>

// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.h
AVision::RGBleds leds;
// LED posities -
//          [0] [1] [2] [3]
//          [4] [5] [6] [7]

// Kleuren -> Rood, Groen, Blauw, Wit
uint32_t red;
uint32_t green;
uint32_t blue;
uint32_t white;
uint32_t bright_white;

void setup()
{
    initDebug(DEBUG_ENABLED);
    // Aantal RGB LEDs (Voor de A-Vision ESP8266 learning board is dat 8)
    leds.setLEDcount(8);
    // LED type (WS2812 => NEO_GRB) (Voor de A-Vision ESP8266 learning board is dat NEO_GRB)
    leds.setLEDtype(NEO_GRB);
    // LEDs Data in (Din) pin (Voor de A-Vision ESP8266 learning board is dat D3)
    leds.init(D3);

    // Waarden -> 0 is uit, 255 is maximaal
    red    = leds.color(128, 0, 0, 0);
    green  = leds.color(0, 128, 0, 0);
    blue   = leds.color(0, 0, 128, 0);
    white  = leds.color(0, 0, 0, 128);
    bright_white = leds.color(255, 255, 255, 255);

    leds.setLED(0, red);
    leds.setLED(1, green);
    leds.setLED(2, blue);
    leds.setLED(3, white);

    leds.setLED(4, bright_white);
    leds.setLED(5, bright_white);
    leds.setLED(6, bright_white);
    leds.setLED(7, bright_white);

    // Licht helderheid in procenten 0% - 100%
    leds.setBrightness(100);
}

void loop()
{
    // https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/
    int level = 100 * analogRead(ANALOG_INPUT) / MAX_ANALOG_VALUE;
    leds.setBrightness(level);
    leds.loop();
}
