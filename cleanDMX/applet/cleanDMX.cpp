#include "WProgram.h"
void shiftDmxOut(int pin, int theByte);



/* DMX Shift Out for arduino - 004 and 005
 * -------------
 *
 * Shifts data in DMX format out to DMX enabled devices
 * it is extremely restrictive in terms of timing. Therefore
 * the program will stop the interrupts when sending data
 *
 * The elektronic foundation for DMX is RS 485, so you have to use  
 * a MAX-485 or a 75176.
 *
 * wirring for sending dmx with a MAX-485
 
    1 - RO  - Receiver Output --- set to ground with a 100 ohm resistor
    2 - RE  - Receiver Output Enable -- set to ground
    3 - DE  - Driver Output Enable -- set to 5v 
    4 - DI  - Driver Input -- Input from Arduino
    5 - GnD - Ground Connection -- set to ground -- refence for the DMX singal --- (DMX pin 1)
    6 - A   - Driver Output / Receiver Input -- DMX Signal (hot)------------------ (DMX pin 3)
    7 - B   - Driver Output / Receiver Input -- DMX Signal inversion ( cold)------ (DMX pin 2)
    8 - Vcc - Positive Supply -- 4,75V < Vcc < 5,25V
     
 * Every dmx packet contains 512 bytes of information (for 512 channels). 
 * The start of each packet is market by a start byte (shiftDmxOut(sig,0);), 
 * you should always send all 512 bytes even if you don*t use all 512 channels.
 * The time between every dmx packet is market by a break 
 * between 88us and 1s ( digitalWrite(sig, LOW); delay(10);)
 * 
 * (cleft) 2006 by Tomek Ness and D. Cuartielles
 * K3 - School of Arts and Communication
 * fhp - University of Applied Sciences
 * <http://www.arduino.cc>
 * <http://www.mah.se/k3>
 * <http://www.design.fh-potsdam.de>
 *
 * @date: 2006-09-30
 * @idea: Tomek Ness
 * @code: D. Cuartielles and Tomek Ness
 * @acknowledgements: Johny Lowgren for his DMX devices
 *
 */


int sig = 7;           

int count = 0;
int swing = 0;
int updown = 0;


/* Sends a DMX byte out on a pin.  Assumes a 16 MHz clock.
 * Disables interrupts, which will disrupt the millis() function if used
 * too frequently. */

void shiftDmxOut(int pin, int theByte)
{
    int wasteTime = 0;
    int theDelay = 1;
    int count = 0;

    //int portNumber = port_to_output[digital_pin_to_port[pin].port];
    //int pinNumber = digital_pin_to_port[pin].bit;
        
    // the first thing we do is to write te pin to high
    // it will be the mark between bytes. It may be also
    // high from before
    //    _SFR_BYTE(_SFR_IO8(portNumber)) |= _BV(pinNumber
    digitalWrite(sig,HIGH);
    delayMicroseconds(10);

    // disable interrupts, otherwise the timer 0 overflow interrupt that
    // tracks milliseconds will make us delay longer than we want.
    cli();

        // DMX starts with a start-bit that must always be zero
       // _SFR_BYTE(_SFR_IO8(portNumber)) &= ~_BV(pinNumber);
        //we need a delay of 4us (then one bit is transfert)
        // at the arduino just the delay for 1us is precise every thing between 2 and 12 is jsut luke
        // to get excatly 4us we have do delay 1us 4 times
        digitalWrite(sig,LOW);
        delayMicroseconds(theDelay);
        delayMicroseconds(theDelay);
        delayMicroseconds(theDelay);
        delayMicroseconds(theDelay);
    
         for (count = 0; count < 8; count++) {
    				
                if (theByte & 01) {
                  digitalWrite(pin,HIGH);
                }
		else {
                  digitalWrite(pin,LOW);
                }

              delayMicroseconds(theDelay);
              delayMicroseconds(theDelay);
              delayMicroseconds(theDelay);
              // to write every bit exactly 4 microseconds, we have to waste some time here.
              //thats why we are doing a for loop with nothing to do, a delayMicroseonds is not smal enough
              for (wasteTime =0; wasteTime <2; wasteTime++) {}
             
              
    		theByte>>=1;
        }
       
    // the last thing we do is to write the pin to high
    // it will be the mark between bytes. (this break is have to be between 8 us and 1 sec)
        digitalWrite(pin,HIGH);

    // reenable interrupts.
    sei();
}

  void setup() {
  pinMode(sig, OUTPUT);
  digitalWrite(13, HIGH);
}

void loop()  {
   
   // sending the break (the break can be between 88us and 1sec)
   digitalWrite(sig, LOW);
   delay(10);
    
   //sending the start byte
   shiftDmxOut(sig,0);
   
   shiftDmxOut(sig, 255); //1

   shiftDmxOut(sig, 0); //2
   shiftDmxOut(sig, 0); //3
   shiftDmxOut(sig, 0); //4
   shiftDmxOut(sig, 0); //5
   shiftDmxOut(sig, 0); //6
   shiftDmxOut(sig, 0); //7

   shiftDmxOut(sig, 0); //8

   for (count = 1; count<=504; count++){  //the rest
     shiftDmxOut(sig, 0);
   }
 }



int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

