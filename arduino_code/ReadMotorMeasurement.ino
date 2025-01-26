/**
 * @file ReadMotorMeasurement.ino
 * @brief Read Motor Measurement example.
 * @note Default I2C Slave Address is 0x20 (KeiganMotor KM-1 Series)
 * @date 2020/5/1
 * @author Takashi Tokuda (Keigan Inc.)
 */

#include <KM1_I2C.h>
#include <TypeUtility.h>

/** 
 * @def M5_STACK_USE
 * @brief Uncomment if using M5Stack and its LCD display
 */
// #define M5_STACK_USE

static int addr = 0x20;
KeiganMotor m(addr); 

static int read_error_cnt = 0; // read motor measurement error

void setup() {

#ifdef M5_STACK_USE
  M5.begin();
  M5.Lcd.setTextSize(3);
#endif

  Serial.begin(115200);
  Serial.println("Read Motor Measurement");

  m.enable();
  m.runAtVelocityRpm(10);

}


void loop() {

  // Request motor measurement data
  bool success = m.readMotorMeasurement(); 

  float degree = 0; 
  float rpm = 0; 
  float torque = 0; 

  // True if received data is valid (without error).
  if(success){
    
    degree = m.degree;
    rpm    = m.rpm;
    torque = m.torque;

  } else read_error_cnt ++;
    
  Serial.print("[");
  Serial.print(addr, HEX);
  Serial.println("] ");
  Serial.print("Pos [deg]: ");
  Serial.println(degree);
  Serial.print("Vel [rpm]: ");
  Serial.println(rpm);
  Serial.print("Trq [N*m]: ");
  Serial.println(torque);
  Serial.print("read error: ");
  Serial.println(read_error_cnt);
  Serial.print("seconds: ");
  Serial.println(millis() / 1000);  
  Serial.println();
  
#ifdef M5_STACK_USE
  M5.Lcd.setCursor(1, 1);
  M5.Lcd.print("[");
  M5.Lcd.print(addr, HEX);
  M5.Lcd.println("] ");
  M5.Lcd.print("Pos [deg]: ");
  M5.Lcd.println(degree);
  M5.Lcd.print("Vel [rpm]: ");
  M5.Lcd.println(rpm);
  M5.Lcd.print("Trq [N*m]: ");
  M5.Lcd.println(torque);
  M5.Lcd.print("read error: ");
  M5.Lcd.println(read_error_cnt);
  M5.Lcd.print("seconds: ");
  M5.Lcd.println(millis() / 1000);
  M5.Lcd.println();
#endif

}

