/*!
 * @file motor.ino
 * @brief DFRobot's Motor Drive
 * @details The example demonstrates how four D.C motors work at the same time, 
 * @n including operations such as rotation, speed 
 * @n adjustment, brakes and so on.
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT license (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2017-07-31
 * @https://github.com/DFRobot/DFRobot_MotorStepper
*/
 
 
#include "Arduino.h"
#include "Wire.h"
#include "DFRobot_MotorStepper.h"

/*****************Keywords instruction*****************/
//M1--->motor_Group_1--->[M1A(+),M1B(-)]
//M2--->motor_Group_2--->[M2A(+),M1B(-)]
//M3--->motor_Group_3--->[M3A(+),M3B(-)]
//M4--->motor_Group_4--->[M4A(+),M4B(-)]
//CW: rotate in positive direction
//CCW: rotate in reverse
//A0: Chip Selection Address 1
//A1: Chip Selection Address 2
//A2: Chip Selection Address 3
//A3: Chip Selection Address 4
/*****************Function instruction*****************/
//void start(uint8_t dir)
  //*This function can be used to start the motor
  //*dir: Set Orientation       CW or CCW
//void speed(uint16_t val)
  //*This function is used to set the motor speed
  //*val: Set the speed       0<=val<=4096
//void stop()
  //*This function is used to stop the motor turning
  
DFRobot_Motor motor1(M1,A0);
DFRobot_Motor motor2(M2,A0);
DFRobot_Motor motor3(M3,A0);
DFRobot_Motor motor4(M4,A0);


void setup() {
  //initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  //join i2c bus (address optional for master)
  Wire.begin();
  //Initialize D.C motor drive chip (Group1-4)
  motor1.init();
  motor2.init();
  motor3.init();
  motor4.init();
}

void loop() 
{
  //Setting initial velocity(Min:0  Max:4096)
  motor1.speed(4096);
  motor2.speed(4096);
  motor3.speed(4096);
  motor4.speed(4096);
  //Motor1 and motor3 rotate in forward direction; motor2 and motor4 rotate in a reverse direction.
  motor1.start(CW);
  motor2.start(CCW);
  motor3.start(CW);
  motor4.start(CCW);
  delay(2000);

  //Change velocity in 2 seconds
  motor1.speed(1000);
  motor2.speed(1000);
  motor3.speed(1000);
  motor4.speed(1000);
  //All motors rotate in reverse
  motor1.start(CCW);
  motor2.start(CW);
  motor3.start(CCW);
  motor4.start(CW);
  delay(2000);
  
  //All motors brake and stop rotating
  motor1.stop();
  motor2.stop();
  motor3.stop();
  motor4.stop();
  delay(2000);
}
