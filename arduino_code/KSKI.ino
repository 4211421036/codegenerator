/*
Tanggal 2 November 2024
KSKI Belajar Bareng Pembuatan Module :Journey 1

*/

#include <Arduino.h>
#include "LedContoller.h" 

LedContoller Led(14, 3.3); //Module Controler LED dengan pin 14 dan arusnya 3.3


void setup() {
  Serial.begin(9800);
  Led.begin();
  Serial.println("Hello, LED!");
}

void loop() {
  Led.turnOn();
  delay(1000);
  Led.turnOff();
  delay(1000);
  Led.blink(500, 5);
}
