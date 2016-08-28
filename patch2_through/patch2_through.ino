

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>


#define CHORUS_DELAY_LENGTH (24*AUDIO_BLOCK_SAMPLES)


short l_delayline[CHORUS_DELAY_LENGTH];


// GUItool: begin automatically generated code
AudioInputAnalog         adc1;           //xy=195,345
AudioEffectChorus        chorus1;        //xy=446,344
AudioOutputAnalog        dac1;           //xy=733,339
AudioConnection          patchCord1(adc1, chorus1);
AudioConnection          patchCord2(chorus1, dac1);
// GUItool: end automatically generated code




void setup() {
  // put your setup code here, to run once:
  AudioMemory(12);
  chorus1.begin(l_delayline,CHORUS_DELAY_LENGTH,4);
}

void loop() {
  // put your main code here, to run repeatedly:

}
