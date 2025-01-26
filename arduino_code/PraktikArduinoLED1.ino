void setup() {
  //Menjadikan PIN 2, 3, dan 4 Arduino menjadi Output
pinMode(2, OUTPUT);
pinMode(3, OUTPUT);
pinMode(4, OUTPUT);

}

void led1(int j){
  for (int i = 0; i < 4; i++){
    digitalWrite(2, HIGH);
    delay(j);
    digitalWrite(2, LOW);
    delay(j);
  }
}

void led2(int j){
  for (int i = 0; i < 4; i++){
    digitalWrite(3, HIGH);
    delay(j);
    digitalWrite(3, LOW);
    delay(j);
  }
}

void led3(int j){
  for (int i = 0; i < 4; i++){
    digitalWrite(4, HIGH);
    delay(j);
    digitalWrite(4, LOW);
    delay(j);
  }
}
void loop(){
led1(500);
led2(500);
led3(500);
}
