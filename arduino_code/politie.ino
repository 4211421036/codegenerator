/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Politie zwaailicht en sirene
*
* Created by       :        Arnold Velzel
* Created on       :        22-10-2021
*
*******************************************************************************/

// Definities
#define DEBUG_ENABLED false // Output info via de seriële poort

// Definieer op welke Digitale ingang de drukknoppen zijn aangesloten
#define BUTTON1 D5
#define BUTTON2 D6
#define BUTTON3 D7
#define BUTTON4 D0

#define SIREN   D8

#include <avision_debug.h>
#include <avision_RGBleds.h>

// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.h
AVision::RGBleds leds;
// LED posities -
//          [0] [1] [2] [3]
//          [4] [5] [6] [7]

// Kleuren -> Black (off), Rood, Blauw
uint32_t off;
uint32_t red;
uint32_t blue;

void leds_red(bool on)
{
    if (on)
    {
        leds.setLED(0, red);
        leds.setLED(1, red);
        leds.setLED(4, red);
        leds.setLED(5, red);
    }
    else
    {
        leds.setLED(0, off);
        leds.setLED(1, off);
        leds.setLED(4, off);
        leds.setLED(5, off);
    }
}
void leds_blue(bool on)
{
    if (on)
    {
        leds.setLED(2, blue);
        leds.setLED(3, blue);
        leds.setLED(6, blue);
        leds.setLED(7, blue);
    }
    else
    {
        leds.setLED(2, off);
        leds.setLED(3, off);
        leds.setLED(6, off);
        leds.setLED(7, off);
    }
}

void setup()
{
    // https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    pinMode(BUTTON3, INPUT_PULLUP);
    pinMode(BUTTON4, INPUT_PULLUP);
    pinMode(SIREN, OUTPUT);

    initDebug(DEBUG_ENABLED);
    // Aantal RGB LEDs (Voor de A-Vision ESP8266 learning board is dat 8)
    leds.setLEDcount(8);
    // LED type (WS2812 => NEO_GRB) (Voor de A-Vision ESP8266 learning board is dat NEO_GRB)
    leds.setLEDtype(NEO_GRBW);
    // LEDs Data in (Din) pin (Voor de A-Vision ESP8266 learning board is dat D3)
    leds.init(D3);

    // Waarden -> 0 is uit, 255 is maximaal
    off = leds.color(0, 0, 0, 0);
    red = leds.color(255, 0, 0, 0);
    blue = leds.color(0, 0, 255, 0);

    leds_red(true);
    leds_blue(true);

    // Licht helderheid in procenten 0% - 100%
    leds.setBrightness(100);
    leds.loop();

    delay(100);
    leds_red(false);
    leds_blue(false);
    leds.loop();
}

void loop()
{
    // https://www.arduino.cc/reference/en/language/functions/time/millis/
    unsigned long m = millis();

    // https://www.arduino.cc/reference/en/language/functions/digital-io/digitalread/
    if (digitalRead(BUTTON1) == LOW)
    {
        if (m % 750 < 375) {
            leds_red(true);
            leds_blue(false);
        } else {
            leds_red(false);
            leds_blue(true);
        }
    }
    if (digitalRead(BUTTON2) == LOW)
    {
        if (m % 1500 < 750) {
            tone(SIREN, 375, 750);
        } else {
            tone(SIREN, 500, 750);
        }
    }
    if (digitalRead(BUTTON3) == LOW)
    {
        if (m % 1500 < 750) {
            if (m % 150 < 75) {
                leds_red(true);
            } else {
                leds_red(false);
            }
            leds_blue(false);
        } else {
            leds_red(false);
            if (m % 150 < 75) {
                leds_blue(true);
            } else {
                leds_blue(false);
            }
        }
    }
    if (digitalRead(BUTTON4) == LOW)
    {
        double d = m % 1500;
        double f = 62.5 * (sin(PI * d / 750) + 1.0) + 375;
        tone(SIREN, f, 10);
    }

    if (digitalRead(BUTTON1) == HIGH && digitalRead(BUTTON3) == HIGH) {
        leds_red(false);
        leds_blue(false);
    }
    if (digitalRead(BUTTON2) == HIGH && digitalRead(BUTTON4) == HIGH) {
        noTone(SIREN);
    }

    leds.loop();
    delay(5);
}
