#include <A89306_asukiaaa.hpp>

A89306_asukiaaa::Core blmd;

void setup() {
  Serial.begin(115200);
  blmd.begin();
}

void loop() {
  namespace Registers = A89306_asukiaaa::Registers;
  static Registers::EEPROM8 e8;
  static Registers::EEPROM17 e17;
  static Registers::EEPROM25 e25;

  Serial.println("print info of shadow registers");
  blmd.printInfo(&Serial);
  Serial.println("at " + String(millis()));
  delay(5000);

  Serial.println("toggle brake by i2c and enable brake");
  e25.brkFromReg = true;
  e25.brkInput = true;
  blmd.writeRegister(e25);

  Serial.println("configure speed by i2c");
  e17.i2cSpeedMode = true;
  blmd.writeRegister(e17);

  Serial.println("change max speed to 150Hz");
  e8.setRegisterByRatedSpeedHz(183);
  blmd.writeRegister(e8);

  Serial.println("disable brake");
  e25.brkInput = false;
  blmd.writeRegister(e25);
  delay(5000);

  Serial.println("enable brake");
  e25.brkInput = true;
  blmd.writeRegister(e25);
  delay(2000);
}
