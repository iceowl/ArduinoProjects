/*
TTS256 Speakjet Interface Chip Demo Sketch
Written by Ryan Owens
SparkFun Electronics

Uses the Voice Box Shield and the TTS256 Text To Speech Processor to turn text input on an Arduino to a voice output.

A speaker can be plugged directly into the SPK+ and - pins on the shield. See the tutorial for more information on how to connect the TTS256 to the Voice Box Shield.

SparkFun Product Links:
TTS256:
Arduino Duemilanove:
Voicebox Shield:
Speaker:
*/

//Soft serial library used to send serial commands on pin 2 instead of regular serial pin.
#include <SoftwareSerial.h>

//Define the Pin Numbers of the Voicebox shield for the sketch.
#define E0  5
#define E1  6
#define E2  7
#define E3  8
#define E4  9
#define E5  10
#define E6  11
#define E7  12

#define RDY  13
#define RES  3
#define SPK  4

//Pin 2 of the shield should be wired to the TTS256 chip.
#define txPin  2

//Create a SoftSerial Object to send strings to the TTS256 chip.
SoftwareSerial speakjet = SoftwareSerial(0, txPin);//rx tx

//Create a message buffer to hold the ascii message to be converted to sound
char message[128]="";

void setup()  
{
  //Configure the pins for the SpeakJet module
  pinMode(txPin, OUTPUT);
  pinMode(SPK, INPUT);
 
  
  //Set up a serial port to talk from Arduino to the SpeakJet module on pin 3.
  speakjet.begin(9600);    
  
  //Set up a serial port to get the ascii message from the host
  Serial.begin(9600);
  
  //Configure the Ready pin as an input
  pinMode(RDY, INPUT);
  
  //Configure Reset line as an output
  pinMode(RES, OUTPUT);
       
  //Configure all of the Event pins as outputs from Arduino, and set them Low.
  for(int i=E0; i<=E7; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  //Serial.println("Speak Easy On");
  //All I/O pins are configured. Reset the SpeakJet module
  digitalWrite(RES, LOW);
  delay(1000);
  digitalWrite(RES, HIGH);
  
}

void loop()
{  
  //Get a message from the serial port
  getMessage(message);
  
  //Send the message to the TTS256
  speakjet.print(message);
  //Serial.println(message);
  
  //Wait 12ms before checking the ready line (specified in TTS256 datasheet)
  delay(12);
  //Wait for the Speakjet to become 'ready' before sending more text.
  while(digitalRead(RDY)==0);
}

//Function: getMessage(char *)
//Description: Retrieves a string from the Serial port. Doesn't return the string until a carriage return character is detected.
//Inputs: None
//Outputs: char * message - The message received on the serial port.
//Returns: Nothing
//usage: getMessage(english_sentance);
void getMessage(char * message)
{
    char in_char=0;    //Create a character to store the incoming byte from the serial port.
    //Wait for a character to come into the serial port
    while(Serial.available() <=0);
    //Copy the incoming character to our variable.
    in_char=Serial.read();
    //Keep retreiving characters until the 'end of sentance(0x0D)' character is received.
    while(in_char != '\n'){
        *message++=in_char;    //Every time we receive a character we should add the character to the message string.
        while(Serial.available() <=0);    //Now wait for the next character...
        in_char = Serial.read();            //and copy it to the variable again.
    }
    *message='\0';    //Strings must end with a Null terminator so we need to add this to our message.
    return;
}
