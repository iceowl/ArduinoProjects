#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioSynthToneSweep      tonesweep1;     //xy=417,461
AudioSynthWaveformSine   sine1;          //xy=418,363
AudioSynthNoisePink      pink1;          //xy=429,503
AudioEffectFlange        flange1;        //xy=550,368
AudioMixer4              mixer1;         //xy=584,472
AudioOutputAnalog        dac1;           //xy=727,371
AudioConnection          patchCord1(tonesweep1, 0, mixer1, 1);
AudioConnection          patchCord2(sine1, flange1);
AudioConnection          patchCord3(pink1, 0, mixer1, 3);
AudioConnection          patchCord4(flange1, 0, mixer1, 0);
AudioConnection          patchCord5(mixer1, dac1);
// GUItool: end automatically generated code

