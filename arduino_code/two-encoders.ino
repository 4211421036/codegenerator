/*
  Copyright 2021 Peter Harrison - Helicron
 
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
 
       https://www.apache.org/licenses/LICENSE-2.0
 
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
  This example creates two independent encoders and samples them using a 
  timer interrupt. If either changes, the current count for both is sent to
  the serial port.
 */

#include <Arduino.h>
#include <BasicEncoder.h>
#include <TimerOne.h>

BasicEncoder encoderA(2, 3);
BasicEncoder encoderB(10, 11);


/****************************************************************/
void timer_service() {
  encoderA.service();
  encoderB.service();
}

void setup() {
  Serial.begin(115200);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timer_service);
}

void loop() {
  int encoder_a_change = encoderA.get_change();
  int encoder_b_change = encoderB.get_change();
  if (encoder_a_change || encoder_b_change) {
    Serial.print(encoderA.get_count());
    Serial.print(' ');
    Serial.print(encoderB.get_count());
    Serial.println();
  }
}