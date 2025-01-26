// Audio BFF example for QT Py RP2040. Loops through all WAV files
// in specified directory of the SD Card. Files are
// not alphabetized and will usu. play in order they were installed.
// Requires SdFat - Adafruit Fork and Adafruit_WavePlayer libraries.

#include "SdFat.h"
#include <Adafruit_WavePlayer.h>
#include <I2S.h>

I2S                 i2s(OUTPUT);       // I2S peripheral is...
Adafruit_WavePlayer player(false, 16); // mono speaker, 16-bit out.

volatile bool       playing   = false; // For syncing cores.
volatile bool       load      = false;

#if SD_FAT_TYPE == 0
SdFat sd;
File dir;
File file;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 dir;
File32 file;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile dir;
ExFile file;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile dir;
FsFile file;
#else  // SD_FAT_TYPE
#error invalid SD_FAT_TYPE
#endif  // SD_FAT_TYPE

// I2S GPIO pin numbers
#define pBCLK A3 // QT Py Audio BFF default BITCLOCK
#define pWS   A2 // QT Py Audio BFF default LRCLOCK
#define pDOUT A1 // QT Py Audio BFF default DATA

#define SD_CS_PIN A0 // QT Py Audio BFF SD chip select

#define SPI_CLOCK SD_SCK_MHZ(50)

#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif  ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS

void setup() {

  // debug output at 115200 baud
  Serial.begin(115200);
  while (! Serial) delay(10);
  // setup SD-card
  Serial.print("Initializing SD card...");
  if (!sd.begin(SD_CONFIG)) {
    Serial.println(" failed!");
    return;
  }
  Serial.println(" done.");
}

void setup1() {
  // Configure I2S, enable audio amp
  i2s.setDATA(pDOUT);
  i2s.setBCLK(pBCLK);
  i2s.setBitsPerSample(16);
}

uint8_t num;

// Core 0 main loop - scans folder for WAV files to play
void loop() {

  if (dir.open("/")) {
    while (file.openNext(&dir, O_RDONLY)) {
      char    filename[EXFAT_MAX_NAME_LENGTH + 1];
      file.getName(filename, sizeof filename);
      Serial.println(filename);
      if (file.isFile() && (filename[0] != '.') && // Skip directories & dotfiles
          !strcasecmp(&filename[strlen(filename) - 4], ".wav")) {
        play_i2s(file);
        delay(1000);
      } // end WAV
      file.close();
    } // end file open
  }// end dir open
} // end loop()

// Core 1 main loop handles I2S audio playback. This allows concurrency
// when pre-loading the next audio buffer without getting into hairy DMA.
// Global 'load' flag is set here when core 0 should load next chunk.
void loop1() {
  if (playing) {
    wavSample sample;
    switch (player.nextSample(&sample)) {
     case WAV_LOAD:
      load = true; // No break, pass through...
     case WAV_OK:
      i2s.write((int32_t)sample.channel0 - 32768);
      i2s.write((int32_t)sample.channel1 - 32768);
      break;
     case WAV_EOF:
     case WAV_ERR_READ:
      playing = load = false;
    } // end switch
  } // end playing
} // end loop1()

// Play WAV file (already opened above). Runs on core 0.
void play_i2s(File file) {
  digitalWrite(LED_BUILTIN, HIGH);
  uint32_t  rate;
  wavStatus status = player.start(file, &rate);
  if ((status == WAV_OK) || (status == WAV_LOAD)) {
    if (i2s.begin(rate)) {
      playing = load = true;
      while (playing) {
        if (load || (status == WAV_LOAD)) {
          load   = false;
          status = player.read();
          if (status == WAV_ERR_READ) playing = false;
        } // end load
      } // end playing
      i2s.write((int16_t)0);
      i2s.write((int16_t)0);
      i2s.end();
    } else {
      Serial.println("Failed to initialize I2S!");
    } // end i2s
  } // end WAV_OK
  digitalWrite(LED_BUILTIN, LOW);
} // end play()
