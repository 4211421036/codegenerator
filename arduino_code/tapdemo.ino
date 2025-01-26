// Basic demo for tap/doubletap readings from Adafruit MSA301

#include <Adafruit_MSA301.h>

Adafruit_MSA301 msa;

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // Try to initialize!
  if (! msa.begin()) {
    Serial.println("Failed to find MSA301 chip");
    while (1) { delay(10); }
  }
  Serial.println("Found MSA301!");

  msa.setPowerMode(MSA301_NORMALMODE);
  msa.setDataRate(MSA301_DATARATE_1000_HZ);
  msa.setBandwidth(MSA301_BANDWIDTH_500_HZ);
  msa.setRange(MSA301_RANGE_2_G);
  msa.setResolution(MSA301_RESOLUTION_14 );

  msa.setClick(false, false, MSA301_TAPDUR_250_MS, 25);
  msa.enableInterrupts(true, true);  // enable single/double tap
}

void loop() {

  uint8_t motionstat = msa.getMotionInterruptStatus();
  if (motionstat) {
    Serial.print("Tap detected (0x"); Serial.print(motionstat, HEX); Serial.println(")");
    if (motionstat & (1<<5)) {
      Serial.println("\t***Single tap");
    }
    if (motionstat & (1<<4)) {
      Serial.println("\t***Double tap");
    }
    Serial.println("");
  }
  delay(10);

}