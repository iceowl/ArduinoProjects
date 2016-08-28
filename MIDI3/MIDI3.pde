



#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "MIDI.h"

// USART definitions

//#define USART_BAUDRATE 31250 
//#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

// Midi flags definitions

//#define NOTE_ON 0
//#define VALID 1
//#define COUNTER 2
//#define COMPFLAG_MIDI_IN 1
//#define COMPFLAG_MIDI_OUT 0
#define MIDI_IN_CHANNEL 1


#define OUTPIN  A5 //Output pin to drive coil.
//#define NOTELIM  70 // Highest MIDI note to output.
//#define PWLIM  300 //Pulse Width limit in us.
//#define PWMULT  3 //  multiplier applied to velocity to get pulse width .
#define LEDA 12 //Playing idicator A
#define LEDB 8 //Playing idicator B
#define LEDC 10 //extra




volatile uint16_t compareB_off = 0;
volatile uint16_t compareA_off = 0; 
volatile uint16_t compareB_on = 0; 
volatile uint16_t compareA_on = 0;
volatile uint8_t  compareB_playing = 0;
volatile uint8_t  compareA_playing = 0; 
volatile uint8_t  modeB = 0; 
volatile uint8_t  modeA = 0; 
volatile uint8_t  channel = 0;

// List of Timer 1 values required to generate various frequencies, encompasses full 88 notes.

static const uint16_t compare_off[88] PROGMEM = {
  65455, 61781, 58313, 55041, 51951, 49035, 46283, 43686, 41234, 38919, 36735, 34673, 32727, 30890, 29157, 27521, 
  25975, 24517, 23141, 21843, 20617, 19459, 18368, 17336, 16363, 15445, 14579, 13761, 12987, 12258, 11571, 10921, 
  10308, 9729, 9184, 8668, 8181, 7722, 7290, 6880, 6494, 6129, 5786, 5461, 5154, 4865, 4592, 4334, 4091, 3861, 3645, 
  3440, 3247, 3065, 2893, 2730, 2577, 2432, 2296, 2167, 2045, 1931, 1822, 1720, 1623, 1532, 1447, 1365, 1288, 1216, 
  1148, 1083, 1023, 965, 911, 860, 811, 766, 723, 683, 644, 608, 574, 541, 512, 483, 456, 430};
  
  
static const uint16_t compare_on[88] PROGMEM = {
  7272, 6864, 6479, 6115, 5772, 5448, 5142, 4853, 4581, 4324, 4081, 3852, 3636, 3432, 3239, 3057, 2886, 2724, 2571, 
  2426, 2290, 2162, 2040, 1926, 1818, 1716, 1619, 1528, 1443, 1362, 1285, 1213, 1145, 1081, 1020, 963, 909, 858, 809, 
  764, 721, 681, 642, 606, 572, 540, 510, 481, 454, 429, 404, 382, 360, 340, 321, 303, 286, 270, 255, 240, 227, 214, 
  202, 191, 180, 170, 160, 151, 143, 135, 127, 120, 113, 107, 101, 95, 90, 85, 80, 75, 71, 67, 63, 60, 56, 53, 50, 47 };

void setup(void){


  // Set MIDI channel to listen on

  // channel = ((PINB & 0xF0) >> 4);
  sei(); //enable global interrupts
  //Set up timer and compare and match A and B
  TCCR1A = 0;

  //Set for a prescale of 8
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= _BV(CS11);

  pinMode(LEDA,OUTPUT);
  pinMode(LEDB,OUTPUT);
  pinMode(LEDC,OUTPUT);
  pinMode(OUTPIN,OUTPUT);
  
  MIDI.begin(MIDI_IN_CHANNEL);
  
  flashDiode(LEDA); 
  flashDiode(LEDB); 
  flashDiode(LEDC);

}

void flashDiode(int inpin) {
  for(int i=0;i<3;i++){
    digitalWrite(inpin, HIGH);
    delay(100);
    digitalWrite(inpin,LOW); 
    delay(100);
  }
}
// Main program Loop

void loop(void)

{
 

  // Store value from USART Buffer
  //temp = UDR;
  if(MIDI.read()){
    switch(MIDI.getType()){
    case NoteOff:
      stopNote(MIDI.getData1());
      digitalWrite(LEDA,LOW);
      break;
    case NoteOn:
      playNote(MIDI.getData1());
      digitalWrite(LEDA,HIGH);
      break;
    }
  }

}



// Lookup Compare Match B value

void set_compB(uint8_t data)
{
  compareB_playing = data;
  // Check if key is in range, if not assign value.

  if (data > 108)

  {
    compareB_off = pgm_read_word(&(compare_off[87]));
    compareB_on = pgm_read_word(&(compare_on[87]));
  }
  else if (data < 21)

  {
    compareB_off = pgm_read_word(&(compare_off[0]));
    compareB_on = pgm_read_word(&(compare_on[0]));
    // In range!
  }
  else

  {
    compareB_off = pgm_read_word(&(compare_off[data - 21]));
    compareB_on = pgm_read_word(&(compare_on[data - 21]));
  }
  // Compare Match B interrupt enable

  modeB = 0;
  TIMSK1 |= (1 << OCIE1B);
}
// Lookup Compare Match B value

void set_compA(uint8_t data)

{
  compareA_playing = data;
  // Check if key is in range, if not assign value.

  if (data > 108)

  {
    compareA_off = pgm_read_word(&(compare_off[87]));
    compareA_on = pgm_read_word(&(compare_on[87]));

  }
  else if (data < 21)

  {
    compareA_off = pgm_read_word(&(compare_off[0]));
    compareA_on = pgm_read_word(&(compare_on[0]));

    // In range!

  }
  else

  {
    compareA_off = pgm_read_word(&(compare_off[data - 21]));
    compareA_on = pgm_read_word(&(compare_on[data - 21]));
  }
  // Compare Match A interrupt enabl
  modeA = 0;
  TIMSK1 |= (1 << OCIE1A);
}

// Find who's playing the note and shut them up
void stopNote(uint8_t data)

{
  
  PORTC = 0b00000000; //just to be sure
  // Check if playing notes are to be shut off
  if (compareB_playing == data)
  {
    TIMSK1  &=~(1 << OCIE1B);
    // Set note output low
    digitalWrite(LEDB,LOW);
    compareB_playing = 0;
  }
  else if (compareA_playing == data)
  {
    TIMSK1  &=~(1 << OCIE1A);
    digitalWrite(LEDC,LOW);
    compareA_playing = 0;

  }
}
// Pick compare module to play note

void playNote(uint8_t data)

{
  // See which compare module isn't busy

  if (compareB_playing == 0)

  {
    digitalWrite(LEDB,HIGH);
    set_compB(data);
  }
  else if (compareA_playing == 0)

  {
    digitalWrite(LEDC,HIGH);
    set_compA(data);
  }

}

// Compare Match B interrupt

ISR(TIMER1_COMPB_vect)

{
  // Toggle output pin

  // PORTC ^= 0b00100000;
  // add start value to current timer 1 value ensures that compare match occurrs at set number of Timer 1 increments

  if (modeB)

  {
    OCR1B = TCNT1 + compareB_off;
    modeB = 0;
    PORTC = 0b00000000;
  }
  else

  {
    OCR1B = TCNT1 + compareB_on;
    modeB = 1;
    PORTC = 0b00100000;
  }

}

// Compare Match A interrupt

ISR(TIMER1_COMPA_vect)

{
  // Toggle output pin
  //PORTC ^= 0b00100000;
  // add start value to current timer 1 value ensures that compare match occurrs at set number of Timer 1 increments
  if (modeA)
  {
    OCR1A = TCNT1 + compareA_off;
    modeA = 0;
    PORTC = 0b00000000;
  }
  else
  {
    OCR1A = TCNT1 + compareA_on;
    modeA = 1;
    PORTC = 0b00100000;
  }

}
// USART Received Data Complete interrupt service routine

//ISR(USART_RX_vect)










