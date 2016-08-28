/* Encoder Library - NoInterrupts Example
   http://www.pjrc.com/teensy/td_libs_Encoder.html

   This example code is in the public domain.
*/

// If you define ENCODER_DO_NOT_USE_INTERRUPTS *before* including
// Encoder, the library will never use interrupts.  This is mainly
// useful to reduce the size of the library when you are using it
// with pins that do not support interrupts.  Without interrupts,
// your program must call the read() function rapidly, or risk
// missing changes in position.
#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#define SW 4
// Beware of Serial.print() speed.  Without interrupts, if you
// transmit too much data with Serial.print() it can slow your
// reading from Encoder.  Arduino 1.0 has improved transmit code.
// Using the fastest baud rate also helps.  Teensy has USB packet
// buffering.  But all boards can experience problems if you print
// too much and fill up buffers.

// Change these two numbers to the pins connected to your encoder.
//   With ENCODER_DO_NOT_USE_INTERRUPTS, no interrupts are ever
//   used, even if the pin has interrupt capability
Encoder myEnc(5, 6);
//   avoid using pins with LEDs attached

boolean          sw_on     = false;
enum             enc_state {up, down, unknown};
long             position  = -999;
boolean          debounce  = false;
unsigned long    theTime   = 0;
enc_state state            = unknown;



void setup() {
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH); // set pullup
  Serial.begin(9600);
  Serial.println("Basic NoInterrupts Test:");
}



void loop() {
  long newPos = myEnc.read();
  boolean SW_t = digitalRead(SW);
  if (newPos != position) {
    if (newPos > position) {
      if (state == up && (millis() > theTime + 250) ) {
        Serial.println("up");
        theTime = millis();
      }
      state = up;
    }
    else if ( newPos < position) {
      if (state == down && (millis() > theTime + 250)) {
        Serial.println("down");
        theTime = millis();
      }
      state = down;
    }
  }
  position = newPos;
  if (SW_t) {
    if (!sw_on) {
      sw_on = true;
      Serial.println("SW off");
    }
  } else {
    if (sw_on) {
      Serial.println("SW on");
      sw_on = false;
    }
  }
  delay(10);
}
