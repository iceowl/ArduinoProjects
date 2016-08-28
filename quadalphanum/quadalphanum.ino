// Demo the quad alphanumeric display LED backpack kit
// scrolls through every character, then scrolls Serial
// input onto the display

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
//#include <JOBDictionary.h>
//#include <JOBDictionary4_0.h>
#include <JOBDictionary4_1_1.h>
//#include<JOBDictionary2.h>
#define MAXWORDS 10000 //10000
#define DICT_SIZE 5191 //5193

#define DEFAULTBRIGHTNESS 4


uint32_t wordNumber = 6000;

Adafruit_AlphaNum4 alpha2 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha3 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha5 = Adafruit_AlphaNum4();

void setup() {
 
  randomSeed(analogRead(A1));

  alpha2.begin(0x70);
  alpha3.begin(0x71);
  alpha4.begin(0x72);  // pass in the address
  alpha5.begin(0x73);


  alpha2.setBrightness(DEFAULTBRIGHTNESS);
  alpha3.setBrightness(DEFAULTBRIGHTNESS);
  alpha4.setBrightness(DEFAULTBRIGHTNESS);
  alpha5.setBrightness(DEFAULTBRIGHTNESS);

  clearDisplays();
 
}

char newWord[30] = "";
char displaybuffer[20] = "";
void loop() {


  //delay(random(10, 10000));

  
  delay(random(100,8000));
  int k = 0;

  
  wordNumber = random(0,9999);

  uint32_t idx = pgm_read_dword_far(&(dictionary0[wordNumber]));

  strcpy(newWord,(char*)idx);

  k = 0;

  addSpace();

  String myString = String(newWord);
  myString.toUpperCase();

  while (k++ < myString.length()) {

    rotateBuffer();
    displaybuffer[15] = myString.charAt(k - 1);

    // set every digit to the buffer

    alpha2.writeDigitAscii(0, displaybuffer[0]);
    alpha2.writeDigitAscii(1, displaybuffer[1]);
    alpha2.writeDigitAscii(2, displaybuffer[2]);
    alpha2.writeDigitAscii(3, displaybuffer[3]);
    alpha3.writeDigitAscii(0, displaybuffer[4]);
    alpha3.writeDigitAscii(1, displaybuffer[5]);
    alpha3.writeDigitAscii(2, displaybuffer[6]);
    alpha3.writeDigitAscii(3, displaybuffer[7]);
    alpha4.writeDigitAscii(0, displaybuffer[8]);
    alpha4.writeDigitAscii(1, displaybuffer[9]);
    alpha4.writeDigitAscii(2, displaybuffer[10]);
    alpha4.writeDigitAscii(3, displaybuffer[11]);
    alpha5.writeDigitAscii(0, displaybuffer[12]);
    alpha5.writeDigitAscii(1, displaybuffer[13]);
    alpha5.writeDigitAscii(2, displaybuffer[14]);
    alpha5.writeDigitAscii(3, displaybuffer[15]);

    // write it out!
    alpha2.writeDisplay();
    alpha3.writeDisplay();
    alpha4.writeDisplay();
    alpha5.writeDisplay();
    delay(150);
  }

}

void rotateBuffer() {
  for (int i = 0; i < 15; i++) {
    displaybuffer[i] = displaybuffer[i + 1];
  }
}
void addSpace() {

  rotateBuffer();
  displaybuffer[15] = ' ';
  rotateBuffer();

}
void clearDisplays() {

  alpha2.writeDigitRaw(0, 0xFFFF);
  alpha2.writeDisplay();
  delay(100);
  alpha2.writeDigitRaw(0, 0x0);
  alpha2.writeDigitRaw(1, 0xFFFF);
  alpha2.writeDisplay();
  delay(100);
  alpha2.writeDigitRaw(1, 0x0);
  alpha2.writeDigitRaw(2, 0xFFFF);
  alpha2.writeDisplay();
  delay(100);
  alpha2.writeDigitRaw(2, 0x0);
  alpha2.writeDigitRaw(3, 0xFFFF);
  alpha2.writeDisplay();
  delay(100);
  alpha2.writeDigitRaw(3, 0x0);
  alpha2.writeDisplay();
  alpha3.writeDigitRaw(0, 0xFFFF);
  alpha3.writeDisplay();
  delay(100);
  alpha3.writeDigitRaw(0, 0x0);
  alpha3.writeDigitRaw(1, 0xFFFF);
  alpha3.writeDisplay();
  delay(100);
  alpha3.writeDigitRaw(1, 0x0);
  alpha3.writeDigitRaw(2, 0xFFFF);
  alpha3.writeDisplay();
  delay(100);
  alpha3.writeDigitRaw(2, 0x0);
  alpha3.writeDigitRaw(3, 0xFFFF);
  alpha3.writeDisplay();
  delay(100);
  alpha3.writeDigitRaw(3, 0x0);
  alpha3.writeDisplay();
  alpha4.writeDigitRaw(0, 0xFFFF);
  alpha4.writeDisplay();
  delay(100);
  alpha4.writeDigitRaw(0, 0x0);
  alpha4.writeDigitRaw(1, 0xFFFF);
  alpha4.writeDisplay();
  delay(100);
  alpha4.writeDigitRaw(1, 0x0);
  alpha4.writeDigitRaw(2, 0xFFFF);
  alpha4.writeDisplay();
  delay(100);
  alpha4.writeDigitRaw(2, 0x0);
  alpha4.writeDigitRaw(3, 0xFFFF);
  alpha4.writeDisplay();
  delay(100);
  alpha4.writeDigitRaw(3, 0x0);
  alpha4.writeDisplay();
  alpha5.writeDigitRaw(0, 0xFFFF);
  alpha5.writeDisplay();
  delay(100);
  alpha5.writeDigitRaw(0, 0x0);
  alpha5.writeDigitRaw(1, 0xFFFF);
  alpha5.writeDisplay();
  delay(100);
  alpha5.writeDigitRaw(1, 0x0);
  alpha5.writeDigitRaw(2, 0xFFFF);
  alpha5.writeDisplay();
  delay(100);
  alpha5.writeDigitRaw(2, 0x0);
  alpha5.writeDigitRaw(3, 0xFFFF);
  alpha5.writeDisplay();
  delay(100);
  alpha5.writeDigitRaw(3, 0x0);
  alpha5.writeDisplay();
  delay(100);
  alpha2.clear();
  alpha2.writeDisplay();
  alpha3.clear();
  alpha3.writeDisplay();
  alpha4.clear();
  alpha4.writeDisplay();
  alpha5.clear();
  alpha5.writeDisplay();

}

