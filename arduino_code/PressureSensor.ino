// include CanSatKit library used for pressure sensor
#include <CanSatKit.h>

using namespace CanSatKit;

// BMP280 is a pressure sensor, create the sensor object
BMP280 bmp;

void setup() {
  SerialUSB.begin(9600);

  // use begin() function to check if sensor is conneted and is able to communicate
  // with the CanSat board
  if(!bmp.begin()) {
    // if connection failed - print message to the user
    SerialUSB.println("BMP init failed!");
    // the program will be 'halted' here and nothing will happen till restart
    while(1);
  } else {
    // print message to the user if everything is OK
    SerialUSB.println("BMP init success!");
  }
  
  // setOversampling function allows to set resolution of the pressure sensor
  // possible values of setOversampling:
  //  1 -- 2.62 Pa
  //  2 -- 1.31 Pa
  //  4 -- 0.66 Pa
  //  8 -- 0.33 Pa
  // 16 -- 0.16 Pa 
  bmp.setOversampling(16);
  
}

void loop() {
  // declare variables for temperature (T) and pressure (P) readings
  double T, P;

  // start measurement, wait for result and save results in T and P variables 
  bmp.measureTemperatureAndPressure(T, P);
  
  // since we use it solely as pressure sensor print only "Pressure = " text
  SerialUSB.print("Pressure = ");
  // print also value of pressure with two decimal places
  SerialUSB.print(P, 2);
  // print units - in this case the library returns hPa
  SerialUSB.println(" hPa");
}
