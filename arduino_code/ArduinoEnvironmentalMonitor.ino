/*
 ArduinoEnvironmentalMonitor

 Get temperature and humidity using an Arduino/Genuino board and
 the DFRobot Accessory Shield which as the DHT11 temperature and
 humidity sensor
 
 created 3 Sept 2016
 by Biagio Montaruli

 this code is in the public domain

 */
#include <AccessoryShield.h>

// declare some variables to store temperature, humidity and heat index :
// tempC -> temperature in degrees Celsius
// tempF -> temperature in degrees Fareneith
// tempK -> temperature in degrees Kelvin
// humidity -> relative humidity
// heatIndex -> heat index 
float tempC, tempF, tempK, humidity, heatIndex;

void setup() {
  // Initialize Serial comunication
  Serial.begin(9600);
  // wait for the serial port to connect, Needed for USB native only
  while(!Serial) ;
  // initialize the Accessory Shield library
  accessoryShield.begin();
}

void loop() {
  // get temperarture in degrees Celsius
  tempC = accessoryShield.getTemperature(DHT11_TEMP_CELSIUS);
  // convert temperature from Celsius to Fareneith 
  tempF = accessoryShield.convertTempCtoF(tempC);
  // convert temperature from Celsius to Kelvin
  tempK = accessoryShield.convertTempCtoK(tempC);
  // check if we have got valid values
  if((tempC != NAN) && (tempF != NAN) && (tempK != NAN)) {
    // print temperature in degrees Celsius
    Serial.print("Temperature in Celsius degrees : ");
    Serial.print(tempC);
    Serial.println(" C");
    // print temperature in degrees Fareneith
    Serial.print("Temperature in Fareneith degrees : ");
    Serial.print(tempF);
    Serial.println(" F");
    // print temperature in degrees Kelvin
    Serial.print("Temperature in Kelvin degrees : ");
    Serial.print(tempK);
    Serial.println(" K");
  }
  else {
    Serial.println("Impossible to get temperature from DHT11 sensor");
  }
  // get humidity from DHT11
  humidity = accessoryShield.getHumidity();
  // check if we got a valid value (humidity value must not be NaN)
  if(humidity != NAN) {
    // print relative humidity value
    Serial.print("Relative humidity : ");
    Serial.print(humidity);
    Serial.println(" %");
  }
  else {
    Serial.println("Impossible to get humidity from DHT11 sensor");
  }
  // compute and print the Heat Index in the Serial monitor
  // For more info about the heat index, see the documentation
  // in source code or follow this link :
  // https://en.wikipedia.org/wiki/Heat_index
  heatIndex = accessoryShield.computeHeatIndex(DHT11_TEMP_CELSIUS);
  Serial.print("Heat index measured in Celsius degrees : ");
  Serial.println(heatIndex);
  Serial.println();
}
