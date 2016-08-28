 /* Ultrasound Sensor
 *------------------
 *
 * Reads values (00014-01199) from an ultrasound sensor (3m sensor)
 * and writes the values to the serialport.
 *
 * http://www.xlab.se | http://www.0j0.org
 * copyleft 2005 Mackie for XLAB | DojoDave for DojoCorp
 *
 * 2007-09: Modified by RJL (robertlucier@yahoo.com)
 * to detect the ultrasound return pulse via INT1
 *
 */

   1. define TIMER_US_PER_TICK 4 // 16MHz / 64 cycles per tick
   2. define TIMER_OVERFLOW_US TIMER_US_PER_TICK * 65536 // timer1 is 16bit
   3. define INCH_PER_SEC 13526 // speed of sound 

// use pin 3 because it is connected to external interrupt 1 (INT1) int ultraSoundSignal = 3; // Ultrasound signal pinint val = 0;

int ledPin = 13; // LED connected to digital pin 13 int count = 0;

// these are volatile because they are changed in interrupt handlers // otherwise the compiler will assume they are unchanged and replace with constants. volatile long ofStart = 0; volatile long timerStart = 0; volatile long ultrasoundValue = 0; volatile int timer1_overflow = 0;

ISR(SIG_OVERFLOW1) {

  // keep track of timer1 overflows.  
  // these should happen every TIMER_OVERFLOW_US microseconds (approx 4 per second at 16MHz clockspeed)
  timer1_overflow += 1;

};

void ultrasound_response() { /* Listening for echo pulse

 * -------------------------------------------------------------------
 * response should come withing ~.5ms (6") and 10ms(20")
 * handles falling pin interrupt signal
 */
 if (ofStart != -1) {
   // check ofStart and return if a new ping has started.
   ultrasoundValue = diffTime();
 }

}

void markTime() {

 // !!! even though interrupts are disabled, this calculation is still error prone
 // if the TCNT1 rolls over, but timer1_overflow isn't incremented.  
 cli(); // disable global interrupts
 timerStart = TCNT1;  
 ofStart = timer1_overflow;
 sei(); // enable global interrupts

}

long diffTime() {

  // return diff in microsec between now and the last mark
  long us;
  us = (timer1_overflow - ofStart) * 65536;
  us += (TCNT1 - timerStart);
  us *= TIMER_US_PER_TICK;
  return us;

}

long microsecs() {

  // for debugging
  long us;
  us = timer1_overflow * 65536;
  us += TCNT1;
  us *= TIMER_US_PER_TICK;
  return us;

}

void setup() {

  beginSerial(9600);                  // Sets the baud rate to 9600
  pinMode(ledPin, OUTPUT);            // Sets the digital pin as output
  attachInterrupt(1, ultrasound_response, FALLING);
  TCNT1 = 0; // reset timer 1
  TCCR1A = 0x00;          // COM1A1=0, COM1A0=0 => Disconnect Pin OC1 from Timer/Counter 1 -- PWM11=0,
                          // PWM10=0 => PWM Operation disabled                                                                      
  TCCR1B = 0x03; // clock/64 = increments every 3.2us  - overflows in 209.7mS  
  TIMSK1 |= _BV(0);   // timer1 overflow interrupt enable TOIE1=1. atmel_doc2545 p. 135      

}

void triggerPulse() {

  /* Send low-high-low pulse to activate the trigger pulse of the sensor
   * -------------------------------------------------------------------
   */
  int val;
  ultrasoundValue = -1; // reset any previous value
  ofStart = -1; // indicator to INT1 interrupt to ignore this LOW
  pinMode(ultraSoundSignal, OUTPUT); // Switch signalpin to output
  digitalWrite(ultraSoundSignal, LOW); // Send low pulse
  delayMicroseconds(2); // Wait for 2 microseconds
  digitalWrite(ultraSoundSignal, HIGH); // Send high pulse
  delayMicroseconds(5); // Wait for 5 microseconds
  digitalWrite(ultraSoundSignal, LOW); // Holdoff  
  Serial.print("!");

  // this could also be handled in an interrupt
  pinMode(ultraSoundSignal, INPUT); // Switch signalpin to output
  // poll until ultrasound sensor goes low
  val = digitalRead(ultraSoundSignal); // Append signal value to val
    while(val == LOW) { // Loop until pin reads a high value
    val = digitalRead(ultraSoundSignal);
  }
  markTime();
  Serial.print("_");

}

void printValue() {

  long in;
  long ft;
  // divide by 1000000 here in three steps of 100
  in = ((ultrasoundValue/100) * (INCH_PER_SEC/100))/100;
  in /= 2; // total elapsed time is there and back - divide by two for one way
  ft = in / 12;
  in = in % 12;
  Serial.print("\n");
  Serial.print('A'); // Example identifier for the sensor
  Serial.print(ultrasoundValue);
  Serial.print("\t");
  Serial.print(ft);
  Serial.print("'\t");
  Serial.print(in);
  Serial.print("\"");
  Serial.print("\n");
  serialFlush();

}

void loop() {

  printValue();
  triggerPulse();
  Serial.print(".");
  delay(100);

} 
