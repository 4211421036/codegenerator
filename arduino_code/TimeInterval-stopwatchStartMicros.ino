/*
Author: ESDeveloperBR
email: esdeveloperbr@gmail.com
Library Download: https://github.com/ESDeveloperBR/TimeInterval
    
Objective:
- Use the "TimeInterval" class to create a stopwatch to identify how long it takes to execute some "Serial.print()" commands.

Objetivo:
- Utilizar a classe "TimeInterval" para criar um cronometro para identificar quanto tempo leva para executar alguns comandos "Serial.print()".
*/

#include <Arduino.h>
#include <TimeInterval.h>

TimeInterval intervalTest;

// <<<<<<<<<<<<<<<<<<<<<<<<<< Setup >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void setup() {
  Serial.begin(115200);
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<< Loop >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void loop() {
  /*
    void TimeInterval::stopwatchStartMicros()
    Initializes the stopwatch in microseconds
  */
  intervalTest.stopwatchStartMicros();

  Serial.println("*************");
  Serial.println("Hello World");
  Serial.println("*************");

  /*
    long TimeInterval::stopwatchStopMicros()
    Stop stopwatch - Microseconds and return the obtained value  
  */
  long returnStopwatch = intervalTest.stopwatchStopMicros();

  Serial.print("Execution of all previous println commands takes ");
  Serial.print(returnStopwatch);
  Serial.println(" microsconds.");
  Serial.println("");
  delay(5000);
}