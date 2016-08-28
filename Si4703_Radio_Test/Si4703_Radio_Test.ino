#include <Si4703_Breakout.h>
#include <Wire.h>
//#include <CapSense.h>
int LEDPin = 13;
int resetPin = 2;
int chargePin =7;
int SDIO  = A4;
int SCLK  = A5;
int emfProbe = A1;
int irProbe = A2;
int gravProbe = A0;
int emfSense = 500; //raise to decrease sensitivity up to 1023
int irSense = 500;
unsigned char printSens = 0;
unsigned char dograv = 0;
unsigned char bannerNotPrinted = 1;

unsigned long delta;
unsigned long oldDelta;
unsigned long oldChargeTime;
unsigned long chargeTime;

Si4703_Breakout radio(resetPin, SDIO, SCLK);
int channel = 0;
int volume = 1;
char rdsBuffer[10];

void setup()
{
  pinMode(LEDPin,OUTPUT);
  digitalWrite(LEDPin,HIGH);
  Serial.begin(9600);

  randomSeed(analogRead(3));
  pinMode(chargePin,OUTPUT);
  digitalWrite(chargePin,LOW);
  radio.powerOn();
  radio.setVolume(8);
}

void printBanner() {
  if(Serial.available() > 0){
    Serial.println("\n\nSi4703_Breakout Test Sketch");
    Serial.println("===========================");  
    Serial.println("a b     Favourite stations");
    Serial.println("+ -     Volume (max 15)");
    Serial.println("u d     Seek up / down");
    Serial.println("r       Listen for RDS Data (15 sec timeout)");
    Serial.println("Send me a command letter.");
  }
  bannerNotPrinted = 0;
}

void loop()
{


 

  int emf = analogRead(emfProbe);
  int ir  = analogRead(irProbe);
  int cVolt = analogRead(gravProbe);

  emf = constrain(emf,1,emfSense);
  emf = map(emf,0,emfSense,25,750);
  ir = constrain(ir,1,irSense);
  ir = map(ir,0,irSense,25,750);

  oldChargeTime = chargeTime;
  oldDelta = delta;




  if(cVolt < 100 && dograv)  {

    chargeTime = micros();
    digitalWrite(chargePin, HIGH);
    while(((cVolt = analogRead(gravProbe)) < 800) && (micros()-chargeTime < 1000000)){
      1;
    }
    chargeTime = micros() - chargeTime;
    if( oldChargeTime > chargeTime)
      delta = oldChargeTime - chargeTime;
    else if( chargeTime > oldChargeTime ) 
      delta = chargeTime - oldChargeTime;
    else delta = 0;
    digitalWrite(chargePin,LOW);

  } 
  else digitalWrite(chargePin,LOW);


  if (Serial.available())
  {
     if(bannerNotPrinted) printBanner();
    char ch = Serial.read();
    if (ch == 'u') 
    {
      channel = radio.seekUp();
      displayInfo();
    } 
    else if (ch == 'd') 
    {
      channel = radio.seekDown();
      displayInfo();
    } 
    else if (ch == '+') 
    {
      volume ++;
      if (volume == 16) volume = 15;
      radio.setVolume(volume);
      displayInfo();
    } 
    else if (ch == '-') 
    {
      volume --;
      if (volume < 0) volume = 0;
      radio.setVolume(volume);
      displayInfo();
    } 
    else if (ch == 'a')
    {
      channel = 885; // Rock FM
      radio.setChannel(channel, 1);
      displayInfo();
    }
    else if (ch == 'b')
    {
      channel = 985; // BBC R4
      radio.setChannel(channel, 1);
      displayInfo();
    }
    else if (ch == 'r')
    {
      Serial.println("RDS listening");
      radio.readRDS(rdsBuffer, 15000);
      Serial.print("RDS heard:");
      Serial.println(rdsBuffer);      
    }
    else if (ch == 'z')
    {
      for(int x=0; x<200;x++){
        unsigned char seekDir = 1;
        channel = radio.seek(seekDir);
        displayInfo();
        int d = random(400);
        delay(d);
      }
    }
    else if (ch == 'w')
    {
      for(int x=0; x<200;x++){
        unsigned char seekDir = 0;
        channel = radio.seek(seekDir);
        displayInfo();
        int d = random(400);
        delay(d);
      }
    }
    else if (ch == 'x') {
      int y =0;
      for (int x=0;x<=408;x++){
        y++;
        if(y>102) y=0;
        radio.setChannel(y,0);
        channel = (y*2)+875;
        displayInfo();
        int d = random(300);
        delay(d+50);
      }
    }
    else if (ch == 'q') {
      for (int x=0;x < 200;x++){
        int y = random(103);
        radio.setChannel(y,0);
        channel = (y*2)+875;
        displayInfo();
        int d = random(300);
        delay(d+50);
      }
    }
    else if (ch == 'i') {
      if(printSens)
        printSens = 0;
      else printSens = 1;

    }
    else if (ch == 'g') {
      if(!dograv)
        dograv = 1;
      else dograv =0;
    }
  }
  else {
    unsigned char seekDirection;
    if((emf < 35 ) && (ir < 102)) {
      radio.setVolume(0);
    }
    else {
      radio.setVolume(8);
      if(ir > 200) 
        seekDirection = 0;
      else  
        seekDirection = 1;
      radio.seek(seekDirection);
      //int d = random(550);
      if(emf > ir)
        delay(emf);
      else delay(ir);
    }
  }

  if(printSens) {
    if(!dograv) printSensors();
    printIrEmf(ir,emf);
  }

}

void printSensors() {
  Serial.print("irProbe :");
  Serial.print(analogRead(irProbe));
  Serial.print("  emfProbe:");
  Serial.print(analogRead(emfProbe));
  Serial.print("  gravProbe:");
  Serial.println(analogRead(gravProbe));
}

void printIrEmf(int ir, int emf) {
  Serial.print("ir :");
  Serial.print(ir);
  Serial.print(" emf:");

  if(oldDelta != delta) {
    Serial.print(emf);
    Serial.print("  chargeTime:");
    Serial.print(chargeTime);
    Serial.print(" diff:");
    Serial.println(delta);
  } 
  else Serial.println(emf);


}
void displayInfo()
{
  Serial.print("Channel:"); 
  Serial.print(channel); 
  Serial.print(" Volume:"); 
  Serial.println(volume); 
}

















