#include "ssd.h"  // Memasukkan file ssd.h yang Anda miliki

// Definisikan pin-pin yang akan digunakan
int pin1 = 2;
int pin2 = 3;
int pin4 = 4;
int pin5 = 5;
int pin6 = 6;
int pin7 = 7;
int pin9 = 8;
int pin10 = 9;

// Buat objek dari kelas SegmentDisplay dengan nama display
SegmentDisplay display(pin1, pin2, pin4, pin5, pin6, pin7, pin9, pin10);

void setup() {
  // panggil fungsi testDisplay untuk menguji tampilan
  display.testDisplay();
  
  delay(2000);  // Tunggu 2 detik
}

void loop() {
  // Menampilkan angka 1
  display.displayHex(1, false); 
  delay(1000);  // Tunggu 1 detik
  
  // Menampilkan angka 2
  display.displayHex(2, false); 
  delay(1000);  // Tunggu 1 detik
  
  // Menampilkan angka 3
  display.displayHex(3, false); 
  delay(1000);  // Tunggu 1 detik
}
