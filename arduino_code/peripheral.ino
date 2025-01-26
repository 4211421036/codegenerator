#include <wire_asukiaaa.h>

#define DEVICE_ADDRESS 0x61
#define BUFF_LEN 10

wire_asukiaaa::PeripheralHandler wirePeri(&Wire, BUFF_LEN);

// // It can prohibit writing for register like this
// bool prohibitWriting(int index) {
//   return index == BUFF_LEN - 1;
// }
// wire_asukiaaa::PeripheralHandler wirePeri(&Wire, BUFF_LEN, prohibitWriting);

unsigned long handledReceivedAt = 0;

void setup() {
  Wire.onReceive([](int v) { wirePeri.onReceive(v); });
  Wire.onRequest([]() { wirePeri.onRequest(); });
  Wire.begin(DEVICE_ADDRESS);
  Serial.begin(9600);
  Serial.println("Start");
}

void loop() {
  if (wirePeri.receivedAt != handledReceivedAt) {
    handledReceivedAt = wirePeri.receivedAt;
    for (int i = 0; i < wirePeri.buffLen; ++i) {
      Serial.print(wirePeri.buffs[i]);
      Serial.print(" ");
    }
    Serial.println();
    Serial.println("receivedAt: " + String(wirePeri.receivedAt));
  }
  wirePeri.buffs[wirePeri.buffLen - 1] = millis() / 1000 % 0xff;
  delay(1);
}
