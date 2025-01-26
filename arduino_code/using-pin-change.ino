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
 pin-change interrupts associated with the two pins. If a change is detected
 in the main loop, the current count is sent to the serial port. 
 
 Note that if you enable pin change interrupts for other pins in the same
 group, you may cause conflicts.
 */

#include <Arduino.h>
#include <BasicEncoder.h>

const int8_t pinA = 2;
const int8_t pinB = 3;

BasicEncoder encoder(pinA, pinB);

void pciSetup(byte pin)  // Setup pin change interupt on pin
{
  *digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR |= bit(digitalPinToPCICRbit(pin));                    // clear outstanding interrupt
  PCICR |= bit(digitalPinToPCICRbit(pin));                    // enable interrupt for group
}

void setup_encoders(int a, int b) {
  uint8_t old_sreg = SREG;     // save the current interrupt enable flag
  noInterrupts();
  pciSetup(a);
  pciSetup(b);
  encoder.reset();
  SREG = old_sreg;    // restore the previous interrupt enable flag state
}

ISR(PCINT2_vect)  // pin change interrupt for D0 to D7
{
  encoder.service();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Interrupts");
  setup_encoders(pinA,pinB);
  encoder.set_reverse();
}

void loop() {
  int encoder_change = encoder.get_change();
  if (encoder_change) {
    Serial.println(encoder.get_count());
  } 
}
