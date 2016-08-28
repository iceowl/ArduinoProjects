
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define CHORUS_DELAY_LENGTH (12*AUDIO_BLOCK_SAMPLES)


short l_delayline[CHORUS_DELAY_LENGTH];
// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=131,429
AudioSynthWaveform       waveform1;      //xy=196,341
AudioEffectDelay         delay1;         //xy=276,423
AudioMixer4              mixer1;         //xy=436,447
AudioEffectChorus        chorus1;        //xy=446,344
AudioOutputAnalog        dac1;           //xy=733,339
AudioConnection          patchCord1(waveform2, delay1);
AudioConnection          patchCord2(waveform1, chorus1);
AudioConnection          patchCord3(delay1, 1, mixer1, 1);
AudioConnection          patchCord4(mixer1, dac1);
AudioConnection          patchCord5(chorus1, 0, mixer1, 0);
// GUItool: end automatically generated code


void setup() {

  AudioMemory(14);
  delay1.delay(1,12);
  waveform1.begin(1.0,441,WAVEFORM_SINE);
  waveform2.begin(1.0,443,WAVEFORM_SINE);
chorus1.begin(l_delayline,CHORUS_DELAY_LENGTH,6);
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
