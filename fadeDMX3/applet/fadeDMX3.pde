/*
 * DMX fade for arduino 008
 * based on the code of Tomek Ness and D. Cuartielles
 *
 * adapted to arduino 008 by Peter Szakal and Gabor Papp
 * http://nextlab.hu
 */

#include "pins_arduino.h"

int sig = 13; // signal
int greenLevel  = 0;
int greenTarget = 0;
int g1 = 255;
int redLevel  = 0;
int redTarget = 0;
int r1 = 0;
int blueLevel  = 0;
int blueTarget = 0;
int b1 = 0;
int greenControl = 5;
int redControl = 4;
int blueControl = 3;
int lightState = 0;
long fadeTime = 8;
long crossFadeTime = 500;
long oldTime = 0;
boolean amOn = false;


/* Sends a DMX byte out on a pin.  Assumes a 16 MHz clock.
 * Disables interrupts, which will disrupt the millis() function if used
 * too frequently. */
void shiftDmxOut(int pin, int theByte)
{
  int port_to_output[] = {
    NOT_A_PORT,
    NOT_A_PORT,
    _SFR_IO_ADDR(PORTB),
    _SFR_IO_ADDR(PORTC),
    _SFR_IO_ADDR(PORTD)
    };

    int portNumber = port_to_output[digitalPinToPort(pin)];
  int pinMask    = digitalPinToBitMask(pin);

  // the first thing we do is to write te pin to high
  // it will be the mark between bytes. It may be also
  // high from before
  _SFR_BYTE(_SFR_IO8(portNumber)) |= pinMask;
  delayMicroseconds(10);

  // disable interrupts, otherwise the timer 0 overflow interrupt that
  // tracks milliseconds will make us delay longer than we want.
  cli();

  // DMX starts with a start-bit that must always be zero
  _SFR_BYTE(_SFR_IO8(portNumber)) &= ~pinMask;

  // we need a delay of 4us (then one bit is transfered)
  // this seems more stable then using delayMicroseconds
  asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
  asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");

  asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
  asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");

  asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
  asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");

  for (int i = 0; i < 8; i++)
  {
    if (theByte & 01)
    {
      _SFR_BYTE(_SFR_IO8(portNumber)) |= pinMask;
    }
    else
    {
      _SFR_BYTE(_SFR_IO8(portNumber)) &= ~pinMask;
    }

    asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
    asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");

    asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
    asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");

    asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
    asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");

    theByte >>= 1;
  }

  // the last thing we do is to write the pin to high
  // it will be the mark between bytes. (this break is have to be between 8 us and 1 sec)
  _SFR_BYTE(_SFR_IO8(portNumber)) |= pinMask;

  // reenable interrupts.
  sei();
}

void fadeUPLights(){

  if(greenLevel < greenTarget){
    greenLevel++;
  }

  if(redLevel < redTarget){
    redLevel++;
  }

  if(blueLevel < blueTarget){
    blueLevel++;
  }

  if(blueLevel == blueTarget && greenLevel == greenTarget && redLevel == redTarget){
    amOn = true;
  }

}

void setup()
{
  pinMode(sig, OUTPUT);
  pinMode(greenControl,INPUT);
  pinMode(redControl,INPUT);
  pinMode(blueControl,INPUT);
  oldTime = millis();
  greenTarget = (analogRead(greenControl) - 1) / 4;
  blueTarget = (analogRead(blueControl) - 1)/ 4;
  redTarget = (analogRead(redControl) - 1) / 4; 


}

void crossFade(){


  if((millis() - oldTime) < crossFadeTime){
    b1;
  } 
  else {
    oldTime = millis();

    if( b1 <= 255 && lightState == 0){
      b1++;
      g1--;
    }
    if(b1 == 255){
      lightState = 1;
    }
    if( r1 <= 255 && lightState == 1){
      r1++;
      b1--;
    }
    if(r1 == 255){
      lightState = 2;
    }
    if( g1 <= 255 && lightState == 2){
      r1--;
      g1++;
    }
    if(g1 == 255){
      lightState = 0;
    }
  }
}

void loop()
{

  /***** sending the dmx signal *****/

  // sending the break (the break can be between 88us and 1sec)
  digitalWrite(sig, LOW);

  delay(10);

  // sending the start byte
  shiftDmxOut(sig, 0);



  for (int count = 1; count <= 4; count++)   {

    if(count == 1) 
    {
      shiftDmxOut(sig, greenLevel);
    } 
    else if(count == 2)
    {
      shiftDmxOut(sig, redLevel);
    } 
    else if(count == 3)
    {
      shiftDmxOut(sig, blueLevel);
    }  
    else if(count > 3)
    {
      shiftDmxOut(sig,0);
    }


  }

  if(!amOn){
    if((millis() - oldTime) > fadeTime){
      fadeUPLights();
      oldTime = millis();
    }
  } 
  else {

    greenLevel = (analogRead(greenControl) - 1) / 4;
    blueLevel = (analogRead(blueControl) - 1)/ 4;
    redLevel = (analogRead(redControl) - 1) / 4; 
    if(greenLevel == 0 && redLevel == 0 && blueLevel == 0){
      greenLevel = g1;
      redLevel = r1;
      blueLevel = b1;
      crossFade();
    }

  }


}

