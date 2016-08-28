#include "WProgram.h"
void setup();
void loop();
int ledPin        = 8;
int motionPin     = 5;
int val         = 0;
int redLED        = 13;
long time = 0;
long blink = 250;
long tripTime = 1000;
boolean ledBlink;
long trip;

void setup() {
  Serial.begin(19200);
  pinMode(ledPin, OUTPUT);
  pinMode(motionPin, INPUT);
  pinMode(redLED, OUTPUT);
  time = millis();
  ledBlink = false;
  trip = millis();
  digitalWrite(redLED,LOW);
}

void loop() {

   long curTime;
   curTime = millis();
   if(curTime - time > blink){
     if(ledBlink){
       digitalWrite(redLED, LOW);
       ledBlink = false;
     } else {
       digitalWrite(redLED, HIGH);
       ledBlink = true;
     }
     time = millis();
   }
   
   val = analogRead(motionPin);
   Serial.println(val);
   if(val < 11) {
     digitalWrite(ledPin,HIGH);
     trip = millis();
   } else {
     if((curTime - trip) > tripTime) {
       digitalWrite(ledPin,LOW);
     }
   }
   
   delay(200);
  
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

