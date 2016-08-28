/* Midi In Basic  0.2 // kuki 8.2007
 *
 * -----------------
 * listen for midi serial data, and light leds for individual notes

 IMPORTANT:
 your arduino might not start if it receives data directly after a reset, because the bootloader thinks you want to uplad a new progam.
 you might need to unplug the midi-hardware until the board is running your program. that is when that statusLed turns on.

#####################################################################################################################################################
SOMETHING ABOUT MIDI MESSAGES
 midi messages start with one status byte followed by 1 _or_ 2 data bytes, depending on the command

 example midi message: 144-36-100
   the status byte "144" tells us what to do. "144" means "note on".
   in this case the second bytes tells us which note to play (36=middle C)
   the third byte is the velocity for that note (that is how powerful the note was struck= 100)
  
 example midi message: 128-36
   this message is a "note off" message (status byte = 128). it is followed by the note (data byte = 36)
   since "note off" messages don't need a velocity value (it's just OFF) there will be no third byte in this case
   NOTE: some midi keyboards will never send a "note off" message, but rather a "note on with zero velocity"
  
 do a web search for midi messages to learn more about aftertouch, poly-pressure, midi time code, midi clock and more interesting things.
#####################################################################################################################################################

HARDWARE NOTE:
The Midi Socket is connected to arduino RX through an opto-isolator to invert the midi signal and seperate the circuits of individual instruments.
connect 8 leds to pin2-pin9 on your arduino.

####################################################################################################################################################


 */

//variables setup

byte incomingByte;
byte note;
byte velocity;


int statusLed = 13;   // select the pin for the LED

int action=2; //0 =note off ; 1=note on ; 2= nada


//setup: declaring iputs and outputs and begin serial
void setup() {
  pinMode(statusLed,OUTPUT);   // declare the LED's pin as output
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  
  //start serial with midi baudrate 31250 or 38400 for debugging
  Serial.begin(31250);        
  digitalWrite(statusLed,HIGH);  
}

//loop: wait for serial data, and interpret the message
void loop () {
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // wait for as status-byte, channel 1, note on or off
    if (incomingByte== 144){ // note on message starting starting
      action=1;
    }else if (incomingByte== 128){ // note off message starting
      action=0;
    }else if (incomingByte== 208){ // aftertouch message starting
       //not implemented yet
    }else if (incomingByte== 160){ // polypressure message starting
       //not implemented yet
    }else if ( (action==0)&&(note==0) ){ // if we received a "note off", we wait for which note (databyte)
      note=incomingByte;
      playNote(note, 0);
      note=0;
      velocity=0;
      action=2;
    }else if ( (action==1)&&(note==0) ){ // if we received a "note on", we wait for the note (databyte)
      note=incomingByte;
    }else if ( (action==1)&&(note!=0) ){ // ...and then the velocity
      velocity=incomingByte;
      playNote(note, velocity);
      note=0;
      velocity=0;
      action=0;
    }else{
      //nada
    }
  }
}

void blink(){
  digitalWrite(statusLed, HIGH);
  delay(100);
  digitalWrite(statusLed, LOW);
  delay(100);
}


void playNote(byte note, byte velocity){
  int value=LOW;
  if (velocity >10){
      value=HIGH;
  }else{
   value=LOW;
  }

 //since we don't want to "play" all notes we wait for a note between 36 & 44
 if(note>=36 && note<44){
   byte myPin=note-34; // to get a pinnumber between 2 and 9
   digitalWrite(myPin, value);
 }

}

 

