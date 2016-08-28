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


int  xPin   = 0;
int  xaPin  = 14;
int  yPin   = 1;
int  yaPin  = 15;
int  zPin   = 2;
int  zaPin  = 16;
int  refPin = 5;
int  raPin  = 19;
int  maxV   = 100;
int  minV   = 100;
byte oldAcc = B00000000;

unsigned long thisTime  = millis();
unsigned long PRINTTIME = 50;



void setup()                    // run once, when the sketch starts
{

  Serial.begin(9600);
  pinMode(xaPin,INPUT);
  pinMode(yaPin,INPUT);
  pinMode(zaPin,INPUT);
  pinMode(raPin,INPUT);

}

void loop()                     // run over and over again
{
  int xVal   = 0;
  int yVal   = 0;
  int zVal   = 0;
  int rVal   = 0;
  byte acc   = B00000000;


  xVal = analogRead(xPin);
  yVal = analogRead(yPin);
  zVal = analogRead(zPin);
  rVal = analogRead(refPin);
  acc = SenseAcc(xVal,yVal,zVal,rVal);
  PrintAcc(acc);
  //Serial.print("x=");
  //Serial.print(xVal);
  //Serial.print("  ");
  //Serial.print("y=");
  //Serial.print(yVal);
  //Serial.print("  "); 
  //Serial.print("z=");
  //Serial.print(zVal);
  //Serial.print("  ");
  //Serial.print("r=");
  //Serial.print(rVal);
  //Serial.print("\n");


}

void PrintAcc(byte acc) {

  unsigned long now = millis();
  if(now > (thisTime + PRINTTIME)){

    byte mAcc = acc & oldAcc;

    if(mAcc & B00000001){
      Serial.println("positiveX");
    }
    if(mAcc & B00000010) {
      Serial.println("negativeX");
    }
    if(mAcc & B00000100){
      Serial.println("positiveY");
    }
    if(mAcc & B00001000) {
      Serial.println("negativeY");
    }
    if(mAcc & B00010000) {
      Serial.println("positiveZ");
    }
    if(mAcc & B00100000){
      Serial.println("negativeZ");
    } 
    oldAcc = ~acc;
    thisTime = now;
  }
}

byte SenseAcc(int xV, int yV, int zV, int rV){
  int vMax = rV/2 + maxV;
  int vMin = rV/2 - minV;
  byte acc = B00000000;
  if(xV > vMax) {
    acc |= B00000001;
  }
  if(xV < vMin) {
    acc |= B00000010;
  }
  if(yV > vMax) {
    acc |= B00000100;
  }
  if(yV < vMin) {
    acc |= B00001000;
  }
  if(zV > vMax) {
    acc |= B00010000;
  }
  if(zV < vMin) {
    acc |= B00100000;
  }

  return acc;

}
