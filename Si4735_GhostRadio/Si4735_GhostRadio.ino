
#define SI4735_NOI2C 1
#define LEDPIN  7
//#define HIGHPIN 2
#define SCANPIN 5
#define EMFPROBE A0
#define IRPROBE  A1
#define EMFBIASPIN A3
#define FREQSPACING 30
#define VOLGND 3
#define VOLPWR 4
#define VOLVOL A2
//Due to a bug in Arduino, these need to be included here too/first
#include <SPI.h>
#include <Wire.h>
#include <Si4735.h>
#include <BlinkM_funcs.h>
//#include <CapacitiveSensor.h>

//Create an instance of the Si4735 named radio.

#define EMFSENSE 400 //raise to decrease sensitivity up to 1023
#define IRSENSE  1023
#define COLORSENSE 500
#define VOLSENSE 1000
#define FADESPEED 10
#define FMFREQSTART 8810
#define FMFREQEND 10780
#define MINFREQS 4


byte volume = 63;

Si4735 radio;
byte blinkm_addr = 0x09;
int irc, emfc;
word freqs[30];
int  freqCount = 0;
int thisFreq = 0;
int scanFreq = FMFREQSTART;
unsigned char doScan = LOW;
int sPin = 0;
int emfBias = 1023;

void setup()
{

  //randomSeed(analogRead(3));
  Serial.begin(9600);
  radio.begin(SI4735_MODE_FM);
  //radio.Mute();

  while(radio.volumeUp()){
   } // set to max vol on startup
 // radio.setVolume(volume);
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN,LOW);
 // pinMode(EMFPROBE,INPUT);
  pinMode(IRPROBE,INPUT);
  pinMode(VOLGND,OUTPUT);
  pinMode(VOLPWR,OUTPUT);
  pinMode(VOLVOL,INPUT);
  pinMode(SCANPIN,INPUT);
 // pinMode(HIGHPIN,OUTPUT);
  pinMode(EMFBIASPIN,INPUT);
//  digitalWrite(HIGHPIN,HIGH);
  digitalWrite(VOLPWR,HIGH);
  digitalWrite(VOLGND,LOW);
  BlinkM_begin();
  delay(100);
  BlinkM_off(0);
  lookForBlinkM();
  BlinkM_setFadeSpeed(blinkm_addr,FADESPEED);
  sPin = digitalRead(SCANPIN);
  if(sPin != LOW) findFrequencies();
  if((freqCount < MINFREQS) || sPin == LOW)doScan = HIGH;
 
}

void loop()
{


  readSetColor();
  checkVolume();
  digitalWrite(LEDPIN,HIGH);

  if(doScan){
    if(scanFreq >=FMFREQEND){
      scanFreq = FMFREQSTART;
    } 
    else {
      scanFreq += 20;
      radio.setFrequency(scanFreq);
    }
  }
  else {
    if(thisFreq>=freqCount){
      thisFreq = 0;
    }
    else{
      thisFreq++;
    }
    radio.setFrequency(freqs[thisFreq]);
  }
  //radio.seekUp();
  // BlinkM_fadeToRGB( blinkm_addr, irColor,emfColor,0);

  digitalWrite(LEDPIN,LOW);

  if(emfc>irc)
    delay(emfc/10);
  else delay(irc/10);

}


void readSetColor(){

  int emf = analogRead(EMFPROBE);
  int ir  = analogRead(IRPROBE);

  int emfColor = constrain(emf,0,1023);
  byte emfColorb = map(emfColor,50,emfBias,0,255);
  int irColor = constrain(ir,0,COLORSENSE);
  byte irColorb = map(irColor,COLORSENSE,0,0,127);

  
  emfc = map(emfColor,0,emfBias,5,750);
  irc = constrain(ir,0,IRSENSE);
  irc = map(irc,0,IRSENSE,5,750);
  Serial.print(emf);
  Serial.print(" ");
  Serial.println(emfColorb);
  BlinkM_fadeToHSB(blinkm_addr,irColorb,255,emfColorb);
  //  BlinkM_fadeToRGB(blinkm_addr,capColorb,irColorb,emfColorb);
}
void lookForBlinkM() {
  int a = BlinkM_findFirstI2CDevice();
  if( a == -1 ) {
    //Serial.println("No I2C devices found");
  } 
  else { 
    //  Serial.print("Device found at addr ");
    //  Serial.println( a, DEC);
    blinkm_addr = a;
  }
}

void findFrequencies() {

  bool valid = LOW;
  word f = FMFREQSTART;
  word fp = FMFREQSTART;
  freqCount = 0;
  radio.setFrequency(FMFREQSTART);
  checkVolume();
  while(f >= fp && freqCount < 30) {
    checkVolume();
    if(valid)
      fp = f;
    digitalWrite(LEDPIN,LOW);
    radio.seekUp(true);
    delay(500);
    digitalWrite(LEDPIN,HIGH);
    f = radio.getFrequency(&valid);
    if(valid && f >=fp) {
      delay(100);
      freqs[freqCount] = f;
      freqCount++;
      //   Serial.print("found frequency: ");
      //    Serial.println(f);
      for( int i=0;i<2;i++) {
        BlinkM_setRGB( blinkm_addr, 0x01,0x01,0x01 );
        delay(100);
        BlinkM_setRGB( blinkm_addr, 0x00,0x00,0x00 );
        delay(100);
      }
    }
  }
  if(freqCount > 0)
    freqCount--;
 // Serial.print("total freqs ");
//  Serial.println(freqCount);
}

void checkVolume() {

  int v = analogRead(VOLVOL);
  v = constrain(v,0,VOLSENSE);
  byte vb = map(v,0,VOLSENSE,0,10);

  if(vb != volume){
    radio.setVolume(vb);
    volume = vb;
}
  
  emfBias = analogRead(EMFBIASPIN);


}


















