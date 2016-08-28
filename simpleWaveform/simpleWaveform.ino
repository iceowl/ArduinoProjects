
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=196,341
AudioOutputAnalog        dac1;           //xy=613,345
AudioConnection          patchCord1(waveform1, dac1);
// GUItool: end automatically generated code





void setup() {
  // put your setup code here, to run once:
AudioMemory(5);
waveform1.begin(1.0,440,WAVEFORM_SINE);
}

void loop() {
  // put your main code here, to run repeatedly:

}
