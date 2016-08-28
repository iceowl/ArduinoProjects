
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
float phase = 0.0;
float twopi = 3.14159 * 2;
elapsedMicros usec = 0;
unsigned long freqe = 500;

void setup() {
  analogWriteResolution(12);
  AudioNoInterrupts();
}

void loop() {
  float val = sin(phase) * 2000.0 + 2050.0;
  analogWrite(A14, (int)val);
  phase = phase + 0.02;
  if (phase >= twopi) phase = 0;
  //while (usec < freqe) ; // wait
  //usec = usec - freqe;
}

