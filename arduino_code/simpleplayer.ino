// Adafruit_WavePlayer example using the simplePlayer() function.
// This is much easier to use than other methods, but has some limitations:
// - The function "blocks" -- other code can't run while WAV is playing.
// - WAV playback may be slow, buzz or stutter with high sample rate WAVs.
// - analogWriteResolution() may be changed within the library -- if you
//   need a specific resolution following WAV playing, call that function
//   to reset it to your needs.

#include <Adafruit_Arcada.h>
#include <Adafruit_WavePlayer.h>

#if defined(ARCADA_LEFT_AUDIO_PIN)
  #define STEREO_OUT true
#else
  #define STEREO_OUT false
#endif

typedef struct wavList { // Linked list of WAV filenames
  char           *filename;
  struct wavList *next;
};

Adafruit_Arcada     arcada;
// When using the simplePlayer() function, pass 0 for last two args:
Adafruit_WavePlayer player(STEREO_OUT, 0, 0);
char               *wavPath = "wavs";
wavList            *wavPtr  = NULL;

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

  // Build a looped list of WAV filenames...
  wavPtr = makeWavList(wavPath, true);
  if(wavPtr) arcada.chdir(wavPath);
  else       fatal("No WAVs found!", 500);
}

void loop(void) {
  File file;
  Serial.printf("Trying: '%s'\n", wavPtr->filename);
  if(file = arcada.open(wavPtr->filename, FILE_READ)) {
    arcada.enableSpeaker(true);
#if defined(ARCADA_LEFT_AUDIO_PIN)
    wavStatus status = player.simplePlayer(file, ARCADA_LEFT_AUDIO_PIN, ARCADA_RIGHT_AUDIO_PIN);
#else
    wavStatus status = player.simplePlayer(file, ARCADA_AUDIO_OUT, -1);
#endif
    arcada.enableSpeaker(false);
    Serial.print("WAV status: ");
    Serial.println(status);
    file.close();
  }
  wavPtr = wavPtr->next; // List loops around to start
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
