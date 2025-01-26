#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27,20,4);  

const byte ROWS = 4;    
const byte COLS = 4;    

char Keys[ROWS][COLS] = {   
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = { 9, 8, 7, 6 }; 
byte colPins[COLS] = { 5, 4, 3, 2 };

Keypad customKeypad = Keypad( makeKeymap(Keys), rowPins, colPins, ROWS, COLS);    //Masukkan info keypad pada library


char customKey;         
int number = 0;         
int password = 2436;   
int redPin = 13;  
int greenPin = 12;  
void setup() {
  lcd.init ();              
  lcd.setBacklight(HIGH);  
  pinMode(redPin, OUTPUT);  
 pinMode(greenPin, OUTPUT);   
}

void loop() {

  lcd.setCursor(0,0);
  lcd.print("Input Password");      
  
  customKey = customKeypad.getKey();    

  //------------Prosedur jika input berupa angka------------//
  switch(customKey){
    case '0' ... '9':
      lcd.setCursor(0,1);
      number = number * 10 + (customKey - '0');
      lcd.print(number);
    break;

    //------------Jika input '#' maka cek password------------//
    case '#':
      if(number == password){           
        lcd.setCursor(0,1);
        lcd.print("Selamat Datang"); 
        digitalWrite(redPin, LOW);  
        digitalWrite(greenPin, HIGH);  
        number = 0;
        delay(3000);
        lcd.clear(); 
      }
      else{                             
        lcd.setCursor(0,1);
        lcd.print("Invalid Password");  
        digitalWrite(redPin, HIGH);  
        digitalWrite(greenPin, LOW);
        number = 0;
        delay(3000);
        lcd.clear();
      }
    break;

    //------------Jika input '*' maka hapus tampilan------------//
    case '*':
      number = 0;
      lcd.clear();
    break;
  }
}

