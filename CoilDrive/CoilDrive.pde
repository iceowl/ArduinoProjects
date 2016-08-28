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

int drivePin = A0;                // LED connected to digital pin 13

void setup()                    // run once, when the sketch starts
{
  pinMode(drivePin, OUTPUT);      // sets the digital pin as output
  analogWrite(drivePin, 0);
}

void loop()                     // run over and over again
{
  analogWrite(drivePin, 255);   // sets the LED on
  delayMicroseconds(100);                  // waits for a second
  analogWrite(drivePin, 0);    // sets the LED off
  delay(8);                  // waits for a second
}
