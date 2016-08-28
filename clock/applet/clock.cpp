#include "WProgram.h"
void setup();
void loop();
int ledPin        = 13;
int inByte        = 0;
long blinkTimer   = 0;
int blinkInterval = 1000;
int seconds       = 0;
int secondsT      = 0;
int minutes       = 0;
int hours         = 0; 
char lf           = 10;
boolean go        = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if(Serial.available() > 0)  {
    inByte = Serial.read();
    if(inByte == 65){
      go = true;
    }
  }
  if(millis() - blinkTimer >= blinkInterval/2){
    digitalWrite(ledPin,HIGH);
  }
  if(millis() - blinkTimer >= blinkInterval){
    digitalWrite(ledPin,LOW);
    blinkTimer = millis();
    seconds = (millis() / blinkInterval) - (secondsT*60);
    if (seconds > 59) {
      secondsT++;
      seconds=0;
      minutes++;
      if(minutes > 59) {
        minutes = 0;
        hours++;
      }
    }
    if (hours < 10) {
      Serial.print("0");
      Serial.print(hours);
    } else {
      Serial.print(hours);
    }
    Serial.print(":");
    if(minutes < 10) {
      Serial.print("0");
      Serial.print(minutes);
    } else {
      Serial.print(minutes);
    }
    Serial.print(":");
    if(seconds < 10) {
      Serial.print("0");
      Serial.print(seconds);
    } else {
    Serial.print(seconds);
    }
    Serial.print(lf);
    go = false;

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

