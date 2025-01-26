// OLED demo for accelerometer readings from Adafruit MSA301

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MSA301.h>
#include <Adafruit_Sensor.h>

Adafruit_MSA301 msa;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);


void setup(void) {
  Serial.begin(115200);
  Serial.println("Adafruit MSA301 demo!");
  
  // Try to initialize!
  if (! msa.begin()) {
    Serial.println("Failed to find MSA301 chip");
    while (1) { delay(10); }
  }
  Serial.println("MSA301 Found!");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  } 
  display.display();
  delay(500); // Pause for 2 seconds
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
}

void loop() {
  sensors_event_t event; 
  msa.getEvent(&event);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("- Adafruit MSA301 -");

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
  Serial.print(" \tY: "); Serial.print(event.acceleration.y); 
  Serial.print(" \tZ: "); Serial.print(event.acceleration.z); 
  Serial.println(" m/s^2 ");

  display.setCursor(0, 8);
  display.print("X: "); display.print(event.acceleration.x); display.println(" m/s^2 ");
  display.print("Y: "); display.print(event.acceleration.y); display.println(" m/s^2 ");
  display.print("Z: "); display.print(event.acceleration.z); display.println(" m/s^2 "); 
  display.display();

  delay(100);
}