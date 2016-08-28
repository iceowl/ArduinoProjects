



#define MAXWAVEFORM   3
#define MAXSAMPLESNUM 120
#define WAVE0   0
#define WAVE1   1
#define WAVE2   2
#define WAVE3   3
#define PWMTIMER2_B   3
#define PWMTIMER2_A   11

static int  waveformsTable[MAXWAVEFORM][MAXSAMPLESNUM];
       int i = 0;
       
void setup() {
  
  pinMode(PWMTIMER2_B, OUTPUT);
  pinMode(PWMTIMER2_A, OUTPUT);

  //Setup Timer 2 which controls pins 3 and 11 via hardware
  // we will set up for FAST PWM and the TOP value is controlled via OCR2x
  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));

  
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS20);
  
  //Set up values statically so we don't have to calculate each time , only lookup
  for (int i = 0; i < MAXSAMPLESNUM; i++) {
    waveformsTable[WAVE0][i] = 127.0 * (1.0 + sin ((float)i * 3.14159 / 60.0)); //sine 
    waveformsTable[WAVE1][i] = 3.0 * (float)(i % 60);                           //sawtooth
    waveformsTable[WAVE2][i] = 3.0 * (60.0 - (float)(abs(( i % 120 ) - 60)));   // triangle
  }
  //attachInterrupt(button0, wave0Select, RISING);  // Interrupt attached to the button connected to pin 2
  //attachInterrupt(button1, wave1Select, RISING);  // Interrupt attached to the button connected to pin 3
}

void loop() {
  
  OCR2B = (int)waveformsTable[WAVE0][i]; //PIN 3
  OCR2A = (int)waveformsTable[WAVE1][i]; //PIN 11

  i+=1;
  
  delayMicroseconds(1000);

  if (i >=  MAXSAMPLESNUM - 1) i = 0;

}

// function hooked to the interrupt on digital pin 2
//void wave0Select() {
//  wave0++;
//  if(wave0 == 4)
//    wave0 = 0;
//}

// function hooked to the interrupt on digital pin 3
//void wave1Select() {
// wave1++;
// if(wave1 == 4)
//   wave1 = 0;
//}


