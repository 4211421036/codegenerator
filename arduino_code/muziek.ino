/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        A-Vision ESP8266 dot-matrix alphabet
*
* Created by       :        Arnold Velzel
* Created on       :        08-10-2021
*
*******************************************************************************/

// Definities
#define DEBUG_ENABLED           false // Output info via de seriële poort

// Op welke pin wordt de BUZZER aangesloten (Voor de A-Vision ESP8266 learning board is dat D0)
#define BEEP_BUZZER             D0

#include <avision_debug.h>
#include <avision_beep.h>
// Laad de melodie, toonhoogte en duur
#include "melody.h"

void setup()
{
    initDebug(DEBUG_ENABLED);
    initBeep(BEEP_BUZZER);
}

void loop()
{
    // Speel de melodie
    int noteCount = sizeof(melody) / sizeof(melody[0]);
    for (int note = 0; note < noteCount; note+=2) {
        int noteDuration = 1000 / melody[note+1];
        beep(melody[note], noteDuration, noteDuration+5);
    }

    // Even 10 seconden wachten
    delay(10000);    
}
