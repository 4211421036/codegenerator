/*

	Transform library: IntSignal usage example
	Copyright (C) 2023 Lucio Rossi

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
  Some basic operations with signals
*/

#include "Transform.h"

const uint16_t samples = 8;
const uint16_t signal_max = 32000;

int32_t real[samples];
int32_t imag[samples];
Transform transformer;

void setup() {
	Serial.begin(115200);
	transformer.debug(Serial);
}

void loop() {
	
	for (uint16_t i=0; i<samples; i++) {
		real[i] = i;
		imag[i] = 0;
	}
	
	IntSignal signal(real, imag, samples);
  
	transformer.printSignal(signal);
	
	Serial.println("Reverse-bit representation");
	transformer.InverseBit(signal.real, samples);
	transformer.printSignal(signal);

	delay(9999);
}