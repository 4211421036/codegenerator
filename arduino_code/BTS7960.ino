#include <BTS7960.h>
//BTS7960 works between +3v and +5v so you can use a uno, mega or a nano.
#define L_EN 2
#define R_EN 4
#define L_PWM 5                             //pin 5 supports 980hz pwm frequency
#define R_PWM 6                             //pin 6 supports 980hz pwm frequency

BTS7960 motor1(L_EN, R_EN, L_PWM, R_PWM);   //This method will create a object of the class BTS7960

void setup() {
  Serial.begin(9600);                       //begin the serial monitor for output
  motor1.begin();                           //This method will set the motor driver pins as output
  motor1.enable();                          //This method will set the L_EN and R_EN to HIGH or digitalWrite them to +5v/3v depending on your mcu
}

void loop() {
  
  for(int i=0; i<=255; i=i+10)
  {
    motor1.pwm = i;                         //Set the speed, by default the speed is set to 255 you can change it 
    motor1.front();                         //front functions should turn the motor in clockwise direction
    delay(500);
  }
  motor1.stop();                            //stop function should halt the motor by applying 0 pwm to both R_PWM and L_PWM
  delay(500);
  for(int i=0; i<=255; i=i+10)
  { 
    motor1.pwm = i;                         //Set the speed
    motor1.back();                          //back functions should turn the motor in anti-clockwise direction
    delay(500);
  }
  delay(500);
  motor1.disable();                         //This method will set the L_EN and R_EN to LOW or digitalWrite them to 0v
}
