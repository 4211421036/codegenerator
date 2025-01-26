/*
  temp_humidity_oled

  Simple sketch that show how to use the DHT11 of the DFRobot
  Accessory Shield to get environmental data and print them
  on the OLED display

  created 17 August 2016
  by Biagio Montaruli
  
  This example code is in the public domain.
 */
#include <AccessoryShield.h>

#define DELAY_TIME 4000

float humidity, tempC;

void setup() {
  // initialize Serial comunication
  Serial.begin(9600);
  // wait for the serial port to connect. Needed for Native USB only
  while(!Serial) ;
  // initialize the Accessory Shield library
  accessoryShield.begin();

  // clear the OLED display, otherwise we have have many dots
  // printed on the OLED display of Accessory Shield
  accessoryShield.clearOledDisplay();
  accessoryShield.oledPaint();
  accessoryShield.setTextSize(1);
  accessoryShield.setTextColor(WHITE);
  accessoryShield.setCursor(0,0);
}

void loop() {
  // get environmental data from the DHT11 temperature and hunidity sensor
  // using the method getEnvironmentalData()
  // this method returns a value that represents the state of DHT11 sesnor
  // and receives three parameters :
  // 1) a reference to the variable used to store the humidity
  // 2) a reference to the variable used to store the temperature
  // 3) the temperature unit of measure : CELSIUS, FARENEITH or KELVIN;
  //    represented by the constants DHT11_TEMP_CELSIUS, DHT11_TEMP_FARENEITH
  //    and DHT11_TEMP_KELVIN. By default it uses CELSIUS (DHT11_TEMP_CELSIUS)
  int state = accessoryShield.getEnvironmentalData(humidity, tempC);
  if(state == DHT11_DATA_READ) {
    Serial.println("Successfully read environmental data from DHT11 sensor");
    Serial.print("Temperature in degrees Celsius : ");
    Serial.println(tempC);
    Serial.print("Relative humidity : ");
    Serial.println(humidity);
  }

  accessoryShield.print("Temperature: ");
  accessoryShield.print(tempC, 2);
  accessoryShield.println(" C");
  accessoryShield.print("Humidity: ");
  accessoryShield.print(humidity, 2);
  accessoryShield.println(" %");
  accessoryShield.oledPaint();
  
  delay(DELAY_TIME);
  accessoryShield.clearOledDisplay();
  accessoryShield.oledPaint();
  accessoryShield.setCursor(0,0);
}
