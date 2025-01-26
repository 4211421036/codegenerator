/*
*	BlinkWithMyDelay_Function
*
*	This is an example of how to blink an LED using the MyDelay library by calling a function
*	This allows you to execute code without the use of the delay() function
*	and without having to setup many variables for each time you want to
*	run code in a delay-less manner
*	Written by Marshall G Gates
*	Based off of the Blink Without Delay example http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
*	7/10/15

*/

#include<MyDelay.h>

void ledBlink();//Must declare function before noDelay, function can not take arguments

MyDelay LEDtime(1000, ledBlink, 20 );//Creats a MyDelay varible set to 1000ms, will call ledBlink function 20 times
int LEDpin = 13;
int ledState = LOW;

void setup() {
  pinMode(LEDpin, OUTPUT);
  LEDtime.start();
}

void loop() {
    LEDtime.update();//will check if set time has past and if so will run set function
}

void ledBlink()
{
	// if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(LEDpin, ledState);
}
