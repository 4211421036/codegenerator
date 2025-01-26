/* This example allows the user to read the
 * inputs of the Braccio++ shield UI devices:
 * - joystick (up,down,left,right,select)
 * - button (enter)
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

  Serial.println("Press any button or move the joystick.");
}

void loop()
{
  if(Braccio.isJoystickPressed_LEFT())  Serial.println("LEFT   (Joystick)");
  if(Braccio.isJoystickPressed_RIGHT()) Serial.println("RIGHT  (Joystick)");
  if(Braccio.isJoystickPressed_SELECT())Serial.println("SELECT (Joystick)");
  if(Braccio.isJoystickPressed_UP())    Serial.println("UP     (Joystick)");
  if(Braccio.isJoystickPressed_DOWN())  Serial.println("DOWN   (Joystick)");
  if(Braccio.isButtonPressed_ENTER())   Serial.println("ENTER  (Button)");
  delay(100);
}
