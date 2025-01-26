/*
Project Pak Teguh
ada motor steper lengkap dengan driver nya
a. gerakannya dikontrol:
pakai push button, bisa forward dan backward

b. bisa berhenti secara otomatis jika ada sinyal suara tinggi intinya bisa dikontrol dengan suara

ini dulu.
*/

#include <Stepper.h>

#define STEPS_PER_REVOLUTION 200  // Jumlah langkah motor stepper per revolusi
#define MOTOR_PIN_1 2
#define MOTOR_PIN_2 3
#define MOTOR_PIN_3 4
#define MOTOR_PIN_4 5

#define BUTTON_PIN_FORWARD 6
#define BUTTON_PIN_BACKWARD 7
#define SOUND_SENSOR_PIN A0

const int stepsPerRevolution = 200; // Jumlah langkah per revolusi motor stepper
Stepper stepper(STEPS_PER_REVOLUTION, MOTOR_PIN_1, MOTOR_PIN_3, MOTOR_PIN_2, MOTOR_PIN_4);

int soundThreshold = 1024;  // Ambil nilai ambang batas suara sesuai dengan lingkungan Anda
bool soundDetected = false;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_FORWARD, INPUT_PULLUP);
  pinMode(BUTTON_PIN_BACKWARD, INPUT_PULLUP);
  pinMode(SOUND_SENSOR_PIN, INPUT);

  stepper.setSpeed(100);  // Kecepatan pergerakan stepper (sesuaikan dengan kebutuhan Anda)
  Serial.print("Motor Stepper Berjalan dengan kecepatan 100");
}

void loop() {
  // Membaca tombol dan menggerakkan stepper ke depan atau belakang
  if (digitalRead(BUTTON_PIN_FORWARD) == LOW) {
    forward();  // Langkah motor ke depan
  } else if (digitalRead(BUTTON_PIN_BACKWARD) == LOW) {
    backward();  // Langkah motor ke belakang
  }

  // Membaca sensor suara dan menghentikan motor stepper jika suara tinggi terdeteksi
  int soundValue = analogRead(SOUND_SENSOR_PIN);
  if (soundValue > soundThreshold) {
    soundDetected = true;
    stop();  // Menghentikan motor stepper
  } else {
    soundDetected = false;
  }

  // Menggerakkan stepper jika tidak ada suara tinggi yang terdeteksi
  if (!soundDetected) {
    stepper.setSpeed(255);  // Melanjutkan pergerakan motor stepper
  }
}

void forward() {
  int sensorSound = analogRead(A0);
  //Normalisasi Speed Motor
  int stepperSpeed = map(sensorSound, 0, 1024, 0, 100);
  //State Motor Speed
  if(stepperSpeed > 0){
    stepper.setSpeed(stepperSpeed);
    stepper.step(stepsPerRevolution/100);
  }
}

void backward() {
  int sensorSound = analogRead(A0);
  //Normalisasi Speed Motor
  int stepperSpeed = map(sensorSound, 0, 1024, 0, 100);
  //State Motor Speed
  if(stepperSpeed > 0){
    stepper.setSpeed(0);
    delay(1000);
    stepper.setSpeed(stepperSpeed);
    stepper.step(-stepsPerRevolution/100);
  }
}

void stop() {
  int sensorSound = analogRead(A0);
  //Normalisasi Speed Motor
  int stepperSpeed = map(sensorSound, 0, 1024, 0, 100);
  //State Motor Speed
  if(stepperSpeed > 0){
    stepper.setSpeed(0);
  }
}