/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Braccio++.h>

/**************************************************************************************
 * DEFINES
 **************************************************************************************/

#define BUTTON_SELECT  3
#define BUTTON_ENTER   6
#define TIME_DELAY     2000

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static float const HOME_POS[6] = {157.5, 157.5, 157.5, 157.5, 157.5, 90.0};

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

static auto gripper    = Braccio.get(1);
static auto wristRoll  = Braccio.get(2);
static auto wristPitch = Braccio.get(3);
static auto elbow      = Braccio.get(4);
static auto shoulder   = Braccio.get(5);
static auto base       = Braccio.get(6);

float angles[6];

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup() {
  if (Braccio.begin())
  {
    /* Warning: keep a safe distance from the robot and watch out for the robot's
       movement. It could be speedy and hit someone. */
    /* Move to home position. */
    Braccio.moveTo(HOME_POS[0], HOME_POS[1], HOME_POS[2], HOME_POS[3], HOME_POS[4], HOME_POS[5]);
    delay(TIME_DELAY);
  }
}

void loop() {
  int pressedKey = Braccio.getKey();

  if (pressedKey == BUTTON_ENTER)
  {
    // Pinch movement
    gripper.move().to(230.0f);         delay(TIME_DELAY);
    gripper.move().to(HOME_POS[0]);    delay(TIME_DELAY);

    // // Wrist Roll movement
    wristRoll.move().to(0.0f);          delay(TIME_DELAY);
    wristRoll.move().to(HOME_POS[1]);   delay(TIME_DELAY);
    wristRoll.move().to(315.0f);        delay(TIME_DELAY);
    wristRoll.move().to(HOME_POS[1]);   delay(TIME_DELAY);

    // // Wrist Pitch movement
    wristPitch.move().to(70.0f);       delay(TIME_DELAY);
    wristPitch.move().to(HOME_POS[2]); delay(TIME_DELAY);
    wristPitch.move().to(260.0f);      delay(TIME_DELAY);
    wristPitch.move().to(HOME_POS[2]); delay(TIME_DELAY);

    // Elbow movement
    for(float i=HOME_POS[3]; i >= 70.0; i-=5)
        { elbow.move().to(i);          delay(TIME_DELAY/2000); }
    elbow.move().to(HOME_POS[3]);      delay(TIME_DELAY);
    for(float i=HOME_POS[3]; i <= 260.0; i+=5)
        { elbow.move().to(i);          delay(TIME_DELAY/2000); }
    elbow.move().to(HOME_POS[3]);      delay(TIME_DELAY);

    // Shoulder movement
    shoulder.move().to(120.0f);       delay(TIME_DELAY/2);
    shoulder.move().to(90.0f);        delay(TIME_DELAY/2);
    shoulder.move().to(120.0f);       delay(TIME_DELAY/2);
    shoulder.move().to(HOME_POS[4]);  delay(TIME_DELAY);
    shoulder.move().to(200.0f);       delay(TIME_DELAY/2);
    shoulder.move().to(230.0f);       delay(TIME_DELAY/2);
    shoulder.move().to(200.0f);       delay(TIME_DELAY/2);
    shoulder.move().to(HOME_POS[4]);  delay(TIME_DELAY);

    // Base movement
    base.move().to(0.0f);              delay(TIME_DELAY);
    base.move().to(HOME_POS[5]);       delay(TIME_DELAY);
    base.move().to(315.0f);            delay(TIME_DELAY);
    base.move().to(HOME_POS[5]);       delay(TIME_DELAY);

    while(pressedKey == BUTTON_ENTER) { pressedKey = Braccio.getKey(); }
  }

  if (pressedKey == BUTTON_SELECT)
  {
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
                 "| 6 - Base\t\t|\t" + String(angles[5]) + "\t|\n" +
                 "*****************************************\n\n\n\n\n");

    while(pressedKey == BUTTON_SELECT) { pressedKey = Braccio.getKey(); }
    
  }
}