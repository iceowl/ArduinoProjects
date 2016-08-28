#include "WProgram.h"
/* Ultrasound Sensor
 *------------------
 *
 * Reads values (00014-01199) from an ultrasound sensor (3m sensor)
 * and writes the values to the serialport.
 *
 * http://www.xlab.se | http://www.0j0.org
 * copyleft 2005 Mackie for XLAB | DojoDave for DojoCorp
 *
 */

void setup();
void loop();
void flashWarning();
void addDataPoint();
int ultraSoundSignal = 11; // Ultrasound signal pin
int val = 0;
//int ultrasoundValue = 0;
int timecount = 0; // Echo counter
int timesum = 0;
int threshold = 100;
int ledAlive = 13;
int ledRedPin = 12; // LED connected to digital pin 11
int ledGrnPin = 7;
int trip = 0;
int times[25];
int datapoints = 0;
boolean FLASH = false;
unsigned long flashCount;

void setup() {
  //beginSerial(9600);                  // Sets the baud rate to 9600
  pinMode(ledRedPin, OUTPUT);            // Sets the digital pin as output
  pinMode(ledGrnPin, OUTPUT);
  //pinMode(ledAlive,OUTPUT);
  //pinMode(ultraSoundSignal, OUTPUT);
  flashCount = millis();

}

void loop() {

  unsigned long time2 = millis();
  timecount = 0;
  val = 0;
  //digitalWrite(ledAlive,HIGH);
  pinMode(ultraSoundSignal, OUTPUT); // Switch signalpin to output

  /* Send low-high-low pulse to activate the trigger pulse of the sensor
   * -------------------------------------------------------------------
   */

  digitalWrite(ultraSoundSignal, LOW); // Send low pulse
  delayMicroseconds(2); // Wait for 2 microseconds
  digitalWrite(ultraSoundSignal, HIGH); // Send high pulse
  delayMicroseconds(5); // Wait for 5 microseconds
  digitalWrite(ultraSoundSignal, LOW); // Holdoff

  /* Listening for echo pulse
   * -------------------------------------------------------------------
   */

  pinMode(ultraSoundSignal, INPUT); // Switch signalpin to input
  val = digitalRead(ultraSoundSignal); // Append signal value to val
  while(val == LOW) { // Loop until pin reads a high value
    val = digitalRead(ultraSoundSignal);
  }

  while(val == HIGH) { // Loop until pin reads a high value
    val = digitalRead(ultraSoundSignal);
    timecount = timecount +1;            // Count echo pulse time
  }

  /* Writing out values to the serial port
   * -------------------------------------------------------------------
   */
  addDataPoint();
  if(datapoints >= 24) {

    //ultrasoundValue = timecount; // Append echo pulse time to ultrasoundValue

    //serialWrite('A'); // Example identifier for the sensor
    //printInteger(ultrasoundValue);
    //serialWrite(10);
    //serialWrite(13);

    /* Lite up LED if any value is passed by the echo pulse
     * -------------------------------------------------------------------
     */


    if(timesum < 1800){
      digitalWrite(ledRedPin, HIGH);
      digitalWrite(ledGrnPin, LOW);
    } 
   //   else if(timesum > 1800 && timesum < 1950) {
   //     flashWarning();
   //   } 
    else if(timesum > 2000){
      digitalWrite(ledGrnPin, HIGH);
      digitalWrite(ledRedPin, LOW);
    }
  }

  /* Delay of program
   * -------------------------------------------------------------------
   */

  delay(100);
} 
void flashWarning() {
  unsigned long time = millis();
  if(time - flashCount > 400) {
    if(FLASH){
      digitalWrite(ledRedPin,HIGH);
      digitalWrite(ledGrnPin,LOW);
      FLASH = false;
    } 
    else {
      digitalWrite(ledRedPin,LOW);
      digitalWrite(ledGrnPin,HIGH);
      FLASH = true;
    }
    flashCount = time;
  }
}

void addDataPoint(){
  int x = 1;
  int y = 0;
  long datasum = 0;
  if(datapoints == 0){
    while(y < 25) {
      times[y] = 0;
      y++;
    }
  }

  while(x < 25) {
    times[x-1] = times[x];
    datasum += times[x-1];
    x++;
  }
  datasum += timecount;
  times[24] = timecount;
  timesum = datasum / 25;
  datapoints++;
  if(datapoints > 24){
    datapoints = 24;
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

