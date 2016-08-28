


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define FLANGE_DELAY_LENGTH (12*AUDIO_BLOCK_SAMPLES)

int s_idx = FLANGE_DELAY_LENGTH/4;
int s_depth = FLANGE_DELAY_LENGTH/4;
short l_delayline[FLANGE_DELAY_LENGTH];
double s_freq = 3.0;
int freq_hi = 1220;
int freq_low = 440;



AudioSynthToneSweep      tonesweep1;     //xy=390,372
AudioEffectFlange        flange1;        //xy=550,368
AudioOutputAnalog        dac1;           //xy=727,371
//AudioSynthWaveform       waveform1;      //xy=433,386
AudioConnection          patchCord1(tonesweep1, flange1);
AudioConnection          patchCord2(flange1, dac1);


////AudioOutputAnalog        dac1;           //xy=727,371



void setup() {
/**/
AudioMemory(12);
flange1.begin(l_delayline,FLANGE_DELAY_LENGTH,s_idx,s_depth,s_freq);
// GUItool: begin automatically generated code

//waveform1.begin(1.0,440.0,WAVEFORM_SINE);


// GUItool: begin automatically generated code


//
  // Initially the effect is off. It is switched on when the
  // PASSTHRU button is pushed.
  
  }

void loop() {

  tonesweep1.play(1.0,freq_hi,freq_low,2.0);
  while(tonesweep1.isPlaying());
  tonesweep1.play(1.0,freq_low,freq_hi,2.0);
  while(tonesweep1.isPlaying());
 
  }
