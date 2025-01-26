const int buttonSaya = 2;
const int ledSaya =  3;
int buttonStatusSaya = 0;

void setup() 
{
   pinMode(ledSaya, OUTPUT);
   pinMode(buttonSaya, INPUT);
   digitalWrite(buttonStatusSaya, LOW);
}

void loop() 
{
   buttonStatusSaya = digitalRead(buttonSaya); 

   if (buttonStatusSaya == LOW)  
   {
      digitalWrite(ledSaya, HIGH);
   } 
   else 
   {
      digitalWrite(ledSaya, LOW);
   }
}