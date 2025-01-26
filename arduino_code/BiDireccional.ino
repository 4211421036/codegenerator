#include <ObloqAdafruit.h>
#include <SoftwareSerial.h>

#define WIFI_SSID      "SSID_de_Wifi"
#define WIFI_PASSWORD  "PWD_de_WIFI"
#define IO_USERNAME    "usuario_adafruit"
#define IO_KEY         "key_adafruit"

SoftwareSerial softSerial(10,11);
ObloqAdafruit olq(&softSerial,WIFI_SSID,WIFI_PASSWORD,IO_USERNAME,IO_KEY);

unsigned long millisAnterior = 0;
int estadoLampara = 0;

void mensajeRecibido(const String& topic,const String& mensaje)
{
    if(topic=="lampara"){
      if(mensaje=="ON"){
        digitalWrite(13,1);
      }else{
        digitalWrite(13,0);
      }
    }
}

void setup()
{
    softSerial.begin(9600);
    olq.setMsgHandle(mensajeRecibido);
    olq.subscribe("lampara");
}

void loop()
{
    olq.update();

    // no usar delay!, chequear el tiempo transcurrido con millis()
    if(millis() - millisAnterior > 5000)
    {
        olq.publish("temperatura", analogRead(A0));
        millisAnterior =  millis();
    }

}
