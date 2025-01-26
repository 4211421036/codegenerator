#include <wire_asukiaaa.h>

#define DEVICE_ADDRESS 0x61

const uint16_t buffLen = 10;
uint8_t buffs[buffLen];

void setup() {
  for (int i = 0; i < buffLen; ++i) {
    buffs[i] = 0;
  }
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  int readResult = wire_asukiaaa::readBytes(&Wire, DEVICE_ADDRESS, 0, buffs, buffLen);
  if (readResult != 0) {
    Serial.println("Failed to read because of error " + String(readResult));
  } else {
    Serial.println("Succeeded in reading");
    Serial.print("Received:");
    for (int i = 0; i < buffLen; ++i) {
      Serial.print(" ");
      Serial.print(buffs[i]);
    }
    Serial.println("");
  }

  buffs[0] = millis() / 1000 % 0xff;
  int writeResult = wire_asukiaaa::writeBytes(&Wire, DEVICE_ADDRESS, 0, buffs, buffLen);
  if (writeResult != 0) {
    Serial.println("Failed to write because of error " + String(writeResult));
  } else {
    Serial.println("Succeeded in writing");
  }

  Serial.println("at " + String(millis()));
  Serial.println("");
  delay(500);
}
