#include <SPI.h>                              // SPI interface API
#include <Encoder.h>
#include <Wire.h>                             // Wire support library
#include "Qduino.h"
#include <Time.h>
#include "Adafruit_LEDBackpack.h"             // Support for the LED Backpack FeatherWing
#include "Adafruit_GFX.h"                     // Adafruit's graphics library

#define  DEFBRIGHT  4

Adafruit_7segment clockDisplay = Adafruit_7segment();
Adafruit_7segment matrix2      = Adafruit_7segment();
qduino            q;
fuelGauge         battery;
int               hours   = 0;                      // Track hours
int               minutes = 0;                    // Track minutes
int               seconds = 0;                    // Track seconds
boolean           blinkColon = false;            // Track the status of the colon to blink every second
boolean           ctlTurned  = false;
int               displayValue = 0;
int               dis2 = 0;
long              rnd_number1 = 0;
long              rnd_number2 = 0;





void setup() {

  Wire.begin();
  battery.setup();
  q.setup();
  

  randomSeed(analogRead(A0));

  clockDisplay.begin(0x70);
  matrix2.begin(0x71);
  clockDisplay.print(hours, DEC);
  matrix2.print(hours, DEC);
  clockDisplay.writeDisplay();
  matrix2.writeDisplay();
  clockDisplay.setBrightness(DEFBRIGHT);
  matrix2.setBrightness(DEFBRIGHT);
}


void loop() {

  int charge = battery.chargePercentage();
  battery.reset();

  if (charge >= 75) {

    q.setRGB(GREEN);

  } else if (charge >= 50 && charge < 75) {

    q.setRGB(YELLOW);

  } else if (charge >= 25 && charge < 50) {

    q.setRGB(ORANGE);

  } else if (charge < 25) {

    q.setRGB(RED);
  }
  /*
    matrix2.print(dis2++,DEC);
    if(dis2 < 1000) matrix2.writeDigitNum(0, 0);
    matrix2.writeDisplay();

    if(dis2 > 9999) {
     dis2 = 0;
     clockDisplay.print(++displayValue,DEC);

     clockDisplay.writeDisplay();
     if(displayValue >= 9999) displayValue = 0;
    }
    //delay(1);
  */
  //this stuff is for clocks

  writeRandoms();


  delay(random(100, 9999));
  q.ledOff();
}

void writeRandoms() {

  rnd_number1 = random(0, 9999);
  rnd_number2 = random(0, 9999);

  clockDisplay.print(rnd_number1, DEC);
  matrix2.print(rnd_number2, DEC);


  if (rnd_number1 < 1000) {
    clockDisplay.writeDigitNum(0, 0);
    if (rnd_number1 < 100) {
      clockDisplay.writeDigitNum(1, 0);
      if (rnd_number1 < 10) clockDisplay.writeDigitNum(2, 0);
    }
  }
  if (rnd_number2 < 1000) {
    matrix2.writeDigitNum(0, 0);
    if (rnd_number2 < 100) {
      matrix2.writeDigitNum(1, 0);
      if (rnd_number2 < 10) matrix2.writeDigitNum(2, 0);
    }
  }


  clockDisplay.writeDisplay();
  matrix2.writeDisplay();


}

void setDisplayValue() {

  displayValue = hours * 100 + minutes;
  clockDisplay.print(displayValue, DEC);
  if (hours < 10) {
    // Pad hour 0.
    clockDisplay.writeDigitNum(0, 0); // 0 = leftmost digit 4=rightmost digit despite what Adafruit docs say
    if (hours == 0) clockDisplay.writeDigitNum(1, 0);
    // Also pad when the 10's minute is 0 and should be padded.
    if (minutes < 10) {
      clockDisplay.writeDigitNum(3, 0);
    }

  }
  blinkColon = !blinkColon;
  clockDisplay.drawColon(blinkColon);
  clockDisplay.writeDisplay();
}
