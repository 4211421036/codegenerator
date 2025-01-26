// Perpusatakaan yang dibutuhkan, harap diinstall melalui Libary Manager di Arduino IDE
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "spo2_algorithm.h"

// Inisialisasi sensor MAX30105 dengan nama Particle Sensor
MAX30105 particleSensor;
const byte RATE_SIZE = 4;  // Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];     // Array of heart rates
byte rateSpot = 0;         // Rate Spot awalan
long lastBeat = 0;         // Time at which the last beat occurred

// Bilangan berupa float beat per minutes
float beatsPerMinute;
int beatAvg; // Bilangan integer beat avarange

// Inisialisasi nilai maksimal sensor IR pada sensor MAX30105
#define MAX_BRIGHTNESS 255

// Inisialisasi board pada sensor yang digunakan
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
//To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.
uint16_t irBuffer[100];   //infrared LED sensor data
uint16_t redBuffer[100];  //red LED sensor data
#else
uint32_t irBuffer[100];   //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
#endif

int32_t bufferLength;   //data length
int32_t spo2;           //SPO2 value
int8_t validSPO2;       //indicator to show if the SPO2 calculation is valid
int32_t heartRate;      //heart rate value
int8_t validHeartRate;  //indicator to show if the heart rate calculation is valid

byte pulseLED = 21;  //Must be on PWM pin
byte readLED = 22;   //Blinks with each data read

// WiFi connecting untuk IoT ke Telegram yaitu memakai WiFi UNNES
const char *ssid = "mipa";
const char *password = "reuber2020";

// Const byte maksimal yang dijadikan batasan
const byte MAX_BEATS = 150;
unsigned long beatTimestamps[MAX_BEATS]; // Pengambilan sampel beat dari beat maksimal yang ditetapkan
byte beatValues[MAX_BEATS]; // Mengambil nilai dari sampel yang telah ditetapkan
int beatCount = 0; // Inisialisasi nilai awalan beat sensor MAX30105

// Token yang digunakan di telegram untuk mengirimkan pesan notifikasi
#define BOT_TOKEN "6966281115:AAHMTInRWrMoZ22yxY5z7lM4SaYVyqYcLMw"
#define CHAT_ID "-1002023120178" // ID Chat Gruop telegram
WiFiClientSecure secured_client; // Inisialisasi Security WiFi yang digunakan
UniversalTelegramBot bot(BOT_TOKEN, secured_client); // Inisialisasi bot telegram

void setup() {
  Serial.begin(115200); // Bourt rate yang digunakan dalam komunikasi via Arduino IDE untuk menglihat data yang telah dikoneksikan ke dalam IoT Telegram
  Serial.println("Initializing..."); // Proses inisialisasi

  pinMode(pulseLED, OUTPUT); // Pin mode yang digunakan pada sensor MAX30105 yaitu pulsa LED
  pinMode(readLED, OUTPUT); // Pin mode yang digunakan pada sensor MAX30105 yaitu inframerah sensor

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); // Menunnggu atau delay waktu untuk koneksi ke jaringan WiFi
    Serial.println("Connecting to WiFi..."); // Pesan koneksi WiFi
  }
  Serial.println("Connected to WiFi"); // Status koneksi yang ditampilkan jika dikoneksi
  // Tampilkan alamat IP pada serial monitor
  Serial.print("Alamat IP: "); // Alamat IP yang digunakan untuk komunikasi via Telegram
  Serial.println(WiFi.localIP()); // Melihat Alamat IP
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power. "); // Status jika sensor tidak berjalan
    bot.sendMessage(CHAT_ID, "CardioNetBot Error: MAX30105 was not found. Please check wiring/power....", ""); // Pesan error yang dikirimkan di telegram jika sensor error
    while (1); // Waktu tunggu
  }
  Serial.println("Place your index finger on the sensor with steady pressure."); // Pesan via Arduino IDE jika sensor udah bisa bekerja dan mulai sensing detak jantung

  byte ledBrightness = 60;  //Options: 0=Off to 255=50mA
  byte sampleAverage = 16;   //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 3;         //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 400;    //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411;     //Options: 69, 118, 215, 411
  int adcRange = 4096;      //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);  //Configure sensor with these settings
  particleSensor.setPulseAmplitudeRed(0x0A); // Alamat Pulsa Amplitudo pada sensor
  particleSensor.setPulseAmplitudeGreen(0); // Alamat Pulsa Amplitudo pada sensor
  particleSensor.enableDIETEMPRDY();  //Enable the temp ready interrupt. This is required.

  bot.sendMessage(CHAT_ID, "CardioNetBot Ready Sensing SCA", ""); // Bot siap mengirimkan pesan ke telegram dari data sensing yang telah diperoleh
}

void loop() {

  long irValue = particleSensor.getIR(); // Mendeteksi via infrared bahwa sensor siap sensing

  if (checkForBeat(irValue) == true) // Keaddan jika sensing telah berjalan, maka
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0); // Rumus detak jantung

    if (beatsPerMinute < 255 && beatsPerMinute > 20) // keadaan jika nilai lebih dari 255 dan nilai kurang dari 20 maka
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  bufferLength = 100;  //buffer length of 100 stores 4 seconds of samples running at 25sps

  //read the first 100 samples, and determine the signal range
  for (byte i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false)  //do we have new data?
      particleSensor.check();                    //Check the sensor for new data

    redBuffer[i] = particleSensor.getRed(); // Membaca infrared darah 
    irBuffer[i] = particleSensor.getIR(); // Membaca dan validasi infrared yang menambrak kandungan darah
    particleSensor.nextSample();  //We're finished with this sample so move to next sample

    Serial.print(F("red=")); // Serial Komunikasi untuk menampilkan nilai infrared
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir=")); // Serial komunikasi untuk menampilkan nilai infrared
    Serial.println(irBuffer[i], DEC);
  }

  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  
  //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
  for (byte i = 25; i < 100; i++) {
    redBuffer[i - 25] = redBuffer[i];
    irBuffer[i - 25] = irBuffer[i];
  }

  //take 25 sets of samples before calculating the heart rate.
  for (byte i = 75; i < 100; i++) {
    while (particleSensor.available() == false)  //do we have new data?
      particleSensor.check();                    //Check the sensor for new data

    digitalWrite(readLED, !digitalRead(readLED));  //Blink onboard LED with every data read

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();  //We're finished with this sample so move to next sample

    //send samples and calculation result to terminal program through UART
    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.print(irBuffer[i], DEC);
    
    String BpmT = "BPM: " + String(beatsPerMinute) + "\n";
    bot.sendMessage(CHAT_ID, BpmT, "Markdown"); // Bot mengirimkan pesan BPM yang didapat dari sensor
    Serial.print(", BPM="); // Serial komunikasi BPM via Arduino IDE
    Serial.print(beatsPerMinute); // Data serial
    delay(1000); // waktu tunggu
    String BpmTV = "Avg BPM: " + String(beatAvg) + "\n"; // Rata - rata detak jantung yang diukur salama 1 menit
    bot.sendMessage(CHAT_ID, BpmTV, "Markdown"); // Bot mengirimkan rata-rata detak jantung
    Serial.print(", Avg BPM="); // Serial Rata-rata detak jantung via Arduino IDE
    Serial.print(beatAvg); // Data Serial
    delay(1000); // Waktu tunggu

    String smes = "Saturasi Oksigen: " + String(spo2, DEC) + " %\n";
    bot.sendMessage(CHAT_ID, smes, "Markdown"); // Bot mengirimkan saturasi oksigen
    Serial.print(F(", SPO2=")); // Serial komunikasi SPO2 via Arduino IDE
    Serial.print(spo2, DEC); // Data Serial
    delay(1000); // Waktu tunggu

    String soi = "Valid Oksigen: " + String(validSPO2) + "\n";
    bot.sendMessage(CHAT_ID, soi, "Markdown"); // Bot mengirimkan pesan melalui telegram bot
    Serial.print(F(", SPO2Valid=")); // Serial Komunikasi Validasi SPO2
    Serial.println(validSPO2, DEC); // Data Serial
    delay(1000); // Waktu tunggu
  }

  //After gathering 25 new samples recalculate HR and SP02
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  float temperature = particleSensor.readTemperature();
  String ter = "temperature: " + String(temperature) + "\n";
  bot.sendMessage(CHAT_ID, ter, "Markdown");
  delay(1000);

  Serial.print("IR=");
  Serial.print(irValue);
  String CTEM = "temperatureC: " + String(temperature) + "\n";
  bot.sendMessage(CHAT_ID, CTEM, "Markdown");
  Serial.print("temperatureC=");
  Serial.print(temperature, 4);
  delay(1000);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
}