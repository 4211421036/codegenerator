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

void setup()
{
    softSerial.begin(9600);
}

void loop()
{
    olq.update();

    // no usar delay!, chequear el tiempo transcurrido con millis()
    if(millis() - millisAnterior > 5000)
    {
        if(estadoLampara==0){
            estadoLampara=1;
        }else{
            estadoLampara=0;
        }
        olq.publish("lampara", estadoLampara);
        millisAnterior =  millis();
    }

}
