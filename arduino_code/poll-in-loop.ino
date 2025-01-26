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

 This example creates a single encoder instance that is updated by
 running the service method directly in the main program loop. This
 must be called frequently enough to avoid missing any pin changes. That
 means several hundred times a second in many cases. If a change is detected
 in the main loop, the current count is sent to the serial port.
 
 You can add a delay into the loop in this eample to see just how often you
 need to call the service method and still get reliable counts.
 */

#include <Arduino.h>
#include <BasicEncoder.h>

const int8_t pinA = 2;
const int8_t pinB = 3;

BasicEncoder encoder(pinA, pinB);

void setup() {
  Serial.begin(115200);
  Serial.println(F("Polling in loop()"));
}

void loop() {
  encoder.service();
  int encoder_change = encoder.get_change();
  if (encoder_change) {
    Serial.println(encoder.get_count());
  } 
  // Even a short delay here will affect performance.
  // Uncomment and change the delay to see what happens.
  //delay(10);
}
