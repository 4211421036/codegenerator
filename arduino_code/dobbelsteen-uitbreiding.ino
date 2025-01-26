/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Arduino dot-matrix dies
*
* Created by       :        Arnold Velzel
* Created on       :        08-10-2021
*
*******************************************************************************/

// Definities
#define DEBUG_ENABLED           false // Output info via de seriële poort

// Aantal rijen displays (Voor de A-Vision ESP8266 learning board is dat 1)
#define DISPLAY_ROWS            1   
// Aantal kolommen displays (Voor de A-Vision ESP8266 learning board is dat 1)
#define DISPLAY_COLUMNS         1

// Digitale ingang voor de drukknop (Vrije keuze)
#define BUTTON1                 D6

// Includes
#include <avision_debug.h>
#include <avision_display.h>
// https://pjrp.github.io/MDParolaFontEditor -- maak je eigen font
#include "dies_font.h"

// Maak display object
AVision::dotmatrixDisplay display;

void setup()
{
    // https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/
    pinMode(BUTTON1, INPUT_PULLUP);

    initDebug(DEBUG_ENABLED);
    display.init(DISPLAY_ROWS, DISPLAY_COLUMNS, AVISION_DISPLAY);
    display.setFont(_font_die_numbers);

    display.text = "";

    for (size_t i = 0; i < 6; i++)
    {
        display.text = String(i+1);
        display.loop();
        delay(250);
    }
    delay(500);

    display.text = "0";
    display.loop();
}

unsigned long releasetime = 0;
bool rolling = false;
void loop()
{
    unsigned long m = millis();
    // https://www.arduino.cc/reference/en/language/functions/digital-io/digitalread/
    if (digitalRead(BUTTON1) == LOW || rolling) {
        rolling = true;

        if (digitalRead(BUTTON1) != LOW && releasetime == 0) {
            releasetime = m;
        }

        // https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
        display.text = String(random(1,7));
        display.loop();
        if (digitalRead(BUTTON1) == LOW || (m - releasetime) < 5000) {
            // still rolling
            if (digitalRead(BUTTON1) == LOW || (m-releasetime) < 500) {
                delay(50);
            } else {
                delay((m-releasetime)/10);
            }
        } else {
            rolling = false;
            releasetime = 0;
            String die = display.text;
            for (size_t f = 0; f < 10; f++)
            {
                delay(50);
                display.text = " ";
                display.loop();
                delay(50);
                display.text = die;
                display.loop();
            }
            
        }
    }
}
