// Adafruit_WavePlayer example using Adafruit_ZeroTimer
// (within Adafruit_Arcada) for timing control.

#include <Adafruit_Arcada.h>
#include <Adafruit_WavePlayer.h>

// EVEN FOR 8-BIT WAVS, it's important to use the NATIVE DAC RESOLUTION:
// 10 bits on SAMD21, 12 bits on SAMD51. Initialize DAC by calling
// analogWriteResolution(), passing that value. DAC upscaling in Arduino
// is handled improperly and will result in a slight DC offset. Using
// the native DAC res (upsampling in Adafruit_WavePlayer) corrects this.
#if defined(__SAMD51__)
  #define DAC_BITS   12
#else
  #define DAC_BITS   10
#endif
#define SPEAKER_IDLE (1 << (DAC_BITS - 1))
#if defined(ARCADA_LEFT_AUDIO_PIN)
  #define STEREO_OUT true
#else
  #define STEREO_OUT false
#endif
#define BUFFER_SIZE  2048  // Two 1K load buffers

typedef struct wavList { // Linked list of WAV filenames
  char           *filename;
  struct wavList *next;
};

Adafruit_Arcada     arcada;
Adafruit_WavePlayer player(STEREO_OUT, DAC_BITS, BUFFER_SIZE);
bool                readflag = false; // See wavOutCallback()
bool                playing  = false;
char               *wavPath  = "wavs";
wavList            *wavPtr   = NULL;

// Crude error handler. Prints message to Serial Monitor, blinks LED.
void fatal(const char *message, uint16_t blinkDelay) {
  Serial.begin(9600);
  Serial.println(message);
  for(bool ledState = HIGH;; ledState = !ledState) {
    digitalWrite(LED_BUILTIN, ledState);
    delay(blinkDelay);
  }
}

void setup(void) {
  if(!arcada.arcadaBegin())     fatal("Arcada init fail!", 100);
  // TinyUSB teporarily disabled -- was leading to filesys corruption?
#if 0 && defined(USE_TINYUSB)
  if(!arcada.filesysBeginMSD()) fatal("No filesystem found!", 250);
#else
  if(!arcada.filesysBegin())    fatal("No filesystem found!", 250);
#endif
  Serial.begin(9600);
  //while(!Serial) yield();

  analogWriteResolution(DAC_BITS); // See notes above

  // Build a looped list of WAV filenames...
  wavPtr = makeWavList(wavPath, true);
  if(wavPtr) arcada.chdir(wavPath);
  else       fatal("No WAVs found!", 500);
}

void loop(void) {
  File file;
  Serial.printf("Trying: '%s'\n", wavPtr->filename);
  if(file = arcada.open(wavPtr->filename, FILE_READ)) {
    uint32_t sampleRate;
    do { // Wait for prior WAV (if any) to finish playing
      yield();
    } while(playing);
    wavStatus status = player.start(file, &sampleRate);
    if((status == WAV_LOAD) || (status == WAV_EOF)) {
      // Begin audio playback
      playing = true;
      if(status == WAV_LOAD) readflag = true;
      arcada.enableSpeaker(true);
      arcada.timerCallback(sampleRate, wavOutCallback);
      do { // Repeat this loop until WAV_EOF or WAV_ERR_*
        if(readflag) {
          readflag = false; // reset flag BEFORE the read!
          status   = player.read();
          yield();
        }
      } while((status == WAV_OK) || (status == WAV_LOAD));
      // Might be EOF, might be error
      Serial.print("WAV end: ");
      Serial.println(status);
    } else {
      Serial.print("WAV error: ");
      Serial.println(status);
    }
    file.close();
  }

  wavPtr = wavPtr->next; // List loops around to start

  // Audio might be continuing to play at this point! It's switched
  // off in wavOutCallback() below only when final buffer is depleted.
}

// Single-sample-playing callback function for timerCallback() above.
void wavOutCallback(void) {
  wavSample sample;
  wavStatus status = player.nextSample(&sample);
  if((status == WAV_OK) || (status == WAV_LOAD)) {
#if STEREO_OUT
    analogWrite(ARCADA_LEFT_AUDIO_PIN , sample.channel0);
    analogWrite(ARCADA_RIGHT_AUDIO_PIN, sample.channel1);
#else
    analogWrite(ARCADA_AUDIO_OUT      , sample.channel0);
#endif
    // If nextSample() indicates it's time to read more WAV data,
    // set a flag and handle it in loop(), not here in the interrupt!
    // The read operation will almost certainly take longer than a
    // single audio sample cycle and would cause audio to stutter.
    if(status == WAV_LOAD) readflag = true;
  } else if(status == WAV_EOF) {
    // End of WAV file reached, stop timer, stop audio
    arcada.timerStop();
#if STEREO_OUT
    analogWrite(ARCADA_LEFT_AUDIO_PIN , SPEAKER_IDLE);
    analogWrite(ARCADA_RIGHT_AUDIO_PIN, SPEAKER_IDLE);
#else
    analogWrite(ARCADA_AUDIO_OUT      , SPEAKER_IDLE);
#endif
    arcada.enableSpeaker(false);
    playing = readflag = false;
  } // else WAV_ERR_STALL, do nothing
}

// Scan a directory for all WAVs, build and return a linked list. Does NOT
// filter out non-supported WAV variants, but Adafruit_WavePlayer handles
// most non-compressed WAVs now, so we're in decent shape all considered.
// List is NOT sorted, uses the order they come out of openFileByIndex().
wavList *makeWavList(char *path, bool loop) {
  File     file;
  char     filename[SD_MAX_FILENAME_SIZE+1];
  wavList *listHead = NULL, *listTail = NULL, *wptr;

  for(int i=0; file = arcada.openFileByIndex(path, i, O_READ, "wav"); i++) {
    yield();
    // Next WAV found, alloc new wavlist struct, try duplicating filename
    if((wptr = (wavList *)malloc(sizeof(wavList)))) {
      file.getName(filename, SD_MAX_FILENAME_SIZE);
      if((wptr->filename = strdup(filename))) {
        // Struct and filename allocated OK, add to linked list...
        if(listTail) listTail->next = wptr;
        else         listHead       = wptr;
        listTail = wptr;
      } else {
        free(wptr); // Filename alloc failed, delete struct
      }
    }
    file.close();
  }

  // If loop requested and any items in list, make list circular...
  if(loop && listTail) listTail->next = listHead;

  return listHead;
}
