#include <Braccio++.h>

#define JOYSTICK_LEFT        1
#define JOYSTICK_RIGHT       2
#define JOYSTICK_SEL_BUTTON  3
#define JOYSTICK_UP          4
#define JOYSTICK_DOWN        5
#define BUTTON_ENTER         6

int motorID = 0;
float initialAngle = SmartServoClass::MAX_ANGLE / 2.0f;
float currentAngle = initialAngle;

void setup() {
  Braccio.begin();
  delay(500); // Waits for the Braccio initialization

  // Sets the initial angle for the motors
  for (int i = 1; i <= 6; i++) {
    Braccio.move(i).to(initialAngle);
    delay(1000); // Necessary to set the motor ID correctly
  }

  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("Press the up and down joystick to select one motor.");
}

void loop() {
  int pressedKey = Braccio.getKey();

  // Check if the control key pressed is the Joystick Up
  if (pressedKey == JOYSTICK_UP) {

    if (motorID < 6) {
      motorID++;  // Increment the ID
    } else {
      motorID = 1; // Restart the ID to motor 1
    }

    currentAngle = initialAngle;
    
    while (Braccio.getKey() == JOYSTICK_UP); // Avoids more than one increment

    Serial.println("Motor " + String(motorID) + " selected.");
    Serial.println("Use the left and right joystick to move the selected motor.");
  }

  // Check if the control key pressed is the Joystick Down
  if (pressedKey == JOYSTICK_DOWN) {

    if (motorID > 1) {
      motorID--;  // Increment the ID
    } else {
      motorID = 6; // Select motor 6
    }

    while (Braccio.getKey() == JOYSTICK_DOWN); // Avoids more than one increment

    Serial.println("Motor " + String(motorID) + " selected.");
    Serial.println("Use the left and right joystick to move the selected motor.");
  }

  // Check if the Joystick Left is pressed
  if (pressedKey == JOYSTICK_LEFT) {
    currentAngle = currentAngle + 5.00;
  }

  // Check if the Joystick Right is pressed
  if (pressedKey == JOYSTICK_RIGHT) {
    currentAngle = currentAngle - 5.00;
  }

  Braccio.move(motorID).to(currentAngle); // Moves the motor to the new angle
  delay(100);
}
