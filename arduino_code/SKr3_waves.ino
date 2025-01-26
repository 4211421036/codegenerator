/*
  This file is part of the Arduino_ScienceKitCarrier library.
  Copyright (c) 2023 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Wire.h"
#include "pin_def.h"
#include "led_gauge.h"
#include "led_range.h"
#include "analogWave.h"
#include "scheduler.h"

/* ########################################################################## */
/* __________________________ CONFIGURATION DEFINES _________________________ */
/* ########################################################################## */
#define USE_CHAT_AT
#define USE_TIMER
//#define USE_DEBUG_CODE 

/* ########################################################################## */
/* _____________________________ OTHERS DEFINES _____________________________ */
/* ########################################################################## */
#define ANRES        9
#define RES          100
#define SERIAL_AT    Serial

/* ########################################################################## */
/* ____________________________GLOBAL VARIABLES _____________________________ */
/* ########################################################################## */



// Firmware version
uint8_t version[2]={0,7};

char command=0;
byte data[6];

uint16_t wave[RES];

LedGauge frequency_1(FQ_1, 10, 10, ROW_A_1, ROW_A_2, ROW_A_3, COL_A_1, COL_A_2, COL_A_3);
LedGauge frequency_2(FQ_2, 10, 10, ROW_B_1, ROW_B_2, ROW_B_3, COL_B_1, COL_B_2, COL_B_3, true);

LedRange range(FQ_R, ROW_C_1, ROW_C_2, ROW_C_3, COL_C_1, COL_C_2);

analogWave wave1(DAC,wave,RES,0);
analogWave wave2(DAC2,wave,RES,0);

int previous_frequency_1 = 10;
int previous_frequency_2 = 10;
int freq1 = 10;
int freq2 = 10;

int previous_phase_1 = 0;
int previous_phase_2 = 0;

int phase1 = 0;
int phase2 = 0;

const float step = ADC_RES/float(RES);
const float phase_to_deg = 180/float(RES);

bool test_flag = false;
unsigned long time_rephase;
int rephase;

/* ########################################################################## */
/* ________CHAT AT___________________________________________________________ */
/* ########################################################################## */

#ifdef USE_CHAT_AT

#include "chATWrapper.h"
static CAtWrapper at(&SERIAL_AT);


#endif //USE_CHAT_AT

/* ########################################################################## */
/* ________TIMER_____________________________________________________________ */
/* ########################################################################## */

#ifdef USE_TIMER
#include "FspTimer.h"
#include "scheduler.h"

static FspTimer timer;
static bool elapsed = false;

/* -------------------------------------------------------------------------- */ 
void task10ms() {
/* -------------------------------------------------------------------------- */    
   frequency_1.refresh();
   frequency_2.refresh();
   range.update();

   freq1=frequency_1.getGaugeValue()*range.getRange1();
   freq2=frequency_2.getGaugeValue()*range.getRange2();  
}

/* -------------------------------------------------------------------------- */ 
void task15ms() {
/* -------------------------------------------------------------------------- */    
   phase1=analogRead(PH_1)/step;
   if (phase1>(RES+10)){
      phase1=0;
   }

   phase2=analogRead(PH_2)/step;
   if (phase2>RES){
      phase2=0;
   }

   if (freq1!=previous_frequency_1){
      wave1.freq(freq1);
      previous_frequency_1=freq1;
      wave1.sync(wave2);
   }

   if (freq2!=previous_frequency_2){
      wave2.freq(freq2);
      previous_frequency_2=freq2;
      wave2.sync(wave1);
   }
}

/* -------------------------------------------------------------------------- */ 
void task20ms() {
/* -------------------------------------------------------------------------- */    
   #ifdef USE_CHAT_AT
   at.run();
   #endif 
}


/* -------------------------------------------------------------------------- */ 
void task500ms() {
/* -------------------------------------------------------------------------- */
   if (phase1!=previous_phase_1){
      wave1.offset(phase1/2);
      previous_phase_1=phase1;
      if (phase2==0){
         wave1.sync(wave2);
      }
   }
   if (phase2!=previous_phase_2){
      wave2.offset(phase2/2);
      previous_phase_2=phase2;
      if (phase1==0){
         wave2.sync(wave1);
      }
   }
}





/* -------------------------------------------------------------------------- */ 
void timer_callback(timer_callback_args_t *arg) {
/* -------------------------------------------------------------------------- */  
  elapsed = true;
}

/* -------------------------------------------------------------------------- */ 
void set_up_timer() {
/* -------------------------------------------------------------------------- */   
   uint8_t type;
   int8_t  num  = FspTimer::get_available_timer(type);
   if(num >= 0) {
      timer.begin(TIMER_MODE_PERIODIC, type, num, 1000,50 , timer_callback);
      timer.setup_overflow_irq();
      timer.open();
      timer.start(); 
   }

   CScheduler::getInstance().add(task10ms,TASK_10ms);
   CScheduler::getInstance().add(task15ms,TASK_15ms);
   CScheduler::getInstance().add(task20ms,TASK_20ms);
   CScheduler::getInstance().add(task500ms,TASK_500ms);

}

/* -------------------------------------------------------------------------- */ 
bool is_tick_elapsed() {
/* -------------------------------------------------------------------------- */    
   if(elapsed) {
      elapsed = false;
      return true;
   }
   return false;
}



#endif //USE_TIMER


// This function loads an wave array with a sinewave using RES samples.

void generate_wave(){
  float s = (2 * 3.1415) / RES;
  for (auto i = 0; i < RES; ++i) {
    float sample = (1 + sin((float)i * s)) * (float)65535 / 2.0;
    wave[i] = (uint16_t)sample;
  }
}


void comm_event(int event){
  command=Wire.read();
}

void comm_request(){
  switch(command){
    case 'V':                         // version, high rev, low rev
      Wire.write(version,2);
      break;
    case 'D':
      data[0]=frequency_1.getGaugeValue();
      data[1]=range.getRange1();
      data[2]=phase1*phase_to_deg;
      data[3]=frequency_2.getGaugeValue();
      data[4]=range.getRange2();
      data[5]=phase2*phase_to_deg;
      Wire.write(data,6);
      break;
    /*
    case 'T':
      test_flag=true;
      break;
    case 'N':
      test_flag=false;
      break;
    */
  }
}

/* ______________________________SETUP_______________________________________ */

/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */   
  Serial.begin(115200);

  #ifdef USE_DEBUG_CODE
  delay(3000);
  Serial.println("STARTED SETUP");
  #endif

  #ifdef USE_TIMER
  set_up_timer();
  #endif
  
  generate_wave();
  pinMode(PH_1,INPUT);
  pinMode(PH_2,INPUT);
  frequency_1.refresh();
  frequency_2.refresh();
  range.update();

  Wire.begin(0x2F);
  Wire.onReceive(comm_event);
  Wire.onRequest(comm_request);

  //wave1.sine(freq1); //these use default sampling (24)
  //wave2.sine(freq1);
  wave1.begin(freq1);
  wave2.begin(freq2);
  wave1.sync(wave2);
  rephase=0;
  time_rephase=millis();

   
   #ifdef USE_ADC_FIX_IN_SCIENCE_KIT_REV_3_0
   /* 20230707 keep resolution to 0-511 also if core now is correct
    and goes from 1-1023  */
   analogReadResolution(ANRES);
   #endif
}

/* -------------------------------------------------------------------------- */
void all_led_on() {
/* -------------------------------------------------------------------------- */  
   
   for(int i = 0; i < 6;i++) {
      range.set(i);
   }

   for(int i = 0; i < 18; i++) {
      if(i < 9) {
         frequency_1.set(i);
      }
      else if(i < 18) {
         frequency_2.set(i - 9);
      }
   }
}

/* -------------------------------------------------------------------------- */
void led_f1() {
/* -------------------------------------------------------------------------- */  
   range.set(1,1,1,0,0);
   frequency_2.set(1,1,1,0,0,0);
   for(int i = 0; i < 9; i++) {
       frequency_1.set(i);
   }
}

/* -------------------------------------------------------------------------- */
void led_f2() {
/* -------------------------------------------------------------------------- */  
   range.set(1,1,1,0,0);
   frequency_1.set(1,1,1,0,0,0);
   for(int i = 0; i < 9; i++) {
       frequency_2.set(i);
   }
}

/* -------------------------------------------------------------------------- */
void led_r() {
/* -------------------------------------------------------------------------- */  
   frequency_1.set(1,1,1,0,0,0);
   frequency_2.set(1,1,1,0,0,0);
   for(int i = 0; i < 6; i++) {
       range.set(i);
   }
}







/* _______________________________LOOP_______________________________________ */


/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */   
   #ifdef USE_DEBUG_CODE
   //Serial.println("Msin loop running");
   #endif
  

   #ifdef USE_TIMER
   AppSt_t st = at.getStatus();  
   
   if(st == APP_NORMAL) {
      CScheduler::getInstance().run();
   }
   else {
      if(st == APP_DIAGNOSTIC_LED_ON) {
        all_led_on();
      }
      else if(st == APP_DIAGNOSTIC_LED_ON_F1) {
        led_f1();
      }
      else if(st == APP_DIAGNOSTIC_LED_ON_F2) {
        led_f2();
      }
      else if(st == APP_DIAGNOSTIC_LED_ON_R) {
        led_r();
      }
      float f1 = at.getF1();
      if(f1 != 0.0) {
        wave1.freq(f1);
        at.resetF1();
      }
      float f2 = at.getF2();
      if(f2 != 0.0) {
        wave2.freq(f2);
        at.resetF2();
      }
      float a1 = at.getA1();
      if(a1 > 0.0) {
        wave1.amplitude(a1);
        at.resetA1();
      }
      float a2 = at.getA2();
      if(a2 > 0.0) {
        wave2.amplitude(a2);
        at.resetA2();
      }
      at.run();
   }
   #else
   

   frequency_1.refresh();
   frequency_2.refresh();
   range.update();

   freq1=frequency_1.getGaugeValue()*range.getRange1();
   freq2=frequency_2.getGaugeValue()*range.getRange2();

   phase1=analogRead(PH_1)/step;
   if (phase1>(RES+10)){
      phase1=0;
   }

   phase2=analogRead(PH_2)/step;
   if (phase2>RES){
      phase2=0;
   }

   if (freq1!=previous_frequency_1){
      wave1.freq(freq1);
      previous_frequency_1=freq1;
      wave1.sync(wave2);
   }

   if (freq2!=previous_frequency_2){
      wave2.freq(freq2);
      previous_frequency_2=freq2;
      wave2.sync(wave1);
   }

   if (millis()-time_rephase>500){
      if (phase1!=previous_phase_1){
         wave1.offset(phase1/2);
         previous_phase_1=phase1;
         if (phase2==0){
            wave1.sync(wave2);
         }
      }
      if (phase2!=previous_phase_2){
         wave2.offset(phase2/2);
         previous_phase_2=phase2;
         if (phase1==0){
            wave2.sync(wave1);
         }
      }
      time_rephase=millis();
   }

   delay(1);
   #endif

   

   
}
