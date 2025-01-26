#include "Arduino_EMBRYO_2.h"

/* I N I T I A L   D E F I N I T I O N S */
#define motor01      1

/* C O N S T A N T S   A N D   V A R I A B L E S */
const int enablePin = A5;      // Enable Pin
const int DirPin = 12;         // Direction Pin for motor01
const int PulPin = A4;         // Step Pin for motor01
const int ForwardPin  = A2;    // Forward Button for motor01
const int BackwardPin = A1;    // Backward Button for motor01

const int startPin = 2;        // Start Button
const int emergencyPin  = 5;   // Emergency Button
const int HomeEndstop = 3;     // Home endstop
const int FarEndstop = 4;      // Far From Home endstop

long steps = 0;

// Construct object, Embryo(Axis, Enable Pin, Direction Pin, Pulse Pin, Endstop Home, Endstop Far, Forward Button, Backward Button, Start Button, Emergency)
StepMotor motor(motor01,
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
  // put your setup code here, to run once:
  Serial.begin(9600);          // Configure and start Serial Communication
  while (!Serial) {};          // Wait to open the serial monitor
  
  motor.begin();               // Configure inputs pins, outputs pins and interruptions pins
  motor.startWithoutHoming();  // Initialize the motor without homing procedure, detached
                               // interruptions pins and configures endstops as OUTPUT
                               // Use this function only with the motor outside the axis
} 

void loop() {
  // put your main code here, to run repeatedly:
  // Check the forward button signal
  if(motor.readBtnForward() == HIGH)
    motor.moveForward();        // Motor rotates clockwise

  // Check the backward button signal
  if(motor.readBtnBackward() == HIGH)
    motor.moveBackward();       // Motor rotates anticlockwise
}
