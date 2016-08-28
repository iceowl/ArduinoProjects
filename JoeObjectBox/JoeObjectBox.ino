

/*

 Key Codes (in left-to-right order):
 
 None   - 0
 Select - 1
 Left   - 2
 Up     - 3
 Down   - 4
 Right  - 5
 
 */
#include <avr/morepgmspace.h> 
#include <JOBDictionary3_0.h>
#include <JOBDictionary3_0_1.h>
#include <LiquidCrystal.h>
#include <DFR_Key.h>
#include <RogueSD.h>
#include <RogueMP3.h>
#include <string.h>
//#include <JOBDictionary.h>

#define MAXWORDS 10000 //10000
#define DICT_SIZE 5191 //5193
#define THRESHOLD 7
#define MAXSAVEDWORDS 20
#define MAXWORDSIZE   21
#define GRAV_THRESHOLD 5
#define UP 3
#define DOWN 4
#define LEFT 2
#define RIGHT 5
#define SELECT 1
#define GRAVITY_PIN A8
#define GRAVITY_DETECT_PIN A13
#define GRAV_SETTLE_TIME 5000
#define RADTIMETHRESHOLD 1000
#define RADRATETHRESHOLD 500


RogueMP3 rmp3(Serial1);
RogueSD filecommands(Serial1);


char newWord[30] ="";
char filePath[96];
char oldWord[MAXSAVEDWORDS][MAXWORDSIZE];
int  oldSay[MAXSAVEDWORDS];
int  wordCount=0;
int  numberOfSongs = 0;

volatile unsigned char radState = LOW;
volatile unsigned char randWord = LOW;
volatile unsigned char okToGrav = LOW;
static unsigned int wordNumber = 0;
volatile  int radHits = 0;
volatile  int gravAve = 0;
volatile  int gravVal = 0;
volatile unsigned long radTime = 0;
volatile unsigned long oldRadTime = 0;
uint_farptr_t index_base;
uint_farptr_t word_base;


//Pin assignments for SainSmart LCD Keypad Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 
//---------------------------------------------

DFR_Key keypad;

int localKey = 0;
long wordNum = 0;
int waitTime = 0;
int displayedWord = 0;
String keyString = "";
unsigned long startMillis = 0;


void setup() 
{ 
  initializeOldWordQueue();
  pinMode(GRAVITY_PIN,INPUT);
  pinMode(GRAVITY_DETECT_PIN,INPUT);
  okToGrav = 0;
  attachInterrupt(0,chooseWord,RISING);
  // Serial.begin(9600);
  Serial1.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("JoeObjBox V0.6");
  //Serial.println("JobObjBox V0.1");
  startVoiceQueue();
  //dictionary0.init();

  delay(1000);
  randomSeed(analogRead(A15));
  startMillis = millis();
  radTime     = millis();
  /*
  OPTIONAL
   keypad.setRate(x);
   Sets the sample rate at once every x milliseconds.
   Default: 10ms
   */
  keypad.setRate(10);

  lcd.clear();
  delay(100);
  rmp3.setvolume(0);
  rmp3.playfile(" ","7269.mp3",0);
  lcd.print("Ready:");
}

void startVoiceQueue() {
  char buf[6];
  int  theError = 0;
  // synchronize audio player
  rmp3.sync();


  //Serial.println("rmp3Sync");
  //rmp3.stop();
  // Serial.println("rmp3stop");

  // synchronize file system controller
  filecommands.sync();

  //  Serial.println("rMP3 Synchronized.");
  //  int vers = rmp3.version();
  // Serial.print("Version :");
  //  Serial.println(vers);

  // get the number of songs available
  strcpy(filePath, "/*.mp3");
  //strcat(filePath, "/");
  //strcat(filePath, "*.wav");

  numberOfSongs = filecommands.filecount(filePath);

  if (numberOfSongs < 0)
  {
    // rMP3 error
    if (filecommands.LastErrorCode == 8)
      //      Serial.println("No card inserted.");
      lcdDisplay("   Insert Card","   Press Reset");
    //    Serial.println("Reset required to continue.");
    while(1){
    }
  }
  else
  {
    theError = filecommands.LastErrorCode;
    if(theError != 0){
      //    Serial.print("rMP3 Error Code: ");
      //    Serial.println(theError, HEX);
      //     Serial.println("Reset required to continue.");
      itoa(theError,buf,16);
      lcdDisplay("Reset - Error",buf);
      while(1){
      }
    }
  }



  //  Serial.print(numberOfSongs, DEC);
  //  Serial.println(" files available.");
  itoa(numberOfSongs,buf,10);
  lcdDisplay(buf,"files found");

  // rewind directory
  filecommands.opendir("/");

  //  Serial.println("Awaiting trigger.");
}



void loop() 
{ 


  /*
  keypad.getKey();
   Grabs the current key.
   Returns a non-zero integer corresponding to the pressed key,
   OR
   Returns 0 for no keys pressed,
   OR
   Returns -1 (sample wait) when no key is available to be sampled.
   */

  localKey = keypad.getKey();
  if(localKey>0) doKey();
  if(wordNumber >= MAXWORDS){
    wordNumber = 0;
  }
  checkRadState();
  if(radState){
    radState = LOW;
    if(randWord){
      wordNumber = random(0,10000);
      randWord= LOW;
    }
    if(wordNumber< DICT_SIZE){

      unsigned int index = pgm_read_word_far(GET_FAR_ADDRESS(dictionary0[0]) +wordNumber*2);
      strcpy_PX(newWord,index,PROGMEM_SEG2_BASE);
      //  theIndex = index_base0 + wordNumber;
      //  strcpy_PX(newWord, theIndex,PROGMEM_SEG1_BASE); 
    } 
    else {
      unsigned int index = pgm_read_word_far(GET_FAR_ADDRESS(dictionary0[0])+wordNumber*2);
      strcpy_PX(newWord,index,PROGMEM_SEG3_BASE);

      //      theIndex = index_base1 + ((wordNumber-DICT_SIZE));
      //      strcpy_PX(newWord, theIndex,PROGMEM_SEG2_BASE); 
    }

    strcpy(filePath,"");
    rotateQueue();
    displayedWord = 0;
    strcpy(oldWord[0],newWord);
    strncpy(newWord,"",1);
    oldSay[0] = wordNumber;

    char buf[10];
    itoa(wordNumber, buf,10);
    strcpy(filePath,buf);
    strcat(filePath,".mp3");
    rmp3.playfile(" ",filePath,0);
    //   Serial.println(filePath);
    //    Serial.println(oldWord[0]);
    //  Serial.println(wordNumber);

    if(wordCount == 0) lcdDisplay("",oldWord[0]);
    else lcdDisplay(oldWord[1],oldWord[0]);

    if(wordCount < MAXSAVEDWORDS-1) wordCount++;
    delay(1000);
  }
  wordNumber++;
  if(!okToGrav){
    unsigned long now = millis();
    if((now - startMillis) > GRAV_SETTLE_TIME)
      okToGrav = HIGH;
  } 
  int detect = analogRead(GRAVITY_DETECT_PIN);
  if(detect > 500)
    doGravPin();

}

void lcdDisplay(char* line1, char* line2){

  char packedLine[21] = "";


  lcd.clear();
  lcd.setCursor(0, 0);
  packLine(line1,packedLine);
  lcd.print(packedLine);
  strcpy(packedLine,"");
  lcd.setCursor(0, 1);
  packLine(line2,packedLine);
  lcd.print(packedLine);
}

void packLine(char* lIn, char* lOut){

  strcpy(lOut,"");

  int cntChars = 0;
  for(int x=0;x<(MAXWORDSIZE) && x<strlen(lIn);x++){
    if((lIn[x] != ' ') && (lIn[x] != '\0')) cntChars++;
  }

  if(cntChars < MAXWORDSIZE-1) {
    cntChars = MAXWORDSIZE - cntChars;
    cntChars = (cntChars/2)-2;
    if(cntChars>0)
      for(int x=0;x<cntChars;x++) strcat(lOut," ");
  }
  strcat(lOut,lIn);

}

void chooseWord() {
  radHits++;
  if(radHits>=THRESHOLD){
    radState = HIGH;
    radHits=0;
  }
}

void checkRadState() {
  unsigned long now = millis();
  if(radState && !randWord){
    if((now - radTime) > RADTIMETHRESHOLD){
      if(abs((now - radTime) - (oldRadTime)) > RADRATETHRESHOLD){
        oldRadTime = now - radTime;
        radTime = now;
      } 
      else {
        radState = LOW;
      }
    } 
    else {
      radTime =now;
      radState = LOW;
    }
  }
}

char* strcpy_PX(char* des, uint_farptr_t src,unsigned long base)
{

  char* s = des;
  unsigned long p = src + base;

  do {
    *s = pgm_read_byte_far(p++);
  }
  while(*s++);
  return des;
} 


void doKey(){


  switch(localKey) {
    case(UP): 
    {
      if(displayedWord>0){
        displayedWord--;
        lcdDisplay(oldWord[displayedWord],oldWord[displayedWord-1]);
      }
      break;
    }
    case(DOWN):
    {
      if(displayedWord<=(MAXSAVEDWORDS-1) && displayedWord < wordCount-1) displayedWord++;
      lcdDisplay(oldWord[displayedWord],oldWord[displayedWord-1]);
      break;
    }
    case(LEFT):
    {
      speakWordQueue();
      break;
    }
    case(RIGHT):
    {
      radState=HIGH;
      randWord= HIGH;
      radHits = THRESHOLD;
      break;
    }
    case(SELECT):
    {
      clearEverything();
    }
  }

  delay(50);
}

void rotateQueue(){
  for(int x=(MAXSAVEDWORDS-1);x>0;x--) {
    strcpy(oldWord[x],oldWord[x-1]);
    oldSay[x] = oldSay[x-1];
  }


}

void speakWordQueue(){
  // noInterrupts();
  char buf[10];
  for(int x=wordCount-1;x>=0;x--){
    //    Serial.println(oldWord[x]);
    itoa(oldSay[x], buf,10);
    strcpy(filePath,buf);
    strcat(filePath,".mp3");
    rmp3.playfile(" ",filePath,0);
    //    Serial.println(filePath);
    lcdDisplay(oldWord[x+1],oldWord[x]);
    delay(1000);
  } 
  //  interrupts();
}


void initializeOldWordQueue(){

  for(int x=0;x<MAXSAVEDWORDS;x++){
    for(int y=0;y<MAXWORDSIZE;y++){
      oldWord[x][y]='\0';
    }
  }

}

void doGravPin(){

  int gravIn = analogRead(GRAVITY_PIN);
  gravAve = (gravAve+gravIn)/2;
  if(okToGrav && (abs(gravIn - gravAve) > GRAV_THRESHOLD)){
    radState = HIGH;
    okToGrav = 0;
    startMillis = millis();
  }
}

void clearEverything(){
  newWord[0]='\0';
  for(int x=0;x<MAXSAVEDWORDS;x++){
    for(int y=0;y<MAXWORDSIZE;y++){
      oldWord[x][y] = '\0';
    }
  }
  wordCount=0;

  radState = LOW;
  randWord = LOW;

  wordNumber = 0;
  radHits = 0;

  localKey = 0;
  wordNum = 0;
  waitTime = 0;
  displayedWord = 0;
  keyString = "";
  lcdDisplay(" "," ");
}












