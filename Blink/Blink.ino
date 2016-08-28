

/*
 * Blink
 *
 * The basic Arduino example.  Turns on an LED on for one second,
 * then off for one second, and so on...  We use pin 13 because,
 * depending on your Arduino board, it has either a built-in LED
 * or a built-in resistor so that you need only an LED.
 *
 * http://www.arduino.cc/en/Tutorial/Blink
 */
#include <Arduino.h>

int drivePin = 13;                // LED connected to digital pin 13

void setup()                    // run once, when the sketch starts
{
  pinMode(drivePin, OUTPUT);      // sets the digital pin as output
  digitalWrite(drivePin, LOW);
}

void loop()                     // run over and over again
{
  digitalWrite(drivePin, HIGH);   // sets the LED on
  delay(100);                  // waits for a second
  digitalWrite(drivePin, LOW);    // sets the LED off
  delay(1500);                  // waits for a second
}
