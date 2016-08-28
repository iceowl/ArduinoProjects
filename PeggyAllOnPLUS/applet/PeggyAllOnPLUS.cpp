#include "WProgram.h"
#include <Peggy2.h>


void setup();
void loop ();
void slowBlaster();
void Blaster();
void doBlueRows( int start,  int finish);
void doRedRows( int start,  int finish);
void doGreenRows( int start,  int finish);
void allOn();
void getButton();
void staticPeggy(boolean mondrian);
void colorFade();
void signalMotion();
void signalButton();
boolean readMotion();
Peggy2 frame1; 
Peggy2 frame2;


int phase = 0;
int motionPin = 5;
int val = 1000;
int readMotionThreshold = 20;
long motionTime = 0;
long motionThreshold = 3000;
long onThreshold = 60000;
long initialWait = 2000;
long cycleTime = 0;
long recentOn = 0;
boolean first = true;
boolean stillOn = false;
short ALLON = 0;
unsigned int redi = 0;
unsigned int greeni = 10;
unsigned int bluei = 0;
unsigned int lightState = 0;

unsigned long regRow[6];
unsigned long regRow2[6];

unsigned long redRow1[25], redRow2[25];
unsigned long blueRow1[25], blueRow2[25];
unsigned long greenRow1[25], greenRow2[25];
unsigned char InputReg, InputRegOld;
unsigned long time, time2;
unsigned long threshold = 10000;
unsigned long mondrianTime;


void setup() {
  // Serial.begin(9600);
  unsigned int xr = 24;
  unsigned int rowIndex = 5;


  frame1.HardwareInit();
  frame1.Clear();
  frame2.Clear();

  regRow[5] = 18724;
  regRow[4] = 37449;
  regRow[3] = 9362;
  regRow[2] = 18724;
  regRow[1] = 37449;
  regRow[0] = 9362;

  regRow2[5] = 292;
  regRow2[4] = 73;
  regRow2[3] = 146;
  regRow2[2] = 292;
  regRow2[1] = 73;
  regRow2[0] = 146;



  while (xr > 9) {
    redRow1[xr]   = regRow[rowIndex] << 9 ;
    blueRow1[xr]  = regRow[rowIndex-1] << 9;
    greenRow1[xr] = regRow[rowIndex-2] << 9;
    redRow2[xr]   = regRow2[rowIndex];
    blueRow2[xr]  = regRow2[rowIndex-1];
    greenRow2[xr] = regRow2[rowIndex-2];
    rowIndex--;
    if(rowIndex < 3){
      rowIndex = 5;
    }
    xr--;
  }
  redRow1[9] = 9586688;
  redRow1[8] = 19173888;
  redRow1[7] = 8987648;
  redRow1[6] = 5261312;
  redRow1[5] = 19173888;
  redRow1[4] = 9577472;
  redRow1[3] = 4786176;
  redRow1[2] = 19173888;
  redRow1[1] = 9586688;
  redRow1[0] = 4784128;
  redRow2[9] = 292;
  redRow2[8] = 73;
  redRow2[7] = 146;
  redRow2[6] = 292;
  redRow2[5] = 73;
  redRow2[4] = 146;
  redRow2[3] = 292;
  redRow2[2] = 73;
  redRow2[1] = 146;
  redRow2[0] = 292;
  blueRow1[9] = 19173888;
  blueRow1[8] = 4793344;
  blueRow1[7] = 5525504;
  blueRow1[6] = 19051008;
  blueRow1[5] = 8987648;
  blueRow1[4] = 4491264;
  blueRow1[3] = 19026432;
  blueRow1[2] = 9511936;
  blueRow1[1] = 4755968;
  blueRow1[0] = 19164160;
  greenRow1[9] = 4793344;
  greenRow1[8] = 9586688;
  greenRow1[7] = 19040768;
  greenRow1[6] = 9241600;
  greenRow1[5] = 5392384;
  greenRow1[4] = 19485184;
  greenRow1[3] = 9741312;
  greenRow1[2] = 4868096;
  greenRow1[1] = 19211264;
  greenRow1[0] = 9605632;


  blueRow2[9] = 146;
  blueRow2[8] = 146;
  blueRow2[7] = 292;
  blueRow2[6] = 73;
  blueRow2[5] = 146;
  blueRow2[4] = 292;
  blueRow2[3] = 73;
  blueRow2[2] = 146;
  blueRow2[1] = 292;
  blueRow2[0] = 73;  
  greenRow2[9] = 73;
  greenRow2[8] = 292;
  greenRow2[7] = 73;
  greenRow2[6] = 146;
  greenRow2[5] = 292;
  greenRow2[4] = 73;
  greenRow2[3] = 146;
  greenRow2[2] = 292;
  greenRow2[1] = 73;
  greenRow2[0] = 146;


  InputRegOld = (PINC & 31) | ((PINB & 1)<<5);
  time = millis();
  time2 = millis();
  mondrianTime = millis();
  motionTime = millis();


}

void loop () {


  cycleTime = millis();
  val = digitalRead(motionPin);
  if(!first){
    getButton();
  } 
  else if((cycleTime  - time) > initialWait){

    first = false;
  }
  if(ALLON == 0) {
    //allOn();
    slowBlaster();
    frame1.RefreshAll(2);
    frame2.RefreshAll(2);
  } 
  else if (ALLON == 1) {
    Blaster();
    if((cycleTime - time2) > threshold) {
      ALLON = 2;
      time2 = cycleTime;
    }
  } 
  else if (ALLON == 2) {
    staticPeggy(false);
    if((cycleTime - time2) > threshold) {
      ALLON = 3;
      time2 = cycleTime;
    }
  } 
  else if (ALLON == 3) {
    colorFade(); 
    if((cycleTime - time2) > threshold) {
      ALLON = 4;
      time2 = cycleTime;
    }
  }
  else if (ALLON == 4) {
    staticPeggy(true);
    if((cycleTime - time2) > (threshold+5000)){
      ALLON = 1;
      time2 = cycleTime;
    }
  }

}

void slowBlaster() {

  if(phase < 100) {
    doBlueRows(0,24);
  } 
  else if (phase >= 100 && phase < 200) {
    doRedRows(0,24);
  } 
  else if (phase >=200 && phase < 300){
    doGreenRows(0,24);
  } 
  else if (phase >= 300){
    phase = 0;
  }
  phase++;
}

void Blaster() {
  if(phase < 10) {
    doBlueRows(0,24);
  } 
  else if (phase >= 10 && phase < 20) {
    doRedRows(0,24);
  } 
  else if (phase >=20 && phase < 30){
    doGreenRows(0,24);
  } 
  else if (phase >= 30){
    phase = 0;
  }
  phase++;
  frame1.RefreshAll(6);
  frame2.RefreshAll(4);
}

void doBlueRows( int start,  int finish) {

  int r = start;

  frame1.Clear();
  frame2.Clear();
  while(r <= finish){
    frame1.WriteRow(r,blueRow1[r]);
    frame2.WriteRow(r,blueRow2[r]);
    r++;
  }

}
void doRedRows( int start,  int finish) {

  int r = start;

  frame1.Clear();
  frame2.Clear();
  while(r <= finish){
    frame1.WriteRow(r,redRow1[r]);
    frame2.WriteRow(r,redRow2[r]);
    r++;
  }


}

void doGreenRows( int start,  int finish) {

  int r = start;

  frame1.Clear();
  frame2.Clear();
  while(r <= finish){
    frame1.WriteRow(r,greenRow1[r]);
    frame2.WriteRow(r,greenRow2[r]);
    r++;
  }

}

void allOn(){
  int y = 0;
  while(y < 25){
    frame1.WriteRow(y,65535<<9);
    frame2.WriteRow(y,511);
    y++;
  }
}

void getButton() {

  long curTime; 
  val = analogRead(motionPin);
  //Serial.println(val);
  curTime = millis();
  if((curTime - recentOn) > onThreshold) {
    ALLON = 0;
    recentOn = millis();
    time2 = millis();
  }
  if(readMotion()) {
    if((curTime - motionTime) > motionThreshold) {
      if(!stillOn){
        recentOn = millis();
        stillOn = true;
      }
      signalMotion();
      ALLON++;
      if(ALLON > 4){
        ALLON = 1;
      }
      motionTime = millis();
      time2 = millis();
      cycleTime = time2;
    }
  } 


  unsigned long whenPressed;
  InputReg = (PINC & 31) | ((PINB & 1)<<5);		// Input reg measures OFF ( bit 5) and b1-b5 (bits 0-4). 
  InputRegOld ^= InputReg;		// InputRegOld is now nonzero if there has been a change.
  if (InputRegOld) {
    InputRegOld &= InputReg;	// InputRegOld is now nonzero if the change was to the button-released (not-pulled-down) state.
    // i.e., the bit that was high after the XOR corresponds to a bit that is presently high.
    // The checks that follow will handle MULTIPLE buttons being pressed and unpressed
    // at the same time.

    if(InputRegOld & 1) {
      whenPressed = millis(); //debounce
      if(whenPressed - time > 50) {
        if(!stillOn){
          recentOn = millis();
          stillOn = true;
        }
        signalButton();
        ALLON++;
        if(ALLON > 4){
          ALLON = 0;
        } 
        time = whenPressed;
        time2 = whenPressed;
        motionTime = whenPressed;
      }

    }
  }

  InputRegOld = InputReg;

}

void staticPeggy(boolean mondrian) {
  unsigned short y = 0;     
  unsigned short reps = 0;
  unsigned long mTime;
  unsigned long al,bl, cl;

  mTime = millis();

  while (y < 25) {

    al = random();  // random(), defined in stdlib.h, produces a LONG integer.
    bl = random();
    if(mondrian){
      cl = al | bl;
      bl |= al;
    } 
    else {
      cl = al & bl;
    }

    if((mondrian && (mTime - mondrianTime) > 1000) || !mondrian){

      frame1.WriteRow( y, bl);
      frame2.WriteRow( y, cl); 
    }
    y++;
  }
  if(!mondrian){
    while (reps < (10 + (rand() & 32)))
    {

      frame1.RefreshAll(1); //Draw frame buffer 1 time
      frame2.RefreshAll(3); //Draw frame buffer 2 times 

      reps++;
    }
  } 
  else {
    frame1.RefreshAll(10);
    frame2.RefreshAll(10);
  }
  if((mTime - mondrianTime) > 1000) {
    mondrianTime = millis();
  }

}


void colorFade() {


  if( bluei < 10 && lightState == 0){
    bluei++;
    greeni--;
    doBlueRows(0,24);  
    frame1.RefreshAll(bluei+2);
    frame2.RefreshAll(bluei);
    doGreenRows(0,24);
    frame1.RefreshAll(greeni+2);
    frame2.RefreshAll(greeni);
  } 
  else  if((bluei == 10) && (lightState == 0)){
    lightState = 1;
  } 
  else if( redi < 10 && lightState == 1){
    redi++;
    bluei--;
    doRedRows(0,24);
    frame1.RefreshAll(redi+2);
    frame2.RefreshAll(redi);
    doBlueRows(0,24);
    frame1.RefreshAll(bluei+2);
    frame2.RefreshAll(bluei);
  } 
  else if((redi == 10) && (lightState == 1)){
    lightState = 2;
  } 
  else if( greeni < 10 && lightState == 2){
    redi--;
    greeni++;
    doGreenRows(0,24);
    frame1.RefreshAll(greeni+2);
    frame2.RefreshAll(greeni);
    doRedRows(0,24);
    frame1.RefreshAll(redi+2);
    frame2.RefreshAll(redi);
  } 
  else if((greeni == 10) && (lightState == 2)){
    lightState = 0;
  }

}
void signalMotion() {
  long t1,t2;
  t2 = millis();
  t1 = t2;

  while((t1 - t2) < 500) {
    allOn();
    frame1.RefreshAll(6);
    frame2.RefreshAll(2);
    t1 = millis();
  }

}

void signalButton() {
  long t1,t2;
  t2 = millis();
  t1 = t2;

  while((t1 - t2) < 500) {
    doRedRows(0,24);  
    frame1.RefreshAll(4);
    frame2.RefreshAll(2);
    t1 = millis();
  }
}

boolean readMotion() {

  long t1, t2;
  long rcount;
  int rval;
  rcount = 0;
  rval = 0;
  t2 = millis();
  t1 = t2;
  while((t1 - t2) < 1) {
    t1 = millis();
    rval = analogRead(motionPin);
    if(rval <= readMotionThreshold){
      if(rcount < 255){
        rcount++;
      }
    } 
    else if(rcount <= 3){
      rcount = 0;
    }

  }
  if(rcount >= 5){
    return true;
  } 
  else {
    return false;
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

