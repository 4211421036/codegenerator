#include "Arduino_EMBRYO_2.h"
/* I N I T I A L    D E F I N E S */
#define X_AXIS      1
#define Y_AXIS      2

/*C O N S T A N T S   A N D   V A R I A B L E S*/
const int enablePin = A5;      // Enable Pin
const int DirPinX = 12;        // Direction Pin X-axis
const int PulPinX = A4;        // Step Pin X-axis
const int HomeEndstopX = 3;    // Home endstop X-axis
const int FarEndstopX = 4;     // Far From Home endstop X-axis
const int BackwardPinX = A1;   // Backward Button X-axis
const int ForwardPinX  = A2;   // Forward Button X-axis

const int DirPinY = 10;        // Direction Pin Y-axis
const int PulPinY = 11;        // Step Pin Y-axis
const int HomeEndstopY = 8;    // Home endstop Y-axis
const int FarEndstopY = 7;     // Far From Home endstop Y-axis
const int BackwardPinY = A3;   // Backward Button Y-axis
const int ForwardPinY  = 6;    // Forward Button Y-axis

const int startPin = 2;        // Start Button
const int emergencyPin  = 5;   // Emergency Button

// Construct object, StepMotor(Axis, Enable Pin, Direction Pin, Pulse Pin, Endstop Home, Endstop Far, Forward Button, Backward Button, Start Button, Emergency Stop Button)
StepMotor axisX(X_AXIS,
                enablePin,
                DirPinX,
                PulPinX,
                HomeEndstopX,
                FarEndstopX,
                ForwardPinX,
                BackwardPinX,
                startPin,
                emergencyPin);

StepMotor axisY(Y_AXIS,
                enablePin,
                DirPinY,
                PulPinY,
                HomeEndstopY,
                FarEndstopY,
                ForwardPinY,
                BackwardPinY,
                startPin,
                emergencyPin);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);          // Configure and start Serial Communication
  while (!Serial) {};          // Wait to open the serial monitor
 
  axisX.begin();               // Configure X-axis inputs pins, outputs pins and interruptions pins
  axisY.begin();               // Configure Y-axis inputs pins, outputs pins and interruptions pins

  axisX.checkInputs();         // Verify the boolean value of X-axis inputs and print in the serial monitor
  
  axisY.checkInputs();         // Verify the boolean value of Y-axis inputs and print in the serial monitor
} 

void loop() {
}
