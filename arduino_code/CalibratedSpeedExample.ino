#include <CalibratedSpeed.h>

// include your motors declarations here

// next values are determined experimentally and is not part of this sketch
// min speed is the speed at witch your motors start spinning. 
// Adjust so the car goes straight ahead (and steady) at minimum speed in both direction.
// max speed is the maximum speed supported by your driver (usually 255 or the maximum you choose). 
// Lower the max left value if your car goes right and right value if the car goes left,
// until your car goes straight at maximum speed.
#define LeftMinSpeed 24
#define RightMinSpeed 24
#define LeftMaxSpeed 246
#define RightMaxSpeed 255

// creates two objects for calibrating left and right motor speed
CalibratedSpeed csl = CalibratedSpeed(LeftMinSpeed, LeftMaxSpeed);
CalibratedSpeed csr = CalibratedSpeed(RightMinSpeed, RightMaxSpeed);

void setup() {
  // put your setup code here, to run once:

  // include your motors initialization here

  Serial.begin(9600); // only because this example outputs values
  // on serial interface instead of the actual motors

// Accelerate form 0 to maximum speed
  for (int spd = 0; spd < 256; spd++)
  {
    // call getSpeed method of your calibration objects
    // getSpeed accepts the desired speed and retuns the calibrated speed value
    // Here the "calibrated" speed is sent to the serial interface  instead of the actual motors
    Serial.print(csl.getSpeed(spd));
    Serial.print(" ");
    Serial.println(csr.getSpeed(spd));
    delay(10);
  }
  // Deccelerate form maximum speed to 0
  for (int spd = 255; spd >= 0; spd--)
  {
    Serial.print(csl.getSpeed(spd));
    Serial.print(" ");
    Serial.println(csr.getSpeed(spd));
    delay(10);
  }
  // Accelerate form 0 to maximum speed in reverse  direction
  for (int spd = 0; spd < 256; spd++)
  {
    // getSpeed method accepts and returs negative values to acomodate all kind of dc motor drivers
    // and it uses the positive calibration values mirrored in the negative domain
    Serial.print(csl.getSpeed(-spd));
    Serial.print(" ");
    Serial.println(csr.getSpeed(-spd));
    delay(10);
  }
  // Deccelerate form maximum speed to 0 in reverse direction
  for (int spd = 255; spd >= 0; spd--)
  {
    Serial.print(csl.getSpeed(-spd));
    Serial.print(" ");
    Serial.println(csr.getSpeed(-spd));
    delay(10);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
