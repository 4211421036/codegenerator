#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x24, 16, 2);

const int btnIncrease = 3;
const int btnDecrease = 4;
const int kipas = 8; // kipas
const int heater = 9; // heater

int sp = 40;
boolean state;

unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
const long debounceDelay = 50;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(btnIncrease, INPUT_PULLUP);
  pinMode(btnDecrease, INPUT_PULLUP);
  pinMode(kipas, OUTPUT);
  pinMode(heater, OUTPUT);
}

void loop() {
  int s = analogRead(A1);
  float v = (s * 5.0) / 1023.0;
  float suhu = v * 100.0;

  // Debounce for btnIncrease
  if (digitalRead(btnIncrease) == LOW && (millis() - lastDebounceTime1) > debounceDelay) {
    while (digitalRead(btnIncrease) == LOW);
    sp++;
    lastDebounceTime1 = millis();
  }

  // Debounce for btnDecrease
  if (digitalRead(btnDecrease) == LOW && (millis() - lastDebounceTime2) > debounceDelay) {
    while (digitalRead(btnDecrease) == LOW);
    sp--;
    lastDebounceTime2 = millis();
  }

  if (suhu >= sp + 2 && state == 0) {
    digitalWrite(kipas, HIGH);
    digitalWrite(heater, LOW);
    state = 1;
  } else if (suhu < sp - 2 && state == 1) {
    digitalWrite(kipas, LOW);
    digitalWrite(heater, HIGH);
    state = 0;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Point : ");
  lcd.print(sp);
  lcd.setCursor(0, 1);
  lcd.print("Act Suhu : ");
  lcd.print(suhu);
  delay(1000);
}
