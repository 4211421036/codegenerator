#include <Wire.h> 
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char keymap[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad myKeypad = Keypad( makeKeymap(keymap), rowPins, colPins, ROWS, COLS); 

void setup(){
  lcd.init(); // inisialisasi LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Mikrokontroler");
  lcd.setCursor(1,1);
  lcd.print("Selamat Datang");
  delay(5000);
  lcd.clear();
}
  
void loop(){
  lcd.setCursor(2, 0);
  lcd.print("Tekan Keypad");
  
  char keypressed = myKeypad.getKey();
  if (keypressed){
    lcd.setCursor(7,1);
    lcd.println(keypressed);
  }
}