/**
   @file MotionControl.ino
   @brief Cosine wave position control example using timer.
   @note Default I2C Slave Address is 0x20 (KeiganMotor KM-1 Series)
   @date 2020/5/1
   @author Takashi Tokuda (Keigan Inc.)
*/

#include <KM1_I2C.h>
#include <TypeUtility.h>
#include <math.h>
#include <MsTimer2.h> /** Please install MsTimer2 library */

static int addr = 0x20;
KeiganMotor m(addr);

static bool isWorking = false; /** true when motion control is working */
static int read_error_cnt = 0; // read motor measurement error count

// Cosine wave 
#define AMP_DEGREE (30) // Amplitude of cosine wave [degree]
#define WAVE_FREQUENCY (0.5) // frequency
#define PERIOD_MS (1000*1/WAVE_FREQUENCY) // period = 1/f 
#define TIMER_INTERVAL_MS (20) // 20 [milliseconds]
#define ONE_PERIOD_STEPS (PERIOD_MS/TIMER_INTERVAL_MS)
#define WAVES_NUM (10) // Number of waves 

void measurement() {

  if (m.readMotorMeasurement()) {
    Serial.print("[");
    Serial.print(addr, HEX);
    Serial.println("] ");
    Serial.print("Pos [deg]: ");
    Serial.println(m.degree);
    Serial.print("Vel [rpm]: ");
    Serial.println(m.rpm);
    Serial.print("Trq [N*m]: ");
    Serial.println(m.torque);
    Serial.print("read error: ");
    Serial.println(read_error_cnt);

  } else {
    Serial.println("Error!!");
    read_error_cnt ++;
    Serial.print("error code: ");
    Serial.println(m.getError().code);
    Serial.print("read error: ");
    Serial.println(read_error_cnt);

  }
}

void timerHandler() {

  static int count = 0;
  static int i = 0;
  static int stamp = millis();

  count ++;

  interrupts(); // Necessary to allow interrupts in this handler

  // Cosine wave for 1 period (2*pi) by 100 steps. 
  m.moveToPositionDegree(AMP_DEGREE * (cos(i * 2 * M_PI / ONE_PERIOD_STEPS)));
  measurement();
  
  i ++;

  Serial.print("Time [ms]: ");
  Serial.println(millis() - stamp); // Time stamp
  Serial.println();

  if (count >= WAVES_NUM * ONE_PERIOD_STEPS) {
    Serial.println("Stopped.");
    MsTimer2::stop();
    m.led(LED_STATE_ON_SOLID, 255, 0, 0); // Set LED Red
    isWorking = false;
  }
}

void setup() {

  Serial.begin(115200);
  Serial.println("Cosine wave position control");

  Serial.println(ONE_PERIOD_STEPS);

  m.enableCheckSum(true);
  m.enable(); 
  m.speedRpm(1000);
  m.presetPosition(0);

  m.positionIDThresholdDegree(100); // Change the Position PID control area 
  m.positionI(5.5);
  
  m.curveType(0); // Turn off trapezoidal curve.
  m.moveToPosition(0); // Fix to the zero point
  m.led(LED_STATE_ON_SOLID, 255, 255, 0); // Set LED Yellow

  delay(1000);

  // Start repeated timer.
  // Please keep the interval more than 15 [milliseconds]. 
  // The interval less than 15 [ms] can have an effect on its actual interval,
  // especially for Arduino Uno because of its calculation speed or log output via serial.   
  MsTimer2::set(TIMER_INTERVAL_MS, timerHandler);  
  MsTimer2::start();


}


/** @note do not execute write command and read command separately. */
void loop() {



}