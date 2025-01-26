/*This code configures and uses the RFM23 radio module to send "Hello World!" messages wirelessly.
It sets up SPI pins, transmit/receive control pins, initializes the radio, and periodically sends the message
while tracking the number of sent messages and the radio module's temperature.*/

#include <SPI.h>
#include <RH_RF22.h>
#include <RHHardwareSPI1.h>

int led = 13;

const int CS_PIN = 38;   // 38
const int INT_PIN = 8;   // 8 on 4.23   //40 on 4.24 or later
const int SPI_MISO = 39; // 39
const int SPI_MOSI = 26; // 26
const int SPI_SCK = 27;  // 27
const int RX_ON = 30;    // low when receiving, high when sending
const int TX_ON = 31;    // low when sending, high when receiving

// create the rf23 object, see https://github.com/jecrespo/RadioHead
RH_RF22 rf23(CS_PIN, INT_PIN, hardware_spi1);

uint8_t testMessage[] = "Hello World!";
// Max message size//      |------------------------------------------------| //49 characters max
unsigned int sendCount = 0;
uint8_t temp = 0;

void setup()
{

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(9600);

  // assign the SPI1 pins
  SPI1.setMISO(SPI_MISO);
  SPI1.setMOSI(SPI_MOSI);
  SPI1.setSCK(SPI_SCK);

  // set RX and TX pins
  pinMode(RX_ON, OUTPUT);
  pinMode(TX_ON, OUTPUT);
  digitalWrite(RX_ON, HIGH); // should be high (inverse logic)
  digitalWrite(TX_ON, LOW);

  Serial.println("rf23_rangeTest_send (runs on OBC 4.23)");
  delay(100);

  // INITIALIZATION
  // radio init function, defined below
  radioInit();
  delay(100);
  // radio setup function, defined below
  radioSetup();
  delay(100);

  temp = rf23.temperatureRead();

  // read the temperature
  Serial.print("radio temperature = ");
  Serial.println(temp);
  delay(100);

  Serial.println("####### end setup ########");
  delay(100);
}

void loop()
{
  digitalWrite(led, HIGH);
  rf23.send(testMessage, sizeof(testMessage));
  delay(1000);
  if (!rf23.sleep())
  { // must sleep between each sent message!!!!
    Serial.println("failed to sleep");
  }
  digitalWrite(led, LOW);
  delay(100);

  sendCount++;

  Serial.print("messages sent = ");
  Serial.println(sendCount);
  if ((sendCount % 10) == 0)
  {
    temp = rf23.temperatureRead();
    Serial.print("radio temperature = ");
    Serial.println(temp);
  }
  delay(100);
}

// initialize the radio
void radioInit()
{
  unsigned int initAtmpt = 0;
  // if frozen here, check radio power supply
  while (!rf23.init())
  {
    Serial.print("driver init failed: ");
    Serial.println(initAtmpt);
    initAtmpt++;
    delay(1000);
  }
  Serial.println("driver init success");
}

// set the radio frequency, power level, and modulation scheme
void radioSetup()
{
  rf23.setFrequency(433); // 433 according to the link budget
                          // RH_RF22_RF23B_TXPOW_13DBM    low power
                          // RH_RF22_TXPOW_20DBM          mid power (.1W )
                          // RH_RF22_RF23BP_TXPOW_30DBM   highest power (for flight) (1W)

  // must put to sleep before switching modulation scheme
  if (!rf23.sleep())
  {
    Serial.println("failed to sleep");
  }
  // set modulation scheme
  if (!rf23.setModemConfig(RH_RF22::GFSK_Rb2Fd5))
  {
    // <FSK_Rb125Fd125>   highest FSK data rate (125kbs)
    // <FSK_Rb2Fd5>       low FSK data rate (2kbs)
    // <GFSK_Rb125Fd125>  highest GFSK rate GFSK (125kbs)
    // <GFSK_Rb2Fd5>      lowest GFSK data rate (2kbs) FAVORED
    // <FSK_Rb_512Fd2_5>  lowest FSK data rate(0.512kbs)

    Serial.println("modulation setting unsuccessful ");
  }
  // make it sleep again just in case
  if (!rf23.sleep())
  {
    Serial.println("failed to sleep");
  }
  else
  {
    Serial.println("setup successful");
  }
}