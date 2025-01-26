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
const int emergencyPin  = 5;    // Emergency Stop Button
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
  // put your setup code here, to run once:
  // Configure and start Serial Communication
  Serial.begin(9600); 
  while (!Serial) {};           // Wait for serial port to connect. 

  axis.begin();                 // Configure inputs pins, outputs pins and interruptions pins
  
  axis.checkInputs();           // Verify the boolean value of the axis inputs and print in the serial monitor
} 

void loop() {
  // put your main code here, to run repeatedly:
  // Check the forward button signal
  if(axis.readBtnForward() == HIGH)
    Serial.println("Forward Button was pressed!");

  // Check the backward button signal
  if(axis.readBtnBackward() == HIGH)
    Serial.println("Backward Button was pressed!");
  
  // Check the Endstop Home switch signal
  if(axis.readEndstopHome() == HIGH)
    Serial.println("Endstop Home was pressed!");

  // Check the Endstop Far switch signal
  if(axis.readEndstopFar() == HIGH)
    Serial.println("Endstop Far was pressed!");
  
  // Check the Emergency Stop Button signal
  if(axis.readBtnEmergencyStop() == LOW)
    // The emergency stop button is attached in the interrupt service routine
    // So when you click the button, you will see the Emergency message printed too
    Serial.println("Emergency Stop Button was pressed!");
}
