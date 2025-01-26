#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>

const int BUZZER_PIN = 7;
const int PULSE_INPUT = A0;
const int PULSE_BLINK = 13;
const int THRESHOLD = 550;
const int RR_MAX_SIZE = 10; // Ukuran maksimum untuk menyimpan RR intervals

PulseSensorPlayground pulseSensor;

unsigned long lastBeatTime = 0;
unsigned long previousBeatTime = 0;
float rrIntervals[RR_MAX_SIZE] = {0}; // Array untuk menyimpan RR intervals
int rrIndex = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);

  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.blinkOnPulse(PULSE_BLINK);
  pulseSensor.setThreshold(THRESHOLD);

  if (!pulseSensor.begin()) {
    while (1); // Tahan dalam kondisi error
  }
}

void loop() {
  if (pulseSensor.sawStartOfBeat()) {
    previousBeatTime = lastBeatTime;
    lastBeatTime = millis();

    float currentRR = lastBeatTime - previousBeatTime;
    rrIntervals[rrIndex] = currentRR;

    rrIndex = (rrIndex + 1) % RR_MAX_SIZE;

    float meanRR = 0;
    for (int i = 0; i < RR_MAX_SIZE; i++) {
      meanRR += rrIntervals[i];
    }
    meanRR /= RR_MAX_SIZE;

    float sdRR = 0;
    for (int i = 0; i < RR_MAX_SIZE; i++) {
      sdRR += (rrIntervals[i] - meanRR) * (rrIntervals[i] - meanRR);
    }
    sdRR = sqrt(sdRR / RR_MAX_SIZE);

    Serial.print("Mean RR: ");
    Serial.print(meanRR);
    Serial.print("ms, SD RR: ");
    Serial.println(sdRR);
  }
}
