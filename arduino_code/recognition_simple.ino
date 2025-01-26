/**
 * @file recognition_simple.ino
 * @author Mechasolution Tech Support Team (techms5499@gmail.com)
 * @brief simple recognition example
 * @version 0.1
 * @date 2024-05-02
 *
 * @copyright Copyright (c) 2024 Mechasolution (https://mechasolution.com)
 *
 */

#include <SoftwareSerial.h>

#include <Mecha_VoiceRecognition.h>

#define PIN_LED_GREEN 4
#define PIN_LED_YELLOW 5
#define PIN_LED_RED 6

SoftwareSerial uart_voice(2, 3);
Mecha_VoiceRecognition voice(&uart_voice);

void setup() {
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  Serial.begin(9600);

  voice.setDebugOn(&Serial);
  voice.setGroup(VOICE_GROUP_1);

  if (voice.init() == false) {
    while (true) {
      ;
    }
  }

  if (voice.setMode(VOICE_MODE_RECOGNITION) == false) {
    while (true) {
      ;
    }
  }
}

void loop() {
  VOICE_InstructionTypeDef response = voice.getInstruction();
  switch (response) {
  case VOICE_INSTRUCTION_1:
    Serial.println("Red");
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_RED, HIGH);
    break;
  case VOICE_INSTRUCTION_2:
    Serial.println("Yellow");
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_YELLOW, HIGH);
    digitalWrite(PIN_LED_RED, LOW);
    break;
  case VOICE_INSTRUCTION_3:
    Serial.println("Green");
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    break;
  case VOICE_INSTRUCTION_4:
    Serial.println("White");
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_YELLOW, HIGH);
    digitalWrite(PIN_LED_RED, HIGH);
    break;
  case VOICE_INSTRUCTION_5:
    Serial.println("Black");
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    break;
  case VOICE_INSTRUCTION_ERROR:
    Serial.println("error");
    break;
  case VOICE_INSTRUCTION_NOTHING:
    // no data recieved
    break;
  }
}
