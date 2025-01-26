#include "Arduino_EMBRYO_2.h"
/* I N I T I A L    D E F I N E S */
#define X_AXIS      1

/*C O N S T A N T S   A N D   V A R I A B L E S*/
const int enablePin = A5;       // Enable Pin
const int DirPin = 5;           // Direction Pin X-axis
const int PulPin = 6;           // Step Pin X-axis
const int BackwardPin = A1;     // Backward Button X-axis
const int ForwardPin  = A2;     // Forward Button X-axis
const int startPin = 2;         // Start Button
const int emergencyPin  = 5;    // Emergency Button
const int HomeEndstop = 3;      // Home endstop X-axis
const int FarEndstop = 4;       // Far From Home endstop X-axis

// Construct object, StepMotor(Axis, Enable Pin, Direction Pin, Pulse Pin, Endstop Home, Endstop Far, Forward Button, Backward Button, Start Button, Emergency Stop Button)
StepMotor axis(X_AXIS,
                enablePin,
                DirPin,
                PulPin,
                HomeEndstop,
                FarEndstop,
                ForwardPin,
                BackwardPin,
                startPin,
                emergencyPin);

void setup() {
  Serial.begin(9600);           // Configure and start Serial Communication
  while (!Serial) {};           // Wait to open the serial monitor

  axis.begin();                 // Configure inputs pins, outputs pins and interruptions pins
  
  Serial.println("Press the Start Button to start the machine");
  while(!axis.ready());         // Wait for Start button to be pressed
                                // The start button is attached to the interrupt
                                // service routine that enables the motor and runs
                                // the homing procedure
}
void loop() {
  // put your main code here, to run repeatedly:
  // Check the forward button signal
  if(axis.readBtnForward() == HIGH)
    axis.moveForward();  // Motor rotates clockwise

  // Check the backward button signal
  if(axis.readBtnBackward() == HIGH)
    axis.moveBackward(); // Motor rotates anticlockwise
}
