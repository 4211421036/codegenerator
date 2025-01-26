byte inSer;
char str;

void setup() {
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  Serial.begin(9600);
  Serial.println("Kontrol Led via PC");
}

void loop() {
  if (Serial.available() > 0) {  // Periksa apakah ada data yang tersedia di Serial
    inSer = Serial.read();      // membaca kode ASCII
    str = char(inSer);          // Mengubah data ASCII ke char

    switch (str) {
      case '1':  // LED Kuning Berkedip dengan cepat
        blinkLED(8, 100);
        blinkLED(9, 500);
        blinkLED(10, 1000);
        Serial.println("LED Kuning: Cepat, LED Merah: Sedang, LED Hijau: Lambat");
        break;

      case '2':  // LED Kuning Berkedip dengan lambat
        blinkLED(8, 1000);
        blinkLED(9, 500);
        blinkLED(10, 100);
        Serial.println("LED Kuning: Lambat, LED Merah: Cepat, LED Hijau: Sedang");
        break;

      case '3':  // LED Kuning Berkedip dengan sedang
        blinkLED(8, 500);
        blinkLED(9, 1000);
        blinkLED(10, 100);
        Serial.println("LED Kuning: Sedang, LED Merah: Lambat, LED Hijau: Cepat");
        break;

      case '4':  // Mematikan semua LED
        digitalWrite(8, LOW);
        digitalWrite(9, LOW);
        digitalWrite(10, LOW);
        Serial.println("Semua LED dimatikan");
        break;
    }
  }
}

void blinkLED(int pin, int duration) {
  digitalWrite(pin, HIGH);
  delay(duration / 2);
  digitalWrite(pin, LOW);
  delay(duration / 2);
}
