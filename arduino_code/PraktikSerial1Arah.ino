byte inSer, str;
void setup() {
   pinMode(7, OUTPUT);
   Serial.begin(9600);
}

void loop() {
  Serial.println("Lampu Berkedip");
  digitalWrite(7, HIGH);
  delay(500);
  digitalWrite(7, LOW);
  delay(500);
}
