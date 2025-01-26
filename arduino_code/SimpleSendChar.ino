#include "SoftwareSerialTX.h"

// use a SoftwareSerialTX on Pin 2
SoftwareSerialTX txOnly(2);

void setup() {
  // setup baurdrate
  txOnly.begin(9600);
}

void loop() {
  // send a char
  txOnly.write('.');
  delay(100);
}
