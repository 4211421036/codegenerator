// main.ino
#include <Wire.h>
#include "module.h"

void setup() {
  Serial.begin(9600);
}

void loop() {
  float suhu = 25.0; // Contoh suhu
  tampilkanSuhu(suhu);
  delay(1000);
}