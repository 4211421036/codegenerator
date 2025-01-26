#include <BLVD20KM_asukiaaa.h>

#define RS485_DE 4
// #define RS485_RE RS485_DE // you can unify DE and RE
#define RS485_RE 5
#define MOTOR_ADDRESS 1
#define RS485_BAUDRATE 115200
#define RS485_SERIAL Serial1

BLVD20KM_asukiaaa motor(&RS485_SERIAL, MOTOR_ADDRESS, RS485_DE, RS485_RE);

void setup() {
  Serial.begin(115200);
  motor.begin(RS485_BAUDRATE);
}

void readAndPrintSpeed() {
  uint16_t speed;
  auto result = motor.readSpeed(&speed);
  if (result == 0) {
    Serial.println("Speed is " + String(speed));
  } else {
    Serial.println("Cannot read speed. E:" + String(result) + " " +
                   BLVD20KM_asukiaaa::getStrOfError(result));
  }
}

void loop() {
  motor.writeSpeed(500);
  motor.writeForward();
  readAndPrintSpeed();
  delay(2000);

  motor.writeStop();
  delay(1000);

  motor.writeSpeed(200);
  motor.writeReverse();
  readAndPrintSpeed();
  delay(1000);

  motor.writeSpeed(500);
  readAndPrintSpeed();
  delay(1000);

  motor.writeStop();
  uint16_t alarmState;
  auto result = motor.readAlarm(&alarmState);
  if (result == 0) {
    Serial.println("Current alarm: " + String(alarmState));
  } else {
    Serial.println("Current alarm:0x" + String(alarmState, HEX) + " " +
                   BLVD20KM_asukiaaa::getStrOfAlarm(alarmState));
  }
  delay(1000);
}
