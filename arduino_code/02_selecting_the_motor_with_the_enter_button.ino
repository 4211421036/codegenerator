#include <Braccio++.h>

#define BUTTON_ENTER 6

int motorID = 0;
boolean movement = false;
float initialAngle = 0.0;

void setup() {
  Braccio.begin();

  delay(500); // Waits for the Braccio initialization

  // Sets the initial angle for the motors
  for(int i = 1; i <= 6; i++){
    Braccio.move(i).to(0.0f);
    delay(1000); // Necessary to set the motor ID correctly
  }
  
  Serial.begin(115200);
  
  Serial.println("Press the Enter Button to select the motor 1.");
}

void loop() {
  // Check if the control key pressed is the Enter Button
  if(Braccio.getKey() == BUTTON_ENTER){
    
    if(motorID <= 6){
      
      motorID++;  // Increment the ID
      
      if(motorID > 6){
        motorID = 1; // Restart the ID to motor 1
      }
    }
      
    movement = true; // Flag allows the motor to move
    
    while(Braccio.getKey() == BUTTON_ENTER); // Avoids more than one increment
    Serial.println("Motor " + String(motorID) + " selected.");
  }

  if(movement){
    
    for (float angle = 0.0; angle <= 315.0; angle+=45.0){
      Braccio.move(motorID).to(angle);
      
      Serial.println("Motor " + String(motorID) + " - current angle: " + String(angle));
      delay(250);
    }

    delay(500);
    
    for (float angle = 315.0; angle >= 0.0; angle-=45.0){
      Braccio.move(motorID).to(angle);
      
      Serial.println("Motor " + String(motorID) + " - current angle: " + String(angle));
      delay(250);
    }
    
    movement = false;
    
    Serial.println("\n\nPress the Enter Button to select the next motor.\n\n");
  }
}
