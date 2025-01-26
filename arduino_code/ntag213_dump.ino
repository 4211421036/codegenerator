#include <Wire.h>
#include <Adafruit_MFRC630.h>

/* Indicate the pin number where PDOWN is connected. */
#if defined(ESP8266)
#define PDOWN_PIN         (A0)
#else
#define PDOWN_PIN         (A2)
#endif

/* Use the default I2C address */
Adafruit_MFRC630 rfid = Adafruit_MFRC630(MFRC630_I2C_ADDR, PDOWN_PIN);

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

/*
 * This more concise loop show the minimim requirements to dump the first 39
 * 4 byte blocks of memory from an NTAG2xx card. No meaningful error-handling
 * or debug output is present here, so this code is intended as a simple
 * starting point for further work.
 */
bool radio_ntag156b_dump_minimal(void)
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
        /* NTAG has a ATQA of 00 44 (Ultralight does as well!). */
        if (atqa == 0x44) {
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
            if (uidlen == 7) {
                /* Try to read the first 42 pages from the card. */
                for (uint8_t i = 0; i < 42; i++) {
                    /* We should be able to read the page contents now. */
                    uint8_t pagebuf[4] = { 0, 0, 0, 0 };
                    uint8_t len = rfid.ntagReadPage(i, pagebuf);
                    Serial.print(i);
                    Serial.print(": ");
                    print_buf_hex(pagebuf, len);
                }
                rc = true;
            } else {
                /* Should be 7, not sure what kind of tag we have. */
                Serial.print("Unexpected UID length: "); Serial.println(uidlen);
                rc = false;
            }
        } else {
            /* Found a tag, but it isn't NTAG */
            Serial.print("Unexpected ATQA value: "); Serial.println(atqa, HEX);
            rc = false;
        }
    } else {
        /* No tag found! */
        rc = false;
    }

    return rc;
}

/**
 *
 */
void setup() {
  Serial.begin(115200);

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
  while (!radio_ntag156b_dump_minimal())
  {
    delay(50);
  }
}

void loop() {
    /* Blinky! */
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
