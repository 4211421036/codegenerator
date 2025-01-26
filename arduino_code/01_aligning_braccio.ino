/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Braccio++.h>

/**************************************************************************************
 * DEFINES
 **************************************************************************************/

#define TIME_DELAY     1000

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

// Braccio ++ joints
auto gripper    = Braccio.get(1);
auto wristRoll  = Braccio.get(2);
auto wristPitch = Braccio.get(3);
auto elbow      = Braccio.get(4);
auto shoulder   = Braccio.get(5);
auto base       = Braccio.get(6);

float initialBase = 90.0;
float initialAngle = 157.5;

float angles[6];

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup() {
  if (Braccio.begin()){
    /* Warning: keep a safe distance from the robot and watch out for the robot's
       movement. It could be speedy and hit someone. */

    /* Move to home position. */
    gripper.move().to(initialAngle);       delay(TIME_DELAY);
    wristRoll.move().to(initialAngle);     delay(TIME_DELAY);
    wristPitch.move().to(initialAngle);    delay(TIME_DELAY);
    elbow.move().to(initialAngle);         delay(TIME_DELAY);
    shoulder.move().to(initialAngle);      delay(TIME_DELAY);
    base.move().to(initialBase);           delay(TIME_DELAY);
  }

  Serial.begin(115200);
  while(!Serial){}
}

void loop() {
  // Fetch the joints positions
  Braccio.positions(angles);

  // Print the joint angles
  Serial.println("************* Joints Angles *************");
  Serial.println("|\tMotor ID\t|\tAngle\t|");
  Serial.println("----------------------------------------");
  Serial.print("| 1 - Gripper\t\t|\t" + String(angles[0]) + "\t|\n" + 
               "| 2 - Wrist Rotation\t|\t" + String(angles[1]) + "\t|\n" +
               "| 3 - Wrist Vertical\t|\t" + String(angles[2]) + "\t|\n" + 
               "| 4 - Elbow\t\t|\t" + String(angles[3]) + "\t|\n" + 
               "| 5 - Shoulder\t\t|\t" + String(angles[4]) + "\t|\n" + 
               "| 6 - Base\t\t|\t" + String(angles[5]) + "\t|\n");
  Serial.println("*****************************************\n\n\n\n\n");
  Serial.println("\n\n\n\n");
  
  delay(TIME_DELAY);
}
