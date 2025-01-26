#include <Adafruit_MFRC630.h>


/* This example requires a second serial part, so no Uno :( */
#ifndef HAVE_HWSERIAL1
void setup() { Serial.begin(115200); Serial.println("This board is not supported!"); }
void loop() { delay(10); }
#else

/* Indicate the pin number where PDOWN is connected. */
#if defined(ESP8266)
#define PDOWN_PIN         (A0)
#else
#define PDOWN_PIN         (A2)
#endif

/* Use UART */
Adafruit_MFRC630 rfid = Adafruit_MFRC630(&Serial1, PDOWN_PIN);

/* Prints out len bytes of hex data in table format. */
static void print_buf_hex(uint8_t *buf, size_t len)
{
  for (uint8_t i = 0; i < len; i++)
  {
    Serial.print("0x");
    if (buf[i] < 16)
    {
      Serial.print("0");
    }
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
}

/* Dumps an entire sector (4*16-byte blocks) to the serial monitor. */
void radio_mifare_dump_sector(uint8_t sector_num)
{
  uint8_t readbuf[16] = { 0 };
  /* Try to read four blocks inside the sector. */
  for (uint8_t b = 0; b < 4 ; b++) {
    uint8_t len = 0;
    len = rfid.mifareReadBlock(sector_num * 4 + b, readbuf);
    if (len == 0) {
      /* No data returned! */
      Serial.print("What!?! No data returned for block ");
      Serial.print(sector_num * 4 + b);
      Serial.println("!");
      #if MOJIC_TRICK
      Serial.println("(ノ ゜Д゜)ノ ︵ ┻━┻");
      #endif
      return;
    } else {
      /* Display the block contents. */
      Serial.print(sector_num * 4 + b); Serial.print(": ");
      print_buf_hex(readbuf, len);
    }
  }
}

/*
 * This more concise loop show the minimim requirements to dump the first 1K
 * of memory from a Mifare Classic or Mifare Plus compatible card. No meaningful
 * error-handling or debug output is present here, so this code is intended
 * as a simple starting point for further work.
 */
bool radio_mifare1K_dump_minimal(void)
{
    bool rc;

    /* Put the IC in a known-state. */
    rfid.softReset();

    /* Configure the radio for ISO14443A-106. */
    rfid.configRadio(MFRC630_RADIOCFG_ISO1443A_106);

    /* Request a tag (activates the near field, etc.). */
    uint16_t atqa = rfid.iso14443aRequest();

    /* Looks like we found a tag, move on to selection. */
    if (atqa)
    {
        uint8_t uid[10] = { 0 };
        uint8_t uidlen;
        uint8_t sak;

        /* Retrieve the UID and SAK values. */
        uidlen = rfid.iso14443aSelect(uid, &sak);
        Serial.print("Found a tag with UUID ");
        for (uint8_t i = 0; i < uidlen; i++) {
            Serial.print(uid[i], HEX);
            Serial.print(" ");
        }
        Serial.println("");
        if (uidlen == 4) {
            /* Assume Mifare Classic/Plus and set the global/default key. */
            rfid.mifareLoadKey(rfid.mifareKeyGlobal);
            /* Try to authenticate sectors 0..15. */
            for (uint8_t s = 0; s < 16; s++) {
                /* Try to authenticate this sector. */
                Serial.print("Sector "); Serial.println(s);
                if(rfid.mifareAuth(MIFARE_CMD_AUTH_A, s*4, uid)) {
                    /* We should be able to read the sector contents now. */
                    radio_mifare_dump_sector(s);
                } else {
                    Serial.print("AUTH_A failed for sector ");
                    Serial.println(s);
                }
            }
            rc = true;
        } else {
            Serial.print("Unexpected UID length: "); Serial.println(uidlen);
            rc = false;
        }
    } else {
        rc = false;     /* No tag found, return false. */
    }

    return rc;
}

/**
 *
 */
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  while (!Serial) {
    delay(1);
  }

  Serial.println("");
  Serial.println("-------------------------------");
  Serial.println("Adafruit MFRC630 Mifare 1K Test");
  Serial.println("-------------------------------");

  pinMode(LED_BUILTIN, OUTPUT);

  /* Try to initialize the IC */
  if (!(rfid.begin())) {
    Serial.println("Unable to initialize the MFRC630. Check wiring?");
    while(1) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
      digitalWrite(LED_BUILTIN, LOW);
      delay(50);
    }
  }

  /*
   * This will be INCREDIBLY chatty on the I2C bus, but can be used as a
   * quick test to wait until a card enters the near field.
   */
  Serial.println("Waiting for an ISO14443-A compatible card ...");
  while (!radio_mifare1K_dump_minimal())
  {
    delay(500);
  }
}

void loop() {
    /* Blinky! */
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

#endif /* HAVE_HWSERIAL1 */
