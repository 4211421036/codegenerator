/* This example demonstrates how to use the motor
 * API of the Braccio++ to directly control the
 * angle for each smart servo motor.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Braccio++.h>

/**************************************************************************************
 * FUNCTIONS
 **************************************************************************************/

void test_motor(int const id)
{
  Serial.print("Connecting ..... ");
  for (; !Braccio.get(id).connected(); delay(10)) { }
  Serial.println("OK.");
  delay(500);

  Serial.print("Disengaging .... ");
  Braccio.get(id).disengage();
  Serial.println("OK.");
  delay(500);

  Serial.print("Engaging ....... ");
  Braccio.get(id).engage();
  Serial.println("OK.");
  delay(500);

  Serial.print("Drive to start . ");
  Braccio.get(id).move().to(0.0f);
  Serial.println("OK.");
  delay(1500);

  for (float target_angle = 0.0f; target_angle < SmartServoClass::MAX_ANGLE; target_angle += 1.0f)
  {
    Braccio.get(id).move().to(target_angle);
    delay(250);

    char msg[64] = {0};
    snprintf(msg, sizeof(msg), "Angle (Target | Current) = %.2f | %.2f", target_angle, Braccio.get(id).position());
    Serial.println(msg);
  }
}

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(115200);
  while(!Serial) { }

  if (!Braccio.begin()) {
    Serial.println("Braccio.begin() failed.");
    for(;;) { }
  }

  Serial.println("Testing motor angular movement!");
}

void loop()
{
  Serial.println("Choose motor to test (1 - 6):");
  Serial.println(">> ");

  while((Serial.available() <= 0)) { }
  int const motor_id = Serial.parseInt();
  while(Serial.read() != '\n') { }

  if (motor_id < 1 || motor_id > 6) {
    Serial.println("Error, wrong motor id, choose motor id between 1 and 6");
    return;
  }

  test_motor(motor_id);
}
