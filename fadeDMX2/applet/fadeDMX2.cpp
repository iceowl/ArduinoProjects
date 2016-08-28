#include "WProgram.h"
void shiftDmxOut(int pin, int theByte);
void fadeLights();
#include "pins_arduino.h"
/*
 * DMX fade for arduino 008
 * based on the code of Tomek Ness and D. Cuartielles
 *
 * adapted to arduino 008 by Peter Szakal and Gabor Papp
 * http://nextlab.hu
 */

int sig = 5; // signal
int c1 = 0;
int c2 = 0;
int c3 = 255;
int lightState = 0;
long fadeTime = 10;
long oldTime = 0;

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

void fadeLights(){

  if( c1 <= 255 && lightState == 0){
    c1++;
    c3--;
  }
  if(c1 == 255){
    lightState = 1;
  }
  if( c2 <= 255 && lightState == 1){
    c2++;
    c1--;
  }
  if(c2 == 255){
    lightState = 2;
  }
  if( c3 <= 255 && lightState == 2){
    c2--;
    c3++;
  }
  if(c3 == 255){
    lightState = 0;
  }
}

void setup()
{
  pinMode(sig, OUTPUT);
  oldTime = millis();
}

void loop()
{

  /***** sending the dmx signal *****/

  // sending the break (the break can be between 88us and 1sec)
  digitalWrite(sig, LOW);

  delay(10);

  // sending the start byte
  shiftDmxOut(sig, 0);
  
  // the original says send all 512 channels but for the Quad Puck we only need to send our 4

  for (int count = 1; count <= 4; count++)   {

    if(count == 1) 
    {
      shiftDmxOut(sig, c1);
    } 
    else if(count == 2)
    {
      shiftDmxOut(sig, c2);
    } 
    else if(count == 3)
    {
      shiftDmxOut(sig, c3);
    }  
    else if(count > 3)
    {
      shiftDmxOut(sig,0);
    }


  }
  if((millis() - oldTime) > fadeTime){
    fadeLights();
    oldTime = millis();
  }

}
/***** sending the dmx signal end *****/

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

