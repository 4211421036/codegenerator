#include "Senses_wifi.h"
#include "DHT.h"

const char *ssid = "your-wifi-network-name";
const char *passw = "your-wifi-password";
const char *userid = "your-senses-user-id";
const char *key = "your-registered-key";

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

String response;
float data;

float t,h;

Senses_wifi myiot;

void setup(){
  Serial.begin(9600);
  dht.begin();
  response = myiot.connect(ssid, passw, userid, key);
  Serial.println(response);
}

void loop(){

  response = "";

  /* - DHT sensor reading - */
  t = dht.readTemperature();
  h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.println("Temperature is " + String(t) + " celcuis");
  Serial.println("Humidity is " + String(h) + " %RH");
  Serial.println("----------------------------------------");
  
  response = myiot.send(1,t); /* myiot.send(slotnumber, data) */
  Serial.println(response);

  response = myiot.send(2,h); /* myiot.send(slotnumber, data) */
  Serial.println(response);

  delay(5000);
}
