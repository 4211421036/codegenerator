/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Arduino WS2812 RGB kleuren LED combineren test
*
* Created by       :        Arnold Velzel
* Created on       :        08-10-2021
*
*******************************************************************************/

// Definities
#define DEBUG_ENABLED           false // Output info via de seriële poort

// Definieer op welke Digitale ingang de drukknoppen zijn aangesloten
#define BUTTON1                 D5
#define BUTTON2                 D6
#define BUTTON3                 D7
#define BUTTON4                 D0

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
    // https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    pinMode(BUTTON3, INPUT_PULLUP);
    pinMode(BUTTON4, INPUT_PULLUP);

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

    // Licht helderheid in procenten 0% - 100%
    leds.setBrightness(100);
    leds.loop();
}

void loop()
{
    uint32_t combined = leds.color(0, 0, 0, 0); // All off

    // https://www.arduino.cc/reference/en/language/functions/digital-io/digitalread/
    if (digitalRead(BUTTON1) == LOW) {
        combined += red;
    }
    if (digitalRead(BUTTON2) == LOW) {
        combined += green;
    }
    if (digitalRead(BUTTON3) == LOW) {
        combined += blue;
    }
    if (digitalRead(BUTTON4) == LOW) {
        combined += white;
    }

    leds.setLED(0, red);
    leds.setLED(1, green);
    leds.setLED(2, blue);
    leds.setLED(3, white);

    leds.setLED(4, combined);
    leds.setLED(5, combined);
    leds.setLED(6, combined);
    leds.setLED(7, combined);

    leds.loop();

    delay(50);
}
