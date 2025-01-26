#include <wire_asukiaaa.h>
#define TARGET_DEVICE_ADDRESS 0x08
#define TARGET_WRITE_REGISTER_ADDRESS 0x00
#define TARGET_READ_REGISTER_ADDRESS 0x03

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  uint8_t dataToWrite[] = {0, 1, 2};
  uint8_t writeLen = 3;
  int writeResult = wire_asukiaaa::writeBytes(&Wire, TARGET_DEVICE_ADDRESS, TARGET_WRITE_REGISTER_ADDRESS, dataToWrite, writeLen);
  if (writeResult != 0) {
    Serial.println("Failed to write because of error " + String(writeResult));
  } else {
    Serial.println("Succeeded in writing");
  }

  uint8_t readLen = 3;
  uint8_t dataThatRead[readLen];
  int readResult = wire_asukiaaa::readBytes(&Wire, TARGET_DEVICE_ADDRESS, TARGET_READ_REGISTER_ADDRESS, dataThatRead, readLen);
  if (readResult != 0) {
    Serial.println("Failed to read because of error " + String(readResult));
  } else {
    Serial.println("Succeeded in reading");
    Serial.print("Received:");
    for (int i = 0; i < readLen; ++i) {
      Serial.print(" ");
      Serial.print(dataThatRead[i]);
    }
    Serial.println("");
  }
  Serial.println("at " + String(millis()));
  Serial.println("");
  delay(500);
}
