#include <Stepper.h>
#define STEPS 100  // Jumlah langkah motor


// Pin untuk motor stepper
Stepper stepper(STEPS, 8, 9, 10, 11);


// Pin untuk tombol push
const int BUTTON = 7;


// Jumlah langkah yang diinginkan setiap kali tombol ditekan
const int STEP_AMOUNT = 10; 


// Status sebelumnya dari tombol push
int lastButtonState = LOW;


void setup() {
  stepper.setSpeed(30);
  pinMode(BUTTON, INPUT);
}


void loop() {
  int currentButtonState = digitalRead(BUTTON);
  
  // Periksa apakah tombol baru saja ditekan (transisi dari LOW ke HIGH)
  if (lastButtonState == LOW && currentButtonState == HIGH) {
    stepper.step(STEP_AMOUNT);  // Bergerak sejumlah langkah yang diinginkan
  }
  
  // Perbarui status tombol terakhir untuk perbandingan selanjutnya
  lastButtonState = currentButtonState;


  // Tunda sedikit untuk debounce
  delay(10);
}
