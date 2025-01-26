/*

	Transform library: FFT transform of mic signal
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

#include <PDM.h>
#include "Transform.h"

#define SAMPLES 1024

// default number of output channels
static const char channels = 1;

// default PCM output frequency
static const int frequency = 16000;

// Buffer to read samples into, each sample is 16-bits
short sampleBuffer[SAMPLES];

int32_t real[SAMPLES];
int32_t imag[SAMPLES];
uint32_t module[SAMPLES>>1];

Transform transformer;

// Number of audio samples read
int samplesRead = 0;
volatile int samplesAvailable;

void zero(int32_t* v, uint16_t size) {
    for (uint16_t i=0; i<size; i++) {
        v[i] = 0;
    }
}

void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Ready");
      // Configure the data receive callback
    PDM.onReceive(onPDMdata);

    // Optionally set the gain
    // Defaults to 20 on the BLE Sense and 24 on the Portenta Vision Shield
    // PDM.setGain(30);

    PDM.setBufferSize(SAMPLES<<1);

    // Initialize PDM with:
    // - one channel (mono mode)
    // - a 16 kHz sample rate for the Arduino Nano 33 BLE Sense
    // - a 32 kHz or 64 kHz sample rate for the Arduino Portenta Vision Shield
    if (!PDM.begin(channels, frequency)) {
        Serial.println("Failed to start PDM!");
        while (1);
    }
}

void loop() {



  if (samplesRead == SAMPLES) {

    // do the FFT and extract the peak
    zero(imag, SAMPLES);
    IntSignal signal(real, imag, SAMPLES);
    transformer.FFT(signal);
    signal.getSignalModule(module);
    float peak_position = getApproxMaxPosition(module, SAMPLES>>1);
    float peak_frequency = (peak_position*frequency)/SAMPLES;

    // print the peak value on Serial
    Serial.println(peak_frequency);
    
    // Clear the read count
    samplesRead = 0;
  } else if (samplesAvailable) {
    // append to real
    int j=0;
    for (int i=0; i<samplesAvailable; i++) {
      real[samplesRead+i] = sampleBuffer[i];
      j++;
      if (samplesRead+j==SAMPLES){break;}
    }
    
    samplesRead += j;
    samplesAvailable = 0;
  }

}

/**
 * Callback function to process the data from the PDM microphone.
 * NOTE: This callback is executed as part of an ISR.
 * Therefore using `Serial` to print messages inside this function isn't supported.
 * */
void onPDMdata() {
  // Query the number of available bytes
  int bytesAvailable = PDM.available();

  // Read into the sample buffer only if we have enogh bytes (2*SAMPLES)
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesAvailable = bytesAvailable / 2;

}