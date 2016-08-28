#include "WProgram.h"
#include <Peggy2.h>


void setup();
void loop ();
void doBlueRows( int start,  int finish);
void doRedRows( int start,  int finish);
void doGreenRows( int start,  int finish);
void allOn();
void getButton();
Peggy2 frame1, frame2;

int phase = 0;
boolean ALLON = false;

unsigned long regRow[6];
unsigned long regRow2[6];

unsigned long redRow1[25], redRow2[25];
unsigned long blueRow1[25], blueRow2[25];
unsigned long greenRow1[25], greenRow2[25];
unsigned char InputReg, InputRegOld;
unsigned long time = millis();

void setup() {

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


}

void loop () {

  getButton();

  if(ALLON) {
    allOn();
  } 
  else if(phase < 10) {
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

  if(!ALLON){
    phase++;
  }


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
        if(!ALLON){
          ALLON = true;
        } 
        else {
          ALLON = false;
        }
      }
      time = whenPressed;
    }
  }

  InputRegOld = InputReg;

}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

