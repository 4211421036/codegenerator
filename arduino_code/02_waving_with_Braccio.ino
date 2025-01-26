/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Braccio++.h>

/**************************************************************************************
 * DEFINES
 **************************************************************************************/

#define BUTTON_ENTER   6
#define TIME_DELAY     1000

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

float const HOME_POS[6] = {157.5, 157.5, 157.5, 157.5, 157.5, 90.0};
float const WAVE_POS[6] = {180.0, 260.0, 157.5, 157.5, 157.5, 180.0};

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

bool movement = false; // Flag to initialize joints' movements

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup() {
  if (Braccio.begin()){
    /* Warning:
       Keep a safe distance from the robot until you make sure the code is properly
       working. Be mindful of the robot’s movement prior to that, as it could be
       speedy and accidentally hit someone.
    */

    /* Move to home position. */
    Braccio.moveTo(HOME_POS[0], HOME_POS[1], HOME_POS[2], HOME_POS[3], HOME_POS[4], HOME_POS[5]);
    delay(TIME_DELAY);
  }
}

// Waving with Wrist pitch
void loop() {
  int pressedKey = Braccio.getKey();

  if (pressedKey == BUTTON_ENTER)
    movement = true; // Trigger joints' movements

  if (movement) {
    Braccio.moveTo(WAVE_POS[0], WAVE_POS[1], WAVE_POS[2], WAVE_POS[3], WAVE_POS[4], WAVE_POS[5]);
    delay(TIME_DELAY);

    for (int i = 1; i <= 5; i++) {
      wristPitch.move().to(120.0f);         delay(TIME_DELAY);
      wristPitch.move().to(200.0f);         delay(2*TIME_DELAY);
      wristPitch.move().to(WAVE_POS[3]);
    }

    movement = false; // Stop joints' movements
  }
}
