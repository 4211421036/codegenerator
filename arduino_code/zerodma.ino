// Adafruit_WavePlayer example using Adafruit_ZeroDMA.
// Adafruit_ZeroTimer (within Adafruit_Arcada) is used for
// setting up timing (no per-sample interrupt).

#include <Adafruit_Arcada.h>
#include <Adafruit_ZeroDMA.h>
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
#if 0 && defined(ARCADA_LEFT_AUDIO_PIN)
  #define STEREO_OUT     true
  #define AUDIO_CHANNELS 2
#else
  #define STEREO_OUT     false
  #define AUDIO_CHANNELS 1
#endif
#define BUFFER_SIZE  2048  // Two 1K load buffers

typedef struct wavList { // Linked list of WAV filenames
  char           *filename;
  struct wavList *next;
};

Adafruit_Arcada     arcada;
Adafruit_WavePlayer player(STEREO_OUT, DAC_BITS, BUFFER_SIZE);
Adafruit_ZeroDMA    dma[AUDIO_CHANNELS];
DmacDescriptor     *descriptor[AUDIO_CHANNELS];
char               *wavPath        = "wavs";
wavList            *wavPtr         = NULL;
File                file;                   // Currently-playing WAV file
bool                playing        = false;
bool                stereoDMA      = false; // true only if WAV & hardware are BOTH stereo
void               *nextBuf        = NULL;
uint32_t            nextNumSamples = 0;

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

  int dmacid[] = { // DMA trigger depends on Timer/Counter used by ZeroTimer
#if defined(__SAMD51__)
    TC0_DMAC_ID_OVF, TC1_DMAC_ID_OVF, TC2_DMAC_ID_OVF, TC3_DMAC_ID_OVF,
  #if defined(TC4_DMAC_ID_OVF)        // Higher TC #'s not present on all SAMDs
    TC4_DMAC_ID_OVF, TC5_DMAC_ID_OVF, // And they always come in pairs
  #endif
  #if defined(TC6_DMAC_ID_OVF)
    TC6_DMAC_ID_OVF, TC7_DMAC_ID_OVF
  #endif
#else // SAMD21
    TCC0_DMAC_ID_OVF,
    TCC1_DMAC_ID_OVF,
    TCC2_DMAC_ID_OVF,
    TC3_DMAC_ID_OVF,
    TC4_DMAC_ID_OVF,
    TC5_DMAC_ID_OVF,
  #if defined(TC6_DMAC_ID_OVF)
    TC6_DMAC_ID_OVF,
  #endif
  #if defined(TC7_DMAC_ID_OVF)
    TC7_DMAC_ID_OVF
  #endif
#endif
  };

  // Set up DMA channel & descriptor for each audio output channel...
  for(int i=0; i<AUDIO_CHANNELS; i++) {
    dma[i].allocate();
    dma[i].setTrigger(dmacid[ARCADA_CALLBACKTIMER]);
    dma[i].setAction(DMA_TRIGGER_ACTON_BEAT);
    descriptor[i] = dma[i].addDescriptor(
      NULL,                        // Dummy source pointer for now
#if defined(__SAMD51__)
      (void *)(&DAC->DATA[i].reg), // Dest register = DAC[i]
#else
      (void *)(&DAC->DATA.reg),    // Only one DAC on SAMD21
#endif
      0,                           // Dummy count for now
      DMA_BEAT_SIZE_HWORD,         // Always 16-bit out
      true,                        // Increment source pointer
      false,                       // Don't increment dest pointer
      DMA_ADDRESS_INCREMENT_STEP_SIZE_1, // May override later
      DMA_STEPSEL_SRC);            // Step-select source pointer
    // Attach transfer-done callback to the last channel.
    // Other channel(s) require a dummy callback or the corresponding
    // DMA job doesn't run, unsure why, but OK...
    dma[i].setCallback((i == (AUDIO_CHANNELS-1)) ?
      wavDMAcallback : dummyCallback);
  }
}

void loop(void) {
  do { // Wait for prior WAV (if any) to finish playing
    yield();
  } while(playing);

  Serial.printf("Trying: '%s'\n", wavPtr->filename);

  if(file = arcada.open(wavPtr->filename, FILE_READ)) {
    uint32_t sampleRate;
    uint16_t nChannels;

    wavStatus status = player.start(file, &sampleRate, &nChannels, &nextNumSamples, &nextBuf);
    if((status == WAV_LOAD) || (status == WAV_EOF)) {
      // Begin audio playback
      arcada.enableSpeaker(true);
      arcada.timerCallback(sampleRate, NULL);
      stereoDMA = STEREO_OUT && (nChannels > 1);
      playing   = true;
      issueDMA(nextBuf, nextNumSamples, stereoDMA);
      if(status == WAV_LOAD) {
        // If there's more data in the WAV file, start loading that
        // while the initial buffer plays.
        status = player.read(&nextNumSamples, &nextBuf);
        if(status != WAV_OK) file.close();
      }
    }
  }

  wavPtr = wavPtr->next; // List loops around to start

  // Audio might be continuing to play at this point! It's switched
  // off in wavOutCallback() below only when final buffer is depleted.
}

// Called at the completion of each DMA transfer.
// Sets up next transfer and loads more data.
void wavDMAcallback(Adafruit_ZeroDMA *dma) {
  if(nextBuf) {
    issueDMA(nextBuf, nextNumSamples, stereoDMA);
    if(player.read(&nextNumSamples, &nextBuf) != WAV_OK) {
      // No more data to load; either end of file, or error.
      // Set nextBuf to NULL to indicate no buffer-switch on
      // next DMA callback...however, current buffer will
      // continue to play until depleted (switched off in 'else'
      // case below). It's OK to close the file now.
      nextBuf        = NULL;
      nextNumSamples = 0;
      file.close();
    }
  } else {
    // End of WAV file reached, stop timer, stop audio
    arcada.timerStop();
    playing = false;
#if STEREO_OUT
    analogWrite(ARCADA_LEFT_AUDIO_PIN , SPEAKER_IDLE);
    analogWrite(ARCADA_RIGHT_AUDIO_PIN, SPEAKER_IDLE);
#else
    analogWrite(ARCADA_AUDIO_OUT      , SPEAKER_IDLE);
#endif
    arcada.enableSpeaker(false);
  }
}

// Set up and initiate next DMA transfer
void issueDMA(void *buf, uint16_t count, bool stereo) {
  if(buf && count) {
    player.swapBuffers();
    uint8_t i;
    for(i=0; i<AUDIO_CHANNELS; i++) {
      descriptor[i]->BTCNT.reg = count;
      if(stereo) {
        // If stereo file and stereo out, samples in memory alternate left/right.
        // Increment source address 4 bytes for each sample, and right channel is
        // offset by 2 bytes.
        descriptor[i]->SRCADDR.reg         = (uint32_t)buf + count * 4 + i * 2;
        descriptor[i]->BTCTRL.bit.STEPSIZE = DMA_ADDRESS_INCREMENT_STEP_SIZE_2;
      } else {
        descriptor[i]->SRCADDR.reg         = (uint32_t)buf + count * 2;
        descriptor[i]->BTCTRL.bit.STEPSIZE = DMA_ADDRESS_INCREMENT_STEP_SIZE_1;
      }
    }
    // Start DMA jobs in separate loop to make sure they're closely aligned
    noInterrupts();
    for(i=0; i<AUDIO_CHANNELS; i++) dma[i].startJob();
    interrupts();
  }
}

// Nonsense callback required for stereo output
void dummyCallback(Adafruit_ZeroDMA *dma) { }

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
