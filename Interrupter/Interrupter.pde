/*
 * Basic Interrupter
 */


#define MAXLENGTH  300.0  //microseconds
#define MINLENGTH  5.0   //microseconds
#define MAXFREQ    20.0  //microseconds
#define MINFREQ    10000.0    //micrseconds
#define DRIVEPIN   9

int      PLPin       = A4;
int      PFPin       = A5;
long     PFreq       = 0L;
long     PLength     = 0L;
float    stepFreq    = 0.0;
float    stepLength  = 0.0;
boolean  setBurstTime    = false;

unsigned long burstTime   = 0L;
unsigned long time        = 0L;


void setup()                    // run once, when the sketch starts
{
  pinMode(DRIVEPIN, OUTPUT);      // sets the digital pin as output
  pinMode(PLPin, INPUT);
  pinMode(PFPin, INPUT);
  digitalWrite(DRIVEPIN,LOW);


  stepLength = (MAXLENGTH - MINLENGTH) / 512.0;
  stepFreq   = (MINFREQ - MAXFREQ) / 1024.0;

}

void burst(long PLen){
  delayMicroseconds(PLen);
  digitalWrite(DRIVEPIN, HIGH);
  delayMicroseconds(PLen); //Creat pulse
  digitalWrite(DRIVEPIN,LOW); 
}

void loop()                     // run over and over again
{

  boolean       doPulse     = true;
  boolean       burstMode   = false;
 
  
  PLength = long(MINLENGTH + (readPL() * stepLength));

  if(PLength > MAXLENGTH){
    PLength = MAXLENGTH;
  }

  PFreq = long(MAXFREQ + (readPF() * stepFreq)); 

  if(PFreq < MAXFREQ){
    PFreq = MAXFREQ;
  }

  if(readPL() > 513.0) {
    burstMode = true;
    if(!setBurstTime && (micros() > burstTime)){
      burstTime = micros() + long(PFreq * 100.0);
      setBurstTime = true;
    } 
    else {
      if(micros() > burstTime){
        setBurstTime = false;
        burstTime = micros() + long(readPL() * 500.0);
      }
    }
  } 
  
  time = micros();
  
  if(burstMode){
      while(micros() < (time+MINFREQ)) {
        if(doPulse && setBurstTime) {
          burst(PLength);
          doPulse = false;
        }
      }
  }
  else {
    while(micros() < (time+long(PFreq))) {
      if(doPulse) {
        burst(PLength);
        doPulse = false;
      }
    }
  }
}

float readPF() {

  return(float(analogRead(PFPin)));


}

float readPL() {
  return(float(analogRead(PLPin)));

}












