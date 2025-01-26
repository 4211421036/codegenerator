/**
 * @file record.ino
 * @author Mechasolution Tech Support Team (techms5499@gmail.com)
 * @brief record example
 * @version 0.1
 * @date 2024-05-02
 *
 * @copyright Copyright (c) 2024 Mechasolution (https://mechasolution.com)
 *
 */

#include <SoftwareSerial.h>

#include <Mecha_VoiceRecognition.h>

SoftwareSerial uart_voice(2, 3);
Mecha_VoiceRecognition voice(&uart_voice);

void setup() {
  Serial.begin(9600);

  voice.setDebugOn(&Serial);
  voice.setGroup(VOICE_GROUP_1);

  if (voice.init() == false) {
    while (true) {
      ;
    }
  }

  if (voice.setMode(VOICE_MODE_RECORDING) == false) {
    while (true) {
      ;
    }
  }
}

void loop() {
}
