/* This example allows a user to test if communication
 * can be established with a motor which can be selected
 * via inputting the desired number in the serial monitor.
 */

#include <Braccio++.h>

void setup()
{
  Serial.begin(115200);
  while(!Serial) { }

  if (!Braccio.begin()) {
    Serial.println("Braccio.begin() failed.");
    for(;;) { }
  }

  Serial.println("Testing motor communication!");
  Serial.println("Choose motor 1 to 6 to test the connection");
  Serial.print(">> ");
}

void loop()
{
  while(Serial.available() <= 0) { }
  int const motorID = Serial.parseInt();
  while(Serial.read() != '\n') { }

  if (motorID < 1 || motorID > 6) {
    Serial.println("Error, wrong motor id, choose value between 1 and 6");
    Serial.print(">> ");
    return;
  }
  
  if (Braccio.connected(motorID))
    Serial.println("Communication with motor " + String(motorID) + " successful");
  else
    Serial.println("Communication  failure - Please check the motor " + String(motorID) + " connection");

  Serial.print(">> ");
}
