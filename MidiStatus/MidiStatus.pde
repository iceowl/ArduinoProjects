/*
WRITTEN BY:           TOM SCARFF
DATE:                 9/9/2008
FILE SAVED AS:        midi_analyser.pde
FOR:                  Miduino Atmega168
CLOCK:                16.00 MHz CRYSTAL                                        
PROGRAMME FUNCTION:   To read MIDI input and light appropriate LEDS


 IMPORTANT:
 your Miduino might not start if it receives data directly after a reset,
 because the bootloader thinks you want to upload a new progam.You might 
 need to unplug the midi-hardware until the board is running your program. 
 Also when programming the Miduino disconnect the MIDI IN cable.
 
HARDWARE NOTE:
The Midi IN Socket is connected to the Miduino RX through an 6N139 opto-isolator
 *
 * To send MIDI, attach a MIDI out Female 180 Degree 5-Pin DIN socket to Arduino.
 * Socket is seen from solder tags at rear.
 * DIN-5 pinout is:                                         _______ 
 *    pin 2 - Gnd                                          /       \
 *    pin 4 - 220 ohm resistor to +5V                     | 1     3 |  MIDI jack
 *    pin 5 - Arduino Pin 1 (TX) via a 220 ohm resistor   |  4   5  |
 *    all other pins - unconnected                         \___2___/
 *
________________________________________________________________________
 
   Message               Status Byte       Data Bytes 
 
Note-On Event            1001bbbb (9X)      0kkkkkkk  0vvvvvvv
 
Note-Off  Event          1000bbbb (8X)      0kkkkkkk 0vvvvvvv

Polyphonic Key Pressure  1010bbbb (AX)      0kkkkkkk  0fffffff

Control Change           1011bbbb (BX)     0ccccccc  0CCCCCCC
 
Program Change           1100bbbb (CX)     0nnnnnnn

Channel Pressure         1101bbbb (DX)      0ppppppp

Pitch Bend               1110bbbb (EX)     0iiiiiii       0hhhhhhh


Table 1: Channel Voice Messages
________________________________________________________________________ 



Begin System Exclusive 11110000 (F0) 0mmmmmmm  0xxxxxxx

MIDI  Time Code 11110001 (F1) 0tttdddd   

Song Position Pointer  11110010 (F2)  0jjjjjjj  0kkkkkkk

Song Select  11110011 (F3)  0sssssss

Tune Request  11110110 (F6)

End System Exclusive 11110111 (F7)
 
Timing Clock 11111000 (F8)
 
Start 11111010  (FA)

Continue 11111011 (FB)

Stop 11111100 (FC) 

Active Sensing  11111110 (FE)

System Reset 11111111 (FF)

Table 2: System Messages
_____________________________________________________________________________

Variable     Value Range
 
MIDI Channel bbbb 0000 to 1111 

Key Number kkkkkkk
 0000000 to 1111111
 
Velocity of Note On/Off  vvvvvvv
 0000000 to 1111111

Force On Key fffffff
 0000000 to 1111111
 
Address of Control (0-121) ccccccc
 0000000 to 1111111
 
Value of Control Output CCCCCCC
 0000000 to 1111111
 
Programme Select  nnnnnnn
 0000000 to 1111111

Pressure Value  ppppppp
 0000000 to 1111111

Pitch bend LSB  iiiiiii
 0000000 to 1111111

Pitch bend MSB  hhhhhhh
 0000000 to 1111111

Number of Channels  zzzzzzz
 0000000 to 1111111
 
Number of Song  sssssss
 0000000 to 1111111

Song Position Pointer LSB  jjjjjjj
 0000000 to 1111111

Song Position Pointer MSB  kkkkkkk
 0000000 to 1111111

Manufacturers Identification  mmmmmmm 0000000 to 1111111 

System Exclusive Message xxxxxxx 0000000 to 1111111 

MIDI Timecode quarter frame message ttt dddd

 000 to 111 
0000 to 1111
 

Table 3: MIDI Variables
____________________________________________________________________

 */



byte DEBUG=1;

byte LedPin =13;
byte midiByte;
byte channel=0;
byte velocity;
byte val;
byte velocityFlag=1;
byte noteOnFlag=0;
byte noteFlag=0;
byte statusTest;
byte channelTest;
byte status;
byte selectStatus;
byte Flag=0;
byte x;


void setup() {
  pinMode(2,INPUT); // switch status/channel
  digitalWrite(2, HIGH); // turn on pullup resistor

  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT); 
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(15,OUTPUT); 
  pinMode(16,OUTPUT); 
  pinMode(17,OUTPUT);
  pinMode(18,OUTPUT);
  pinMode(19,OUTPUT);
  
   for (x=1; x<=4; x++){
    digitalWrite( LedPin, HIGH );
    delay(300);
    digitalWrite( LedPin, LOW );
    delay(300);
    }


  Serial.begin(31250);   // set MIDI baud rate
  Serial.flush();
}

void loop() {

  selectStatus = digitalRead(2);   // read the input pin

  readMidi();

  channel= midiByte&0x0F;
  status= midiByte&0xF0;
  statusTest=midiByte&0x80;

  if(statusTest==0x80){        // if midi status and NOT data

    if(selectStatus){

      if(status==0xF0){
        allLedsOff();
        getStatus1(midiByte);  // complete status byte
      }

      else{
        allLedsOff();
        getStatus2(status); // status with channel removed
      }


    }
    //____________________________________________________

    else{
      allLedsOff();
      getChannel(channel); // status removed channel only
    }
  }

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void getStatus1(byte val){


  switch (val) {
  case 0xF0:                //Begin System Exclusive 11110000 (F0)
    digitalWrite(19, HIGH);
    break;
   
  case 0xF1:                //MIDI  Time Code 11110001 (F1)
   digitalWrite(18, HIGH);   
    break;
    
     case 0xF2:             //Song Position Pointer  11110010 (F2) 
   digitalWrite(17, HIGH);   
    break;
    
     case 0xF3:            //Song Select  11110011 (F3)
   digitalWrite(16, HIGH);   
    break;
    
   
     case 0xF7:           //End System Exclusive 11110111 (F7) 
   digitalWrite(15, HIGH);   
    break;
    
     case 0xF8:           //Timing Clock 11111000 (F8) 
   digitalWrite(14, HIGH);   
    break;
    
     case 0xFA:           //Start 11111010  (FA)
   digitalWrite(13, HIGH);   
    break;
    
     case 0xFB:          //Continue 11111011 (FB)
   digitalWrite(12, HIGH);   
    break;
    
     case 0xFC:          //Stop 11111100 (FC)
   digitalWrite(11, HIGH);   
    break;
    
     case 0xFE:          //Active Sensing  11111110 (FE)
   digitalWrite(10, HIGH);   
    break;
    
     case 0xFF:         //System Reset 11111111 (FF) 
   digitalWrite(9, HIGH);   
    break;

 


  default:
    break;
  }
}

//___________________________________________________

void getStatus2(byte val){


  switch (val) {
  case 0x90:                // Note-On Event            1001bbbb (9X)     
    digitalWrite(8, HIGH);
    break;
    // break is optional
  case 0x80:                //Note-Off  Event          1000bbbb (8X)
    digitalWrite(7, HIGH);
    break;
  
  case 0xB0:                //Control Change           1011bbbb (BX)
       digitalWrite(6, HIGH);
    
    break;
    
 case 0xC0:                //Program Change           1100bbbb (CX)
       digitalWrite(5, HIGH);
    
    break;
    
 case 0xD0:                //Channel Pressure         1101bbbb (DX)
       digitalWrite(4, HIGH);
    
    break;
    
  case 0xE0:                //Pitch Bend               1110bbbb (EX)
       digitalWrite(3, HIGH);
    
    break;


  default:
    break;
  }
}

//_____________________________________________________  

void getChannel(byte val){

  // MIDI channels Case 0 to Case 15 correspond to MIDI channels 1 to 16 respectively

  switch (val) {
  case 0:                //channel 1
    digitalWrite(3, HIGH);
    break;
    // break is optional
  case 1:
    digitalWrite(4, HIGH);
    break;

   case 2:
    digitalWrite(5, HIGH);
    break;
    // break is optional
  case 3:
    digitalWrite(6, HIGH);
    break;
    
     case 4:
    digitalWrite(7, HIGH);
    break;
    // break is optional
  case 5:
    digitalWrite(8, HIGH);
    break;
    
    
     case 6:
    digitalWrite(9, HIGH);
    break;
    // break is optional
  case 7:
    digitalWrite(10, HIGH);
    break;
    
    
     case 8:
    digitalWrite(11, HIGH);
    break;
    // break is optional
  case 9:
    digitalWrite(12, HIGH);
    break;
    
     case 10:
    digitalWrite(14, HIGH);
    break;
    // break is optional
  case 11:
    digitalWrite(15, HIGH);
    break;
    
     case 12:
    digitalWrite(16, HIGH);
    break;
    // break is optional
  case 13:
    digitalWrite(17, HIGH);
    break;
    
     case 14:
    digitalWrite(18, HIGH);
    break;
    // break is optional
  case 15:
    digitalWrite(19, HIGH);
    break;
    
    

  default:
    break;
  }
}

//_____________________________________________________  


void readMidi(){

  while (Flag==0){
    if (Serial.available() > 0) {  
      midiByte = Serial.read();
      Serial.print(midiByte,BYTE);          
      Flag=1;
    }
  }
  Flag=0;

}
//____________________________________________________


//+++++++++++++++++++++++++++++++++++++++++++++++++

void allLedsOff(){


  
  digitalWrite(3,LOW); // Set All outputs low
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  digitalWrite(7,LOW);
  digitalWrite(8,LOW); // Set All outputs low
  digitalWrite(9,LOW);
  digitalWrite(10,LOW);
  digitalWrite(11,LOW);
  digitalWrite(12,LOW);
  digitalWrite(13,LOW);
  digitalWrite(14,LOW);
  digitalWrite(15,LOW); 
  digitalWrite(16,LOW);
  digitalWrite(17,LOW);
  digitalWrite(18,LOW);
  digitalWrite(19,LOW);

}






