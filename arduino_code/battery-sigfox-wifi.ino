/*
 * THiNX Sigfox/WiFi Example
 * 
 * This sample can be used for dual-network reporting like home security backup:
 * - system is expected to be backed up with a battery (sample sends voltage only)
 * - system can report WiFi and power outages incl. security alerts using Sigfox
 * - system can report important events without working WiFi connection
 * - when WiFi is available, system is listening to MQTT on wakeup 
 * - new firmware can be offered from THiNX backend
 * 
 * This test sample awakes device every `autoSleepTime` microseconds to send a Sigfox message.
 * Message reports current battery voltage (float) converted to hex string. 
 * In order to measure battery voltage, connect battery (+) using 100k resistor with A0.
 * See: https://arduinodiy.wordpress.com/2016/12/25/monitoring-lipo-battery-voltage-with-wemos-d1-minibattery-shield-and-thingspeak/
 */

// Note: Set Serial Monitor to "Newline" for TelekomDesign TD1208 Sigfox device

#include <SoftwareSerial.h>
#include <THiNXLib.h>

THiNX thx;

SoftwareSerial Sigfox(D2, D1); // RX (e.g. yellow), TX (e.g. orange) -- it's worth noting wire colors here

#define SLEEP_TIME_MICROS 3600e6

#define DEFAULT_SLEEP_DELAY (millis() + 60 * 1000) // autosleep in 60 seconds

unsigned long autoSleepTime = DEFAULT_SLEEP_DELAY;

unsigned int raw = 0;
float voltage = 0.0;

const char *ssid = "THiNX-IoT";
const char *pass = "<enter-your-ssid-password>";

void resetAutoSleepTime() {
  autoSleepTime = DEFAULT_SLEEP_DELAY; // Will power down in 60 secs to let the SigFox message get delivered...
}

/* Called after library gets connected and registered */
void finalizeCallback () {
  Serial.print("THiNX check-in completed. Waking up in ");

  uint32_t micro = SLEEP_TIME_MICROS;
  //micro = 300e6; // testing 5 minutes 
  Serial.print(micro/1000000); Serial.println(" seconds");
  ESP.deepSleep(micro);
}

/* Measures and stores current ADC voltage in global float variable */
void measureBatteryVoltage() {
  Serial.println("Measuring battery voltage..");
  raw = analogRead(A0); // connect battery (+) through 100k resistor to A0
  voltage = raw/1023.0;
  voltage = voltage * 3.7; // my battery is 3.7V, original example has 4.2
  Serial.print("Measured voltage: ");
  Serial.print(voltage);
  Serial.println(" V");
}

/* Takes current voltage and sends as byte-string to SigFox backends */
void updateSigfoxStatus() {
  String voltageString = "ba"; // means battery voltage in THiNX, what about Sigfox?  
  unsigned char * chpt = (unsigned char *)&voltage;
  int i;
  Serial.print("Here are the bytes in memory order: ");
  for (i = 0; i < sizeof(voltage); i++) {
     String byteVal = String(chpt[i],HEX);
     // Pad with zero
     if (byteVal.length() == 1) { byteVal = "0" + byteVal; }
     Serial.print(byteVal);
     voltageString = voltageString + byteVal;       
  }
  Serial.print("\n");
  Serial.print("Voltage string for sigfox: ");
  Sigfox.println(voltageString);
  
  Serial.print("Sending Sigfox command: ");  
  Serial.print("AT$SF="); Serial.println(voltageString);
  
  Sigfox.print("AT$SF="); Sigfox.println(voltageString);
}

/* Takes current voltage and sends as string to THiNX backends */
void updateDashboardStatus() {
  String statusString = String("Battery ") + String(voltage) + String("V");
  Serial.print("*INO: Setting status: ");
  Serial.println(statusString);
  thx.setStatus(statusString);
}

void setup() {  
 
  pinMode(A0, INPUT);  

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1);
  }; // wait for connection
  
  Serial.begin(115200);
  while (!Serial);
  Serial.println("\nI'm awake.");
  Serial.setTimeout(2000);

  Serial.println("Initializing Sigfox Serial...");
  Sigfox.begin(9600);
  while (!Sigfox);

  measureBatteryVoltage();
  
  updateSigfoxStatus();

  // API Key, Owner ID  
  // ENTER YOUR OWN OWNER ID AND API KEY HERE BEFORE FIRST RUN!!!
  // Otherwise device will be registered to THiNX test account and you'll need to reclaim ownership. 
  
  thx = THiNX("4721f08a6df1a36b8517f678768effa8b3f2e53a7a1934423c1f42758dd83db5", "cedc16bb6bb06daaa3ff6d30666d91aacd6e3efbf9abbc151b4dcade59af7c12");
  
  // The check-in should not happen before calling thx.loop() for the first time, 
  // so this is a right  place to pre-set status for first check-in.  
  updateDashboardStatus();   

  // Optionally, this is called on completion (when MQTT server is contacted).
  thx.setFinalizeCallback(finalizeCallback);  
}

void loop() {      

  // Always call the loop or THiNX will not happen (there's callback available, see library example)
  thx.loop();

  // serial echo
   if (Sigfox.available()) {
    Serial.write(Sigfox.read());    
    resetAutoSleepTime();
   }
    
  if (Serial.available()) {
    Sigfox.write(Serial.read());
    resetAutoSleepTime();
  }

  // autosleep requires connecting WAKE (D0?) and RST pin  
  if (millis() > autoSleepTime) {
    Serial.println("Going into deep sleep for 1 hour...");
    Serial.println(millis());    
    ESP.deepSleep(SLEEP_TIME_MICROS);
  }      
}
