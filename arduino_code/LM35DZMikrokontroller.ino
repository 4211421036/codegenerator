#include <LiquidCrystal.h>

LiquidCrystal lcd(2,3,4,5,6,7);

int adc0;
int suhu;

void showStartupMessage() {
  lcd.setCursor(1, 0);
  lcd.print("Selamat Datang!");
  delay(1000);

  lcd.setCursor(2, 1);
  String message = "Sistem Ukur";
  for (byte k = 0; k < message.length(); k++) {
    lcd.print(message[k]);
    delay(300);
  }
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("                ");

  lcd.setCursor(2, 1);
  String message2 = "Suhu LM35DZ";
  for (byte i = 0; i < message2.length(); i++) {
    lcd.print(message2[i]);
    delay(300);
  }
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("                ");

  lcd.setCursor(0, 1);
  String message3 = "Berbasis Arduino";
  for (byte i = 0; i < message3.length(); i++) {
    lcd.print(message3[i]);
    delay(300);
  }
  delay(500);
  delay(2000);
  lcd.clear();
}

void setup() {
  lcd.begin(16,2);
  showStartupMessage();
  delay(1);
  lcd.print("Pengukuran");
  lcd.setCursor(0, 1);
  lcd.print("Temp = ");
}

void loop() {
  adc0 = analogRead(0);
  suhu = adc0 * 0.489;
  lcd.setCursor(7, 1);
  lcd.print(suhu);
  lcd.print("  ");
  delay(1000);
}
