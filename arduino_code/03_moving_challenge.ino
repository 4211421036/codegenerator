/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Braccio++.h>

/**************************************************************************************
 * DEFINES
 **************************************************************************************/

#define BUTTON_ENTER    6
#define TIME_DELAY      1000

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static float const HOME_POS[6] = {157.5, 157.5, 157.5, 157.5, 157.5, 90.0};
static float const AGREE_POS[6] = {160.0, 160.0, 210.0, 240.0, 100.0, 180.0};

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
    /* Warning: keep a safe distance from the robot and watch out for the robot's
       movement. It could be speedy and hit someone. */

    /* Move to home position. */
    Braccio.moveTo(HOME_POS[0], HOME_POS[1], HOME_POS[2], HOME_POS[3], HOME_POS[4], HOME_POS[5]);
    delay(500);
  }
}

void loop() {
  int pressedKey = Braccio.getKey();

  if (pressedKey == BUTTON_ENTER)
    movement = true; // Trigger joints' movements

  if (movement) {
    Braccio.moveTo(AGREE_POS[0], AGREE_POS[1], AGREE_POS[2], AGREE_POS[3], AGREE_POS[4], AGREE_POS[5]);
    delay(TIME_DELAY/10);

    for (int i = 1; i <= 10; i++) {
      wristPitch.move().to(190.0f);       delay(TIME_DELAY/2);
      wristPitch.move().to(240.0f);       delay(TIME_DELAY);
      wristPitch.move().to(AGREE_POS[3]);
    }

    movement = false; // Stop joints' movements
  }
}
