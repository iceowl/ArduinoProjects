#include <MIDI.h>
//#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define out A5  //output pin
#define LEDA 12 //Playing idicator A
#define LEDB 8 //Playing idicator B
#define LEDC 10 //extra
#define APW 0 //Analog in for PW
//#define EN 12


MIDI_CREATE_DEFAULT_INSTANCE();

volatile int compareB_playing = 0;
volatile int compareA_playing = 0;
volatile int compareB_data = 0;
volatile int compareA_data = 0;
volatile int pulseWidth = 30;
volatile boolean PWMflag = false;
int basePW = 200;
int temp = 0;
int multiplyer = 0;
int bendData = 0;
int CH = 0;
int r = 0;
boolean twoNote = false;
double bendAmount = 0.0;





byte MIDI_PW[128] = {
  181,175,170,165,228,221,215,209,203,197,191,255,255,248,
  241,234,228,221,215,209,203,197,191,255,255,248,241,234,228,221,215,209,203,
  197,191,186,181,175,170,165,161,156,152,147,143,139,135,131,128,124,120,117,
  114,110,107,104,101,98,95,93,90,87,85,82,80,78,76,73,71,69,67,65,90,87,85,82,
  80,78,76,73,71,69,67,65,90,87,85,82,80,78,76,73,71,69,67,65,90,87,85,82,80,78,
  76,73,71,69,67,65,90,87,85,82,80,78,76,73,71,69,67,65,90,87,85,82,80,78,75,73};


void setup(){
  pinMode(out,OUTPUT); //setup output pin
  pinMode(LEDA,OUTPUT);
  pinMode(LEDB,OUTPUT);
  pinMode(LEDC,OUTPUT);

  MIDI.begin(); //start midi lib

  //Set up timer and compare and match A and B
  TCCR1A = 0;

  sei(); //  Enable global interrupts

  //Set for a prescale of 8
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= _BV(CS11);

    flashDiode(LEDA); 
    flashDiode(LEDB); 
    flashDiode(LEDC);

}

void flashDiode(int inpin) {
  for(int i=0;i<3;i++){
  digitalWrite(inpin, HIGH);
  delay(10);
  digitalWrite(inpin,LOW); 
  delay(100);
  }
}

void loop(){
 //temp = analogRead(0);
 // basePW = temp / 3.4;
 // if(basePW < 10)
    basePW = 10;

//  temp = analogRead(1);
//  CH = temp / 63;
//  if(CH == 0)
    CH = 1;
  //if(CH > r || CH < r){
    TIMSK1  &=~(1 << OCIE1B);
    TIMSK1  &=~(1 << OCIE1A);
    compareB_playing = 0;
    compareA_playing = 0;
    MIDI.setInputChannel(CH);
    r = CH;
 // }

  //if(digitalRead(EN) == HIGH)
    runMIDI();


}

void runMIDI(){
  if (MIDI.read()) {                    // Is there a MIDI message incoming ?
    switch(MIDI.getType()) {

    case midi::NoteOn:
      play(MIDI.getData1()); //Play note
      break;

    case midi::NoteOff:
      stop(MIDI.getData1()); //Stop note
      break;
    case midi::PitchBend:
      pitchBend(MIDI.getData1(),MIDI.getData2()); //Send data to pitch bend processor
      break;


    }
  }  
}

void play(int note){
  //Pick a compare module to play note
  //See which compare module isn't busy
  if(compareB_playing == 0){
    //Calculat note freq and load into timer
    compareB_data = int((1.0 / (440.0 * (pow(2.0,((float(MIDI.getData1())-69.0)/12.0))))) * 1000000.0); 

    pulseWidth = (MIDI_PW[MIDI.getData1()] / 255.0) * basePW; //Look up PW from note data
    digitalWrite(LEDB, HIGH); //Turn on LED A
    compareB_playing = note; //Set flag as playing

    //Check if 2 notes are playing and set flag and lower PW
    if (compareB_playing > 0 && compareA_playing > 0)
      pulseWidth = pulseWidth / 2;


    TIMSK1 |= (1 << OCIE1B); //Enable compare B
  }


  else if (compareA_playing == 0){
    //Calculat note freq and load into timer
    compareA_data = int((1.0 / (440.0 * (pow(2.0,((float(MIDI.getData1())-69.0)/12.0))))) * 1000000.0);

    pulseWidth = (MIDI_PW[MIDI.getData1()] / 255.0) * basePW; //Look up PW from note data
    digitalWrite(LEDA, HIGH); //Turn on LED B
    compareA_playing = note; //Set flag as playing

    //Check if 2 notes are playing and set flag and lower PW
    if (compareB_playing > 0 && compareA_playing > 0)
      pulseWidth = pulseWidth / 2;



    TIMSK1 |= (1 << OCIE1A); //Enable compare A
  }
}

void stop(int note){
  //Check if playing notes are to be shut off
  if (compareB_playing == note){
    TIMSK1  &=~(1 << OCIE1B); //Disable compare B
    digitalWrite(out, LOW); //Set output to low
    digitalWrite(LEDB, LOW); //Turn off LED B

    if(compareA_playing > 0)
      pulseWidth = (MIDI_PW[compareA_playing] / 255.0) * basePW; 

    compareB_playing = 0; //Set flag as not playing
  }
  else if(compareA_playing == note){
    TIMSK1  &=~(1 << OCIE1A); //Disable compare A
    pulseWidth = pulseWidth * 2;
    digitalWrite(out, LOW); //Set output to low
    digitalWrite(LEDA, LOW); //Turn off LED A

    if(compareB_playing > 0)
      pulseWidth = (MIDI_PW[compareB_playing] / 255.0) * basePW; 

    compareA_playing = 0; //Set flag as not playing
  }
}

void pitchBend(int LSB,int MSB){
  bendData = word(MSB >> 1,LSB >> 1); //Recombine pitchbend data into a 14 bit number
  bendAmount = (float(bendData) - 8192.0) * 0.001465022; //Calculat bend amount

  //Apply pitch bend to compB if playing
  if(compareB_data > 0) 
    compareB_data = int((1.0 / (440.0 * (pow(2.0,(((float(compareB_playing) + bendAmount)-69.0)/12.0))))) * 1000000.0); 

  //Apply pitch bend to compA if playing
  if(compareA_data > 0)
    compareA_data = int((1.0 / (440.0 * (pow(2.0,(((float(compareA_playing) + bendAmount)-69.0)/12.0))))) * 1000000.0);

}


ISR(TIMER1_COMPB_vect){
  OCR1B = TCNT1 + compareB_data; //Relaod timer
  if(!PWMflag) //If a burst is already in progress skip this cycle
    burst();
}

ISR(TIMER1_COMPA_vect){
  OCR1A = TCNT1 + compareA_data; //Relaod timer
  if(!PWMflag) //If a burst is already in progress skip this cycle
    burst();
}

void burst(){
  PWMflag = true; //Set playing flag to true
  delayMicroseconds(10);
  digitalWrite(out, HIGH);
  delayMicroseconds(pulseWidth); //Creat pulse
  digitalWrite(out,LOW); 
  PWMflag = false; //Set playing flag to false
}












