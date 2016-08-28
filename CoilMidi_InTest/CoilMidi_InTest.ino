#define OUTPIN  A5 //Output pin to drive coil.
#define NOTELIM  120 // Highest MIDI note to output.
#define PWLIM  200 //Pulse Width limit in us.
#define PWMULT  3 //  multiplier applied to velocity to get pulse width .
#define NOTEBASE 45
#define ONTIME 50
#define D1PIN 8
#define D2PIN 10
#define SIGNALPIN 12
#define O_FF 1
#define ON 2
#define WAIT 3


//Include libraries.

#include <MIDI.h>
#include <TimerOne.h>

//Define Global Variables

unsigned int  midiperiod[128]; // Array to store midi note periods
unsigned int  pw = PWLIM;            // Current Pulse width
unsigned int  sequence = NOTEBASE;
boolean       doNote = false;
long          noteTime = 0;
byte          note = 0;
byte          velocity = 0;
byte          incomingByte = 0;
byte          action = WAIT;

void setup(void){

  // Prepare Timer1
  Timer1.initialize(1000000);
  noteTime = millis();
  doNote   = false;

  //  Set the pin we want the ISR to toggle for output.
  pinMode(OUTPIN,OUTPUT);
  pinMode(D1PIN,OUTPUT);
  pinMode(D2PIN,OUTPUT);
  pinMode(SIGNALPIN,OUTPUT);

  //Start up the serial port
  Serial.begin(31250);



  //Define Midi Note Periods

  midiperiod[0] =122312;
  midiperiod[1] =115447;
  midiperiod[2] =108968;
  midiperiod[3] =102852;
  midiperiod[4] =97079;
  midiperiod[5] =91631;
  midiperiod[6] =86488;
  midiperiod[7] =81634;
  midiperiod[8] =77052;
  midiperiod[9] =72727;
  midiperiod[10] =68645;
  midiperiod[11] =64793;
  midiperiod[12] =61156;
  midiperiod[13] =57724;
  midiperiod[14] =54484;
  midiperiod[15] =51426;
  midiperiod[16] =48540;
  midiperiod[17] =45815;
  midiperiod[18] =43244;
  midiperiod[19] =40817;
  midiperiod[20] =38526;
  midiperiod[21] =36364;
  midiperiod[22] =34323;
  midiperiod[23] =32396;
  midiperiod[24] =30578;
  midiperiod[25] =28862;
  midiperiod[26] =27242;
  midiperiod[27] =25713;
  midiperiod[28] =24270;
  midiperiod[29] =22908;
  midiperiod[30] =21622;
  midiperiod[31] =20408;
  midiperiod[32] =19263;
  midiperiod[33] =18182;
  midiperiod[34] =17161;
  midiperiod[35] =16198;
  midiperiod[36] =15289;
  midiperiod[37] =14431;
  midiperiod[38] =13621;
  midiperiod[39] =12856;
  midiperiod[40] =12135;
  midiperiod[41] =11454;
  midiperiod[42] =10811;
  midiperiod[43] =10204;
  midiperiod[44] =9631;
  midiperiod[45] =9091;
  midiperiod[46] =8581;
  midiperiod[47] =8099;
  midiperiod[48] =7645;
  midiperiod[49] =7215;
  midiperiod[50] =6810;
  midiperiod[51] =6428;
  midiperiod[52] =6067;
  midiperiod[53] =5727;
  midiperiod[54] =5405;
  midiperiod[55] =5102;
  midiperiod[56] =4816;
  midiperiod[57] =4545;
  midiperiod[58] =4290;
  midiperiod[59] =4050;
  midiperiod[60] =3822;
  midiperiod[61] =3608;
  midiperiod[62] =3405;
  midiperiod[63] =3214;
  midiperiod[64] =3034;
  midiperiod[65] =2863;
  midiperiod[66] =2703;
  midiperiod[67] =2551;
  midiperiod[68] =2408;
  midiperiod[69] =2273;
  midiperiod[70] =2145;
  midiperiod[71] =2025;
  midiperiod[72] =1911;
  midiperiod[73] =1804;
  midiperiod[74] =1703;
  midiperiod[75] =1607;
  midiperiod[76] =1517;
  midiperiod[77] =1432;
  midiperiod[78] =1351;
  midiperiod[79] =1276;
  midiperiod[80] =1204;
  midiperiod[81] =1136;
  midiperiod[82] =1073;
  midiperiod[83] =1012;
  midiperiod[84] =956;
  midiperiod[85] =902;
  midiperiod[86] =851;
  midiperiod[87] =804;
  midiperiod[88] =758;
  midiperiod[89] =716;
  midiperiod[90] =676;
  midiperiod[91] =638;
  midiperiod[92] =602;
  midiperiod[93] =568;
  midiperiod[94] =536;
  midiperiod[95] =506;
  midiperiod[96] =478;
  midiperiod[97] =451;
  midiperiod[98] =426;
  midiperiod[99] =402;
  midiperiod[100] =379;
  midiperiod[101] =358;
  midiperiod[102] =338;
  midiperiod[103] =319;
  midiperiod[104] =301;
  midiperiod[105] =284;
  midiperiod[106] =268;
  midiperiod[107] =253;
  midiperiod[108] =239;
  midiperiod[109] =225;
  midiperiod[110] =213;
  midiperiod[111] =201;
  midiperiod[112] =190;
  midiperiod[113] =179;
  midiperiod[114] =169;
  midiperiod[115] =159;
  midiperiod[116] =150;
  midiperiod[117] =142;
  midiperiod[118] =134;
  midiperiod[119] =127;
  midiperiod[120] =119;
  midiperiod[121] =113;
  midiperiod[122] =106;
  midiperiod[123] =100;
  midiperiod[124] =95;
  midiperiod[125] =89;
  midiperiod[126] =84;
  midiperiod[127] =80;





 // MIDI.begin();  //   Launch MIDI with default options
 // MIDI.turnThruOff();

  for(int i=0;i<3;i++){
    flashDiode(D1PIN);
    delay(100);
  }
  for(int i=0;i<3;i++){
    flashDiode(D2PIN);
    delay(100);
  }


}

void flashDiode(int inpin) {

  analogWrite(inpin, 255);
  delay(10);
  analogWrite(inpin,0); 
}


void pulse(){

  analogWrite(OUTPIN,255);
  delayMicroseconds(pw);
  analogWrite(OUTPIN,0);

}

void loop(){

  //long noteTim = 0;
  //analogWrite(OUTPIN,0);

  /*noteTim = millis();
   
   if((noteTime+ONTIME) < noteTim){
   noteTime = millis();
   if(!doNote) {
   //  Serial.print(sequence);
   //  Serial.println(" OFF");
   Timer1.detachInterrupt();
   doNote = true;
   } 
   else {
   
   sequence++;
   if (sequence > NOTELIM ) {
   sequence = NOTEBASE; 
   }
   //    Serial.print(sequence);
   //    Serial.println(" ");
   //   Serial.print(midiperiod[sequence]);
   //   Serial.println(" ON");
   Timer1.attachInterrupt(pulse,midiperiod[sequence]);
   doNote = false;
   } 
   }
   */
  //Check for availible midi data
/*
  if (MIDI.read()) {

  // flashDiode(D1PIN);
  if(MIDI.check()) {
    flashDiode(D1PIN);
  } else {
    flashDiode(D2PIN);
  }

    switch(MIDI.getType()) {        //   Get the type of the message we caught
    case NoteOn:


      // get note on number and check for limit
      controlnote = MIDI.getData1();
      flashDiode(D1PIN);
      if (controlnote > NOTELIM){
        controlnote = NOTELIM;
      }


      Timer1.attachInterrupt(pulse,midiperiod[controlnote]);


      // Calculate Pulse width from velocity and Limit to P WLIM
      pw = PWMULT * MIDI.getData2();
      if (pw > PWLIM){
        pw = PWLIM;

      }



      break;
    case NoteOff:
      flashDiode(D2PIN);
      //Disable the interupt.
     // Timer1.detachInterrupt();
      break;


    default:
      break;
    }
  }

*/
 if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // wait for as status-byte, channel 1, note on or off
    if (incomingByte== 144) // Note on
    { 
      action = O_FF;
      flashDiode(D1PIN);
    }
    else if (incomingByte== 128) // Note off
    { 
      flashDiode(D2PIN);
      action = ON;
    }
    else if (note==0 && action != WAIT) // note on, wait for note value
    { 
      note=incomingByte;
    }
    else if (note!=0 && action != WAIT)  // velocity
    { 
      velocity=incomingByte;    
      pw = PWMULT * velocity;
      if (pw > PWLIM){
        pw = PWLIM;
      }
      if(action == O_FF){ 
//Midi_Send(0x90,note,velocity); 
        Timer1.attachInterrupt(pulse,midiperiod[note]);
        digitalWrite(SIGNALPIN,HIGH);
      }
      if(action == ON){ 
//        Midi_Send(0x80,note,velocity); 
        Timer1.detachInterrupt();
        digitalWrite(SIGNALPIN,LOW);
      }
      note=0;
      velocity=0;
      action=WAIT;
    }
    else{
    }
  }

}
 




