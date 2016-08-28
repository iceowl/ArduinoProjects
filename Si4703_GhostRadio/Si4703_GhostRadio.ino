#include <Si4703_Breakout.h>
#include <Wire.h>
#include<pins_arduino.h>
#define FREQSPACING 30
#define FMFREQSTART 875
#define FMFREQEND 1080
#define MINFREQS 4
#define RESETPIN 7
#define SDIO A4
#define SCLK A5
#define LEDPIN 13
#define MAXFREQS 30
#define SCANHIPIN 3
#define SCANSPEEDPIN A0
#define SCANLOWPIN 5
#define SCANMODEPIN 4
//#include <CapSense.h>
//#define GRAVINTERVAL 250000
//#define CHARGEBREAK  2000000


int freqs[MAXFREQS];
int freqCount = 0;
int thisFreq = 0;
int scanFreq = FMFREQSTART;
unsigned char doScan = LOW;
int scanSpeed = 100;


Si4703_Breakout radio(RESETPIN, SDIO, SCLK);
//int channel = 0;
int volume = 16;
//char rdsBuffer[10];

void setup()
{

  //randomSeed(analogRead(3));
  //cs7_6.set_CS_Autocal_Millis(0xFFFFFFFF);
  radio.powerOn();
  radio.setVolume(volume);
  pinMode(SCANHIPIN,OUTPUT);
  pinMode(SCANLOWPIN,OUTPUT);
  pinMode(SCANMODEPIN,INPUT);
  digitalWrite(SCANHIPIN,HIGH);
  digitalWrite(SCANLOWPIN,LOW);
  
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN,HIGH);
  findFrequencies();

  if(freqCount < 4) {
    doScan = HIGH;
    thisFreq = FMFREQSTART;
  } 
  else {
    doScan = LOW;
  }

  radio.setChannel(FMFREQSTART,HIGH);
}

void loop()
{


  readScanSpeed();
  digitalWrite(LEDPIN,HIGH);
  if(doScan) {
    scanFreq += 30;
    if(scanFreq > FMFREQEND) {scanFreq = FMFREQSTART;}
    radio.setChannel(scanFreq,HIGH);
  } 
  else {
    if(thisFreq >= freqCount) {
      thisFreq=0;
    }
    radio.setChannel(freqs[thisFreq],HIGH);
    thisFreq++;
  }

  digitalWrite(LEDPIN,LOW);
  delay(scanSpeed);

}


void findFrequencies() {

  int validFreq = FMFREQSTART;
  int fp = FMFREQSTART;
  freqCount = 0;
  radio.setChannel(FMFREQSTART,HIGH);
  while(validFreq >= fp && freqCount < 30) {
    fp = validFreq;
    digitalWrite(LEDPIN,LOW);
    validFreq = radio.seekUp();
    delay(100);
    if(validFreq && validFreq >=fp) {
      digitalWrite(LEDPIN,HIGH);
      delay(100);
      freqs[freqCount] = validFreq;
      freqCount++;
      //   Serial.print("found frequency: ");
      //    Serial.println(f);
    }
  }
  if(freqCount > 0)
    freqCount--;
  // Serial.print("total freqs ");
  //  Serial.println(freqCount);
}

void readScanSpeed(){
  
  scanSpeed = analogRead(SCANSPEEDPIN);
  scanSpeed = map(scanSpeed,0,1023,50,1023);
  if(digitalRead(SCANMODEPIN)) {doScan=HIGH;}
  else {doScan=LOW;}
}




