// FEATHER RP2040 PROP-MAKER WAV PLAYER. Loops through all WAV files
// in specified directory of the CIRCUITPY flash filesystem. Files are
// not alphabetized and will usu. play in order they were installed.
// Requires Adafruit_CPFS and Adafruit_WavePlayer libraries.
// (This is NOT for the Arcada library on M4 boards.)

#include <Adafruit_CPFS.h>
#include <Adafruit_WavePlayer.h>
#include <I2S.h>

FatVolume          *fs        = NULL;  // CIRCUITPY flash filesystem.
I2S                 i2s(OUTPUT);       // I2S peripheral is...
Adafruit_WavePlayer player(false, 16); // mono speaker, 16-bit out.
char                wavPath[] = "/";   // WAVs are here.
volatile bool       playing   = false; // For syncing cores.
volatile bool       load      = false;

void setup() {
  // Start the CIRCUITPY flash filesystem FIRST. Very important!
  fs = Adafruit_CPFS::begin();

  // Start Serial AFTER Adafruit_CPFS, or CIRCUITPY won't show on computer.
  Serial.begin(115200);
  //while(!Serial);

  pinMode(LED_BUILTIN, OUTPUT);

  if (fs == NULL) { // If CIRCUITPY filesystem is missing or malformed...
    // Show error message & blink LED to indicate problem. Full stop.
    Serial.println("Can't access board's CIRCUITPY drive.");
    Serial.println("Has CircuitPython been previously installed?");
    for (;;) digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
  } // else valid CIRCUITPY drive, proceed...

  // Configure I2S, enable audio amp
  i2s.setDATA(PIN_I2S_DATA);
  i2s.setBCLK(PIN_I2S_BIT_CLOCK);
  i2s.setBitsPerSample(16);
  pinMode(PIN_EXTERNAL_POWER, OUTPUT);
  digitalWrite(PIN_EXTERNAL_POWER, HIGH);

  delay(2500); // Give CIRCUITPY a moment to mount on USB, else stutter
}

// Core 0 main loop - scans folder for WAV files to play
void loop() {
  FatFile dir, file;
  char    filename[EXFAT_MAX_NAME_LENGTH + 1];
  if (dir.open(wavPath)) {
    while (file.openNext(&dir, O_RDONLY)) {
      file.getName(filename, sizeof filename);
      if (file.isFile() && (filename[0] != '.') && // Skip directories & dotfiles
          !strcasecmp(&filename[strlen(filename) - 4], ".wav")) {
        play(file);
        delay(1000);
      } // end WAV
      file.close();
    } // end file open
  } // end dir open
} // end loop()

// Play WAV file (already opened above). Runs on core 0.
void play(FatFile file) {
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
