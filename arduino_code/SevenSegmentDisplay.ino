//commom anoda 
//Initiation
const int pinledA = 2;
const int pinledB = 3;
const int pinledC = 4;
const int pinledD = 5;
const int pinledE = 6;
const int pinledF = 7;
const int pinledG = 8;
const int pinledDp = 9;

void setup (){
  pinMode(pinledA, OUTPUT);
  pinMode(pinledB, OUTPUT);
  pinMode(pinledC, OUTPUT);
  pinMode(pinledD, OUTPUT);
  pinMode(pinledE, OUTPUT);
  pinMode(pinledF, OUTPUT);
  pinMode(pinledG, OUTPUT);
  pinMode(pinledDp, OUTPUT);
}

//membuat kondisi ssd awal mati
void loop(){
  digitalWrite(pinledA,HIGH);
  digitalWrite(pinledB,HIGH);
  digitalWrite(pinledC,HIGH);
  digitalWrite(pinledD,HIGH);
  digitalWrite(pinledE,HIGH);
  digitalWrite(pinledF,HIGH);
  digitalWrite(pinledG,HIGH);
  digitalWrite(pinledDp,HIGH);
  
  //membuat angka 1 (pinled B dan C berlogika 0)
  digitalWrite(pinledB,LOW);
  digitalWrite(pinledC,LOW);
  digitalWrite(pinledA,HIGH);
  digitalWrite(pinledD,HIGH);
  digitalWrite(pinledE,HIGH);
  digitalWrite(pinledF,HIGH);
  digitalWrite(pinledG,HIGH);
  delay(1000);

  //membuat angka 2 (pinled C dan F berlogika 1)
  digitalWrite(pinledC,HIGH);
  digitalWrite(pinledF,HIGH);
  digitalWrite(pinledA,LOW);
  digitalWrite(pinledB,LOW);
  digitalWrite(pinledD,LOW);
  digitalWrite(pinledE,LOW);
  digitalWrite(pinledG,LOW);
  delay(1000);

  //membuat angka 3 (pinled E dan F berlogika 1)
  digitalWrite(pinledE,HIGH);
  digitalWrite(pinledF,HIGH);
  digitalWrite(pinledA,LOW);
  digitalWrite(pinledB,LOW);
  digitalWrite(pinledC,LOW);
  digitalWrite(pinledD,LOW);
  digitalWrite(pinledG,LOW);
  delay(1000);
}