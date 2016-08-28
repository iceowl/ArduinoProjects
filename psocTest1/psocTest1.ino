/******************************************************************************
Example.ino

Simple example demonstrating the use of Serial ports and extra pins on the
FreeSoC2 board within the Arduino IDE.

14 May 2015

Developed/Tested with:
FreeSoC2
Arduino.cc IDE 1.6.4
This code is beerware; if you see me (or any other SparkFun employee) at the 
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given. 
******************************************************************************/
// Note the use of long here instead of int! This is important.
const long LEDPin = 13; // As on most Arduino compatible boards, there's an LED
                       //  tied to pin 13. pinMode() works the same with the
                       //  FreeSoC2 as it does normally.
const long buttonPin = P1_D2; // Pins can also be referred to by port number
                       //  and pin number. These numbers are given on the board
                       //  as P<port>.<pin>. P1_D2 is connected to the user
                       //  button on one side ang ground on the other.

void setup() 
{
  // pinMode() functions are unchanged.
  pinMode(LEDPin, OUTPUT); 
  // The onboard user button needs to be a pullup-enabled input.
  pinMode(buttonPin, INPUT_PULLUP); 
  Serial.begin(9600);  // This is the same as it is on the Arduino Leonardo.
                       //  It represents the logical serial port connection to
                       //  the PC.
  Serial1.begin(9600); // Again, same as the Leonardo. This port is the physical
                       //  IO pins (0 and 1) on the headers. It can be accessed
                       //  via the KitProg serial port which is available when
                       //  the Debugger port is connected.
}

void loop() 
{
  // digitalRead() is unchanged.
  int pinDown = digitalRead(buttonPin);

  // Check for button pressed...
  if (pinDown == LOW)
  {
    Serial1.println("KitProg! Button pressed!");
    Serial.println("USB! Button pressed!");
    digitalWrite(LEDPin, HIGH);
  }
  else
  {
    digitalWrite(LEDPin, LOW);
  }

  delay(250);
}

