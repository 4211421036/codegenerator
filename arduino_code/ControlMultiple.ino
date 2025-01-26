#include <BLVD20KM_asukiaaa.h>
#include <rs485_asukiaaa.h>

#define RS485_DE 4
// #define RS485_RE RS485_DE // you can unify DE and RE
#define RS485_RE 5
#define RS485_BAUDRATE 115200
#define RS485_SERIAL Serial1
#define MOTOR_A_ADDRESS 1
#define MOTOR_B_ADDRESS 2

rs485_asukiaaa::ModbusRtu::Central modbus(&RS485_SERIAL, RS485_DE, RS485_RE);
BLVD20KM_asukiaaa motorA(&modbus, MOTOR_A_ADDRESS);
BLVD20KM_asukiaaa motorB(&modbus, MOTOR_B_ADDRESS);

void setup() {
  Serial.begin(115200);
  BLVD20KM_asukiaaa::beginModbus(&modbus, RS485_BAUDRATE);
  motorA.beginWithoutModbus();
  motorB.beginWithoutModbus();
}

void readAndPrintSpeed(String motorLabel, BLVD20KM_asukiaaa* motor) {
  uint16_t speed;
  auto result = motor->readSpeed(&speed);
  Serial.print(motorLabel);
  Serial.print(" ");
  if (result == 0) {
    Serial.println("Speed is " + String(speed));
  } else {
    Serial.println("Cannot read speed. E:" + String(result) + " " +
                   BLVD20KM_asukiaaa::getStrOfError(result));
  }
}

void readAndPrintAlarm(String motorLabel, BLVD20KM_asukiaaa* motor) {
  uint16_t alarmState;
  auto result = motor->readAlarm(&alarmState);
  Serial.print(motorLabel);
  Serial.print(" ");
  if (result == 0) {
    Serial.println("Current alarm:0x" + String(alarmState, HEX) + " " +
                   BLVD20KM_asukiaaa::getStrOfAlarm(alarmState));
  } else {
    Serial.println("Cannot read alarm. E:" + String(result) + " " +
                   BLVD20KM_asukiaaa::getStrOfError(result));
  }
}

void loop() {
  motorA.writeSpeed(500);
  motorA.writeForward();
  readAndPrintSpeed("motorA", &motorA);
  delay(2000);

  motorB.writeSpeed(500);
  motorB.writeForward();
  readAndPrintSpeed("motorB", &motorB);
  delay(2000);

  motorA.writeStop();
  Serial.println("motorA stop");
  delay(2000);

  motorB.writeStop();
  Serial.println("motorB stop");
  delay(2000);

  motorA.writeSpeed(200);
  motorA.writeReverse();
  readAndPrintSpeed("motorA", &motorA);
  delay(2000);

  motorB.writeSpeed(200);
  motorB.writeReverse();
  readAndPrintSpeed("motorB", &motorB);
  delay(2000);

  motorA.writeStop();
  Serial.println("motorA stop");
  delay(2000);

  motorB.writeStop();
  Serial.println("motorB stop");
  delay(2000);

  readAndPrintAlarm("motorA", &motorA);
  readAndPrintAlarm("motorB", &motorB);
  delay(2000);
}
