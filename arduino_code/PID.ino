/**
   @file PID.ino
   @brief Read PID Parameters
   @date 2020/5/1
   @author Takashi Tokuda (Keigan Inc.)
*/

#include <KM1_I2C.h>

static int addr = 0x20;
KeiganMotor m(addr);


void setup() {

  Serial.begin(115200);

  Serial.println(); 
  Serial.println("Read PID parameters");

  float qCurrentP = m.readQCurrentP();
  float qCurrentI = m.readQCurrentI();  
  float qCurrentD = m.readQCurrentD();
  float speedP = m.readSpeedP();
  float speedI = m.readSpeedI();
  float speedD = m.readSpeedD();
  float positionP = m.readPositionP();
  float positionI = m.readPositionI();
  float positionD = m.readPositionD(); 
  float positionIDThresholdDegree = m.readPositionIDThresholdDegree();

  Serial.println(); 
  Serial.print("Q_CURRENT_GAIN_P: ");
  Serial.println(qCurrentP);  
  Serial.print("Q_CURRENT_GAIN_I: ");
  Serial.println(qCurrentI);  
  Serial.print("Q_CURRENT_GAIN_D: ");
  Serial.println(qCurrentD);  
  Serial.println(); 
  Serial.print("SPEED_GAIN_P: ");
  Serial.println(speedP);  
  Serial.print("SPEED_GAIN_I: ");
  Serial.println(speedI);  
  Serial.print("SPEED_GAIN_D: ");
  Serial.println(speedD);  
  Serial.println(); 
  Serial.print("POSITION_GAIN_P: ");
  Serial.println(positionP);  
  Serial.print("POSITION_GAIN_I: ");
  Serial.println(positionI);  
  Serial.print("POSITION_GAIN_D: ");
  Serial.println(positionD);    
  Serial.println(); 
  Serial.print("POSITION_ID_THRESHOLD_DEGREE: ");
  Serial.println(positionIDThresholdDegree);   

  // Set and save new PID parameter if you need
  // m.speedP(14);
  // m.saveAllRegisters();

  // You can reset the PID parameters
  // m.resetPID();
  // m.saveAllRegisters();
   
}


void loop() {



}
