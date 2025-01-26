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

// Aantal rijen displays (Voor de A-Vision ESP8266 learning board is dat 1)
#define DISPLAY_ROWS            1   
// Aantal kolommen displays (Voor de A-Vision ESP8266 learning board is dat 1)
#define DISPLAY_COLUMNS         1   

// Includes
#include <avision_debug.h>
#include <avision_display.h>

// Maak display object
AVision::dotmatrixDisplay display;

void setup()
{
    initDebug(DEBUG_ENABLED);
    display.init(DISPLAY_ROWS, DISPLAY_COLUMNS, AVISION_DISPLAY);
    display.text = "";
}

void loop()
{
    char *alfabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz ";
    for (size_t a = 0; a < strlen(alfabet); a++)
    {
        display.text = " " + String(alfabet[a]);
        display.loop();
        delay(500);
    }    
}
