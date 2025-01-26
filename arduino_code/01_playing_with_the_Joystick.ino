#include <Braccio++.h>

String message = "";

String checkJoystick(int input){
  String joystick[] = { "",
    "Joystick left was moved!",
    "Joystick right was moved!",
    "Joystick select button was pressed!",
    "Joystick up was moved!",
    "Joystick down was moved!"};
   return joystick[input];
}

void setup() {
  Serial.begin(115200);
  while(!Serial){}
  Braccio.begin();
  Serial.println("Press any button or move the joystick.");
}

void loop() {
  int joystickPos = Braccio.getKey();
  message = checkJoystick(joystickPos);
  if(message != ""){
    Serial.println(message);
    message = "";
  }
  delay(500);
}
