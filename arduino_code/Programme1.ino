const int buttonPin = 2;     // broche du poussoir
 
void setup() 
{
  // initialise la DEL de l'Arduino comme sortie
  pinMode(LED_BUILTIN, OUTPUT);
  // initialise le poussoir comme entrée
  pinMode(buttonPin, INPUT);
}
 
void loop() 
{
  // Selon l'état du bouton, on allume ou éteint la DEL
  if (digitalRead(buttonPin)== HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);  // Allume la DEL
  } else {
    digitalWrite(LED_BUILTIN, LOW);  // Eteint la DEL
  }
}