#include <Braccio++.h>

#define JOYSTICK_LEFT        1
#define JOYSTICK_RIGHT       2
#define JOYSTICK_SEL_BUTTON  3
#define JOYSTICK_UP          4
#define JOYSTICK_DOWN        5
#define BUTTON_ENTER         6

int motorID = 0;
float initialAngle = SmartServoClass::MAX_ANGLE / 2.0f; // 315 degrees / 2 = 157.5 degrees
float currentAngle = initialAngle;

void setup() {
  Braccio.begin(); // Initialize Braccio
  
  delay(500); // Waits for the Braccio initialization
  
  // Sets the initial angle for the motors
  for(int i = 1; i <= 6; i++){
    Braccio.move(i).to(initialAngle);
    delay(1000); // Necessary to set the motor ID correctly
  }
    
  Serial.begin(115200);
  
  Serial.println("Press the Enter Button to select the motor 1.");
}

void loop() {
  int pressedKey = Braccio.getKey();
  
  // Check if the control key pressed is the Enter Button
  if(pressedKey == BUTTON_ENTER){
    if(motorID < 6){
      
      motorID++;  // Increment the ID
      
    } else {
      motorID = 1; // Restart the ID to motor 1
    }
    
    currentAngle = initialAngle;
    
    while(Braccio.getKey() == BUTTON_ENTER); // Avoids more than one increment

    Serial.println("Motor " + String(motorID) + " selected.");
    Serial.println("Use the left and right joystick to move the selected motor.");
  }

  // Check if the Joystick Left is pressed
  if(pressedKey == JOYSTICK_LEFT){
    currentAngle = currentAngle + 5.00;
  }

  // Check if the Joystick Right is pressed
  if(pressedKey == JOYSTICK_RIGHT){
    currentAngle = currentAngle - 5.00;
  }
  
  Braccio.move(motorID).to(currentAngle); // Moves the motor to the new angle
  delay(100);
}
