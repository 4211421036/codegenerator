#include <Braccio++.h>

int motorID= 0;

void setup() {
  Braccio.begin();
  delay(500);
  
  Serial.begin(115200);
  while(!Serial){}
}

void loop() {
  Serial.println("To start, send any key to the serial port:");
  while((Serial.available() <= 0)){};
  Serial.read(); // Clear the serial buffer
  
  for(motorID = 1; motorID <=6; motorID++){
    Serial.println("\nMoving the motor " + String(motorID));
    Serial.println("Current angle 0.0 (zero)");
    
    Braccio.move(motorID).to(0.0f);
    delay(2000);  // delay between movements

    Serial.println("Current angle 90.0");
    Braccio.move(motorID).to(90.0f);
    delay(2000);  // delay between movements

    Serial.println("Current angle 180.0");
    Braccio.move(motorID).to(180.0f);
    delay(2000);  // delay between movements

    Serial.println("Current angle 270.0");
    Braccio.move(motorID).to(270.0f);
    delay(2000);  // delay between movements

    Serial.println("Current angle 315.0\n");
    Braccio.move(motorID).to(315.0f);
    delay(2000);  // delay between movements
  }
}
