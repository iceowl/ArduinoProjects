
// Original Code by Uzzors
// Modified JMastroianni 6/2011 with help from Eric Goodchild's code


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "MIDI.h"

#define MIDI_IN_CHANNEL 1 //this doesn't matter as we're in OMNI mode

#define OUTPIN  A5 //Output pin to drive coil. Also port C pin 5

//#define NOTELIM  70 // Highest MIDI note to output.

#define LEDA 12 //Playing indicator A
#define LEDB 8 //Playing indicator B
#define LEDC 10 //idiot light to flash whenever anything is happening


#define MAXBEND         2.0  //max bend divisor = 2 means pitch will vary as a max/min to 1/2 the note pulse width
#define VELOCITY_MID    63
#define TIMER1_PRESCALE 8
#define TIMER2_PRESCALE 8
#define CLOCK_SCALE     4
#define PULSE_SCALE     CLOCK_SCALE/2
#define BASE_PW         2
#define MAX_PW          200


//Uzzors original vars to control the 2 interrupt stop/start periods for TIMER1
volatile uint16_t compareB_off       = 0;
volatile uint16_t compareA_off       = 0; 
volatile uint16_t compareB_on        = 0; 
volatile uint16_t compareA_on        = 0;
volatile uint8_t  compareB_playing   = 0;
volatile uint8_t  compareA_playing   = 0; 
volatile uint8_t  modeB              = 0; 
volatile uint8_t  modeA              = 0; 
volatile uint8_t  channel            = 0;

//Let's decide what the frequency should be to drive the duty cycle interrupter when we have 2 simultaneous notes
volatile uint16_t pulseon  = 0;
volatile uint8_t  aVel     = 0;
volatile uint8_t  bVel     = 0;
//
volatile boolean  pulseOn  = false;
volatile boolean  once     = false;

// List of Timer 1 values required to generate various frequencies, encompasses full 88 notes.
// 
// Trying a higher duty cycle because I'm cutting up the pulses with the 2nd interrupt


//AVR Crystal Frequency: 16000000 Hz
//Prescaler: 1:8
//Duty Cycle: 25.0%

static const uint16_t compare_off[88] = {
  54546, 51484, 48594, 45867, 43293, 40863, 38569, 36405, 34362, 32433, 30612, 28894, 27273, 25742, 24297, 22934, 21646, 
  20431, 19284, 18202, 17181, 16216, 15306, 14447, 13636, 12871, 12149, 11467, 10823, 10215, 9642, 9101, 8590, 8108, 7653, 
  7224, 6818, 6435, 6075, 5733, 5412, 5108, 4821, 4551, 4295, 4054, 3827, 3612, 3409, 3218, 3037, 2867, 2706, 2554, 2411, 2275, 2148, 
  2027, 1914, 1806, 1704, 1609, 1518, 1434, 1353, 1277, 1206, 1137, 1074, 1014, 957, 903, 852, 804, 759, 717, 676, 639, 603, 569, 537, 
  507, 478, 451, 426, 402, 380, 358 };


static const uint16_t compare_on[88] = {
  18181, 17161, 16198, 15289, 14430, 13620, 12856, 12134, 11453, 10810, 10204, 9631, 9090, 8580, 8099, 7644, 7215, 6810, 6428, 
  6067, 5726, 5405, 5102, 4815, 4545, 4290, 4049, 3822, 3607, 3405, 3214, 3033, 2863, 2702, 2551, 2407, 2272, 2145, 2024, 1911, 
  1803, 1702, 1607, 1516, 1431, 1351, 1275, 1203, 1136, 1072, 1012, 955, 901, 851, 803, 758, 715, 675, 637, 601, 568, 536, 506, 
  477, 450, 425, 401, 379, 357, 337, 318, 300, 284, 268, 253, 238, 225, 212, 200, 189, 178, 168, 159, 150, 142, 134, 126, 119 };

static const uint16_t MIDI_PW[128]  = { //these are Eric's scaled values
  181,175,170,165,228,221,215,209,203,197,191,255,255,248,
  241,234,228,221,215,209,203,197,191,255,255,248,241,234,228,221,215,209,203,
  197,191,186,181,175,170,165,161,156,152,147,143,139,135,131,128,124,120,117,
  114,110,107,104,101,98,95,93,90,87,85,82,80,78,76,73,71,69,67,65,90,87,85,82,
  80,78,76,73,71,69,67,65,90,87,85,82,80,78,76,73,71,69,67,65,90,87,85,82,80,78,
  76,73,71,69,67,65,90,87,85,82,80,78,76,73,71,69,67,65,90,87,85,82,80,78,75,73 };



void setup(void){


  sei(); //enable global interrupts
  //Set up timer and compare and match A and B
  TCCR1A = 0;
  //Set up pulser timer 2
  TCCR2A = 0;

  //Set for a prescale of 8 for Timer 1
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= _BV(CS11);

  //Set a prescale of 8 for Timer 2

  TCCR2A &= ~(_BV(CS20) | _BV(CS21) | _BV(CS22));
  TCCR2A |=  _BV(CS21);// | _BV(CS00);



  pinMode(LEDA,OUTPUT);
  pinMode(LEDB,OUTPUT);
  pinMode(LEDC,OUTPUT);
  pinMode(OUTPIN,OUTPUT);

  MIDI.begin(MIDI_IN_CHANNEL);

  //just to let us know things are initialized
  flashDiode(LEDA,3,100); 
  flashDiode(LEDB,3,100); 
  flashDiode(LEDC,3,100);

}

void flashDiode(int inpin, int count, int period) { 
  for(int i=0;i< count;i++){
    digitalWrite(inpin, HIGH);
    delay(period);
    digitalWrite(inpin,LOW); 
    delay(period);
  }
}
// Main program Loop

void loop(void){

  if(MIDI.read()){
    flashDiode(LEDA,1,1); //activity light
    switch(MIDI.getType()){

    case NoteOff:
      stopNote(MIDI.getData1());
      break;

    case NoteOn:
      if(MIDI.getData2()){ //a stop note could be a NoteON command with velocity = 0  Data1 = note number, Data2 = velocity
        playNote(MIDI.getData1(),MIDI.getData2());
      } 
      else { // velocity = 0 so stop
        stopNote(MIDI.getData1());
      }
      break;

      //Send data to pitch bend processor, Data1 = LSB, Data2 = MSB  Pitch data is 2 bytes, the most sig bit of each byte is always 0, 
      //so max bend up is 16383 (0x7F,0x7F) and max bend down is 0 (0x00,0x00).   The midpoint value is 8192  (0x00, 0x40) which equals no 
      //bend.
    case PitchBend:
      doPitchBend(MIDI.getData1(),MIDI.getData2()); 
      break;

    case ControlChange:
      if((MIDI.getData1() >= 120) && (MIDI.getData1() <= 124)){ // all notes off
        if(compareA_playing){
          stopNote(compareA_playing);
        }
        if(compareB_playing){
          stopNote(compareB_playing);
        }
      }
      break; 
    }
  }
  digitalWrite(LEDA,LOW);

}

void doPitchBend(uint8_t LSB, uint8_t MSB){



  float bendPercentage   = 0.0;
  int   bendData         = 0;


  if((compareA_playing == 0) && (compareB_playing == 0)) {
    return; // no note is playing so don't bend anything
  }

  bendData = word(MSB,LSB); //Recombine pitchbend data into a 16 bit number

  if(bendData > 16383){ //protection against insanity - this should never happen
    bendData = 16383;
  }   
  else if (bendData < 0) { //word is unsigned so this should never happen either
    bendData = 0;
  }

  //8192 = MIDI Bend Control Midpoint Value  16383 = max pitch bend up, 0 is max bend down
  bendPercentage =  -(float(bendData-8192)/8192.0);


  if(compareA_playing){ 
    modifyA_params( bendPercentage);
  } 
  if(compareB_playing){
    modifyB_params( bendPercentage);
  }

}

uint8_t getNormalizedIndex(uint8_t index){

  if(index > 108){
    index = 87;
  } 
  else if(index < 21){
    index = 0;
  } 
  else {
    index = index - 21;
  }

  return index;

}

void modifyA_params(float percentage) {

  uint8_t  index   = getNormalizedIndex(compareA_playing);
  uint16_t tempOn  = compare_on[index];
  uint16_t tempOff = compare_off[index];

  //shrink or expand the timings
  compareA_on  = tempOn  + (float(compareA_on)/MAXBEND  * percentage);
  compareA_off = tempOff + (float(compareA_off)/MAXBEND * percentage);

}

void modifyB_params(float percentage){

  uint8_t  index   = getNormalizedIndex(compareB_playing);
  uint16_t tempOn  = compare_on[index];
  uint16_t tempOff = compare_off[index];


  compareB_on  = tempOn  + (float(compareB_on)/MAXBEND  * percentage);
  compareB_off = tempOff + (float(compareB_off)/MAXBEND * percentage);

}

void set_compB(uint8_t data)
{

  // Check if key is in range, if not assign value.
  uint8_t index    = getNormalizedIndex(data);

  compareB_playing = data;
  compareB_off     = compare_off[index];
  compareB_on      = compare_on[index];

  // Compare Match B interrupt enable

  modeB = 0;
}
// Lookup Compare Match B value

void set_compA(uint8_t data)

{

  uint8_t index    = getNormalizedIndex(data);

  compareA_playing = data;
  compareA_off     = compare_off[index];
  compareA_on      = compare_on[index];

  // Compare Match A interrupt enabl
  modeA = 0;

}

// Find who's playing the note and shut them up
void stopNote(uint8_t data)

{
  if (compareB_playing == data) {
    if(compareA_playing == 0){
      killPulser();
    }
    TIMSK1 &= ~(1 << OCIE1B);
    digitalWrite(LEDC,LOW);
    compareB_playing = 0;
    bVel = 0;
    once = true;
    if(compareA_playing){
      setDutyCycle(aVel);
    }
  } 
  else if (compareA_playing == data) {
    if(compareB_playing == 0){
      killPulser();
    }
    TIMSK1 &= ~(1 << OCIE1A);
    digitalWrite(LEDB,LOW);
    compareA_playing = 0;
    aVel = 0;
    once = true;
    if(compareB_playing){
      setDutyCycle(bVel);
    }
  }

}


void killPulser(void){
  TIMSK2 &= ~(1 << OCIE2A);
  PORTC = 0;
}



// Pick compare module to play note
void playNote(uint8_t note, uint8_t noteData)

{
  // See which compare module isn't busy
  //This is all Uzzor's stuff.  I'm just lighting diodes to be pretty
  if (compareA_playing == 0) {

    digitalWrite(LEDB,HIGH);
    set_compA(note);
    aVel = noteData;
    setDutyCycle(noteData);
    TIMSK1 |= (1 << OCIE1A);
    TIMSK2 |= (1 << OCIE2A);
  } 
  else if (compareB_playing == 0) {

    digitalWrite(LEDC,HIGH);
    set_compB(note);
    bVel = noteData;
    setDutyCycle(noteData);
    TIMSK1 |= (1 << OCIE1B);
    TIMSK2 |= (1 << OCIE2A);
  }

}

int getVelocity(uint8_t inVel){

  int temp = 0;

  if(inVel > 127){
    inVel = 127;
  } 
  if(inVel <= 0) {
    return 0;
  }

  temp = inVel - VELOCITY_MID;

  return temp/CLOCK_SCALE;

}

void setDutyCycle(uint8_t velocity){


  uint16_t pwA, pwB, tempPW;
  int temp = 0;

  //I'm trying to adjust the duty cycle to follow the volume.  
  if((compareA_playing > 0) && (compareB_playing > 0)) {
    pwA = MIDI_PW[compareA_playing];
    pwB = MIDI_PW[compareB_playing];
    tempPW = max(pwA,pwB)/PULSE_SCALE;
    tempPW = tempPW * BASE_PW;
  } 
  else if(compareA_playing > 0){
    temp = MIDI_PW[compareA_playing]/PULSE_SCALE + getVelocity(velocity);
    if(temp >=0) {
      tempPW = temp * BASE_PW;
    } 
    else {
      tempPW = MIDI_PW[compareA_playing]/PULSE_SCALE;
    }
  } 
  else if(compareB_playing > 0) {
    temp = MIDI_PW[compareB_playing]/PULSE_SCALE + getVelocity(velocity);
    if(temp >= 0){
      tempPW = temp * BASE_PW;
    } 
    else {
      tempPW = MIDI_PW[compareB_playing]/PULSE_SCALE;
    }
  }
  if(tempPW > MAX_PW){
    tempPW = MAX_PW;
  }
  pulseon = tempPW;
}

// Compare Match B interrupt

ISR(TIMER1_COMPB_vect)

{

  if (modeB){
    OCR1B = TCNT1 + compareB_off;
    modeB = 0;
  } 
  else {
    OCR1B = TCNT1 + compareB_on;
    modeB = 1;
    if(!pulseOn){
      once = false;
    }
  }

}


// Compare Match A interrupt

ISR(TIMER1_COMPA_vect)

{
  if (modeA) {
    OCR1A = TCNT1 + compareA_off;
    modeA = 0;

  } 
  else  {
    OCR1A = TCNT1 + compareA_on;
    modeA = 1;
    if(!pulseOn){
      once = false;
    }
  }
}


ISR(TIMER2_COMPA_vect){ //reduce the duty cycle of the notes if more than one playing, and reduce/increase duty cycle to account for note velocity

  if(modeA | modeB | pulseOn) {

    if(pulseOn){
      OCR2A = TCNT2 + 10;
      PORTC = 0b00000000; //shut off port A5 if the duty cycle is up
      pulseOn = false;

    } 
    else if (!once){ //we have a note play it
      OCR2A = TCNT2 + pulseon;
      PORTC = 0b00100000; //Turn on Arduino pin A5 if either note A or B is sounding and we're currently off
      pulseOn = true;
      once = true;
    }
  }

}
























