#include "Arduino_EMBRYO_2.h"
/* I N I T I A L    D E F I N E S */
#define X_AXIS      1
#define echoPin  9              // Attach pin D9 Arduino to pin Echo of HC-SR04
#define trigPin 10              // Attach pin D10 Arduino to pin Trig of HC-SR04
#define motorOffset 5           // Offset between the sensor and the motor home position

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
int positionNum = 0;

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

int readUltrasonic(){
  long duration;                // Variable for the duration of sound wave travel
  int distance;                 // Variable for the distance measurement
  
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
   // Speed of sound wave divided by 2 (go and back)
  distance = duration * 0.034 / 2;

  return distance;
}

void setup() {
  pinMode(trigPin, OUTPUT);     // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);      // Sets the echoPin as an INPUT

  Serial.begin(9600);           // Configure and start Serial Communication
  while (!Serial) {};           // Wait to open the serial monitor

  axis.begin();                 // Configure inputs pins, outputs pins and interruptions pins

  Serial.println("Press the Start Button to start the machine");
  while(!axis.ready());         // Wait for Start button to be pressed
                                // The start button is attached to the interrupt
                                // service routine that enables the motor and runs
                                // the homing procedure
 
  Serial.println("Move the axis between to minimum position 0cm and the maximum position 45cm");

  Serial.println("Move some object in front of the sensor");
}
void loop() {
  // put your main code here, to run repeatedly:
  // Check the forward button signal
  if(axis.readBtnForward()){
    positionNum = readUltrasonic() - motorOffset;
    // Moves the robot carriage
    axis.toPosition(positionNum);
  }
}
