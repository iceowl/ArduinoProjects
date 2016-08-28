


//#include <avr/pgmspace.h> 
//#include <avr/morepgmspace.h>

#include <LiquidCrystal.h>
#include <DFR_Key.h>
#include <JOBDictionary.h>

#define EMFPIN A4
#define EMFTHRESHOLDPIN A1
#define EMFMODEPIN 12
#define MAXWORDS 2744 //10000
#define RADTHRESHOLD 5 // interrupt threshold # of hits
#define MAXSAVEDWORDS 20
#define MAXWORDSIZE   16
#define UP 3
#define DOWN 4
#define LEFT 2
#define RIGHT 5
#define SELECT 1
#define EMFREQUENCY 100
#define MAXRESETTIME 10000
#define STARTUPDELAY 2000
#define INTERRUPTPIN 2
#define EMFRECORDS 20
#define EMFTHRESHSENSE 13
#define EMFSENSE 15
#define OLDAVERESETTIME 2000

void printWord(char* newWord);
void lcdDisplay( char* line1, char* line2);
void chooseWord();


char newWord[30] ="";
char newWord2[30]="";
char oldWord[MAXSAVEDWORDS][MAXWORDSIZE];
int  wordCount=0;

volatile unsigned char radState = LOW;
volatile unsigned char randWord = LOW;
static unsigned int wordNumber = 0;
volatile int radHits = 0;
unsigned char okToStart = LOW;
unsigned char emfMode = LOW;
unsigned char needRefresh = LOW;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 


DFR_Key keypad;

int localKey = 0;
long wordNum = 0;
int waitTime = 0;
int displayedWord = 0;

String keyString = "";
volatile unsigned int theIndex = 0;

unsigned long startTime = 0;
unsigned long emfTimer = 0;
unsigned long maxTimer = 0;

int emf = 0;
int maxE = 0;
int emfRecords[EMFRECORDS];
int emfCount = 0;
unsigned long emfAverage = 0;
int emfThreshold = 5;
int emfC = 0;
int oldEmfAverage = 0;


void setup() 
{ 

  initializeQueues();
  attachInterrupt(0,chooseWord,FALLING);
  pinMode(INTERRUPTPIN,INPUT_PULLUP);
  Serial.begin(9600);
  pinMode(EMFMODEPIN,INPUT);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Power Up  ");
  //Serial.println("Joe OBox Mini V1.0");
  delay(1500);
  //keypad.setRate(500);
  startTime = millis();
  emfTimer  = millis();
  maxTimer  = millis();
  lcd.clear();
  lcd.print("JoeOBoxMini V1.1");
  delay(500);


}

void loop() 
{ 
  analogWrite(A0,LOW);
  if(!okToStart) {
    if(millis() > startTime+STARTUPDELAY) {
      okToStart=HIGH;
    }
  }
  if(okToStart) {
    pinMode(A0,INPUT);
    localKey = keypad.getKey();
    if(localKey>0){ 
      doKey();
    }
    pinMode(A0,OUTPUT);
    readEMF();
    checkEMFMode();
    countEMFclicks();
    if(!emfMode && needRefresh){
      lcdDisplay(oldWord[0],oldWord[1]);
      needRefresh=LOW;
    }
  }

  if(!emfMode){
    if(radState){
      if(okToStart){
        uint16_t index = pgm_read_word(&(dictionary0[wordNumber]));
        // Serial.println(index,HEX);
        strcpy_P(newWord,(char*)pgm_read_word(&(dictionary0[wordNumber]))); 

        rotateQueue();
        displayedWord = 0;
        strcpy(oldWord[0],newWord);
        strncpy(newWord,"",1);
        if(wordCount == 0) lcdDisplay(oldWord[0],"");
        else lcdDisplay(oldWord[0],oldWord[1]);

        Serial.println(oldWord[0]);
        delay(500);
        if(wordCount < MAXSAVEDWORDS-1) wordCount++;
      } 

      radState = LOW;
      radHits = 0;
    }
  }
  wordNumber++;
  if(wordNumber >= MAXWORDS) {
    wordNumber = 0;
  }
}

void readEMF(){


  if(emfCount >= EMFRECORDS) {
    emfCount = 0;
  }


  emf = analogRead(EMFPIN); 
  emfC = constrain(emf,0,EMFSENSE);
  emfC = map(emfC,0,EMFSENSE,0,15);

  emfRecords[emfCount]=emfC;
  calculateEmfAverage();
  emfCount++;
}

void lcdDisplay(char* line1, char* line2){

  char packedLine[16] = "";


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
  if(radHits>=RADTHRESHOLD){
    radState = HIGH;
    radHits=0;
  }

}
void doKey(){


  switch(localKey) {
    case(DOWN): 
    {

      if(displayedWord>1){
        displayedWord--;
        lcdDisplay(oldWord[displayedWord-1],oldWord[displayedWord]);
      }

      break;
    }
    case(UP):
    {

      if(displayedWord<=(MAXSAVEDWORDS-1) && displayedWord < wordCount-1) {
        displayedWord++;
      }
      if(displayedWord<=1){
        displayedWord=2;
      }
      lcdDisplay(oldWord[displayedWord-1],oldWord[displayedWord]);
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
      radHits = 0;
      break;
    }
    case(SELECT):
    {
      clearEverything();
    }
  }

  delay(100);
}


void checkEMFMode(){


  if( digitalRead(EMFMODEPIN) == HIGH) {
    if(!emfMode) {
      emfMode = HIGH;
      lcdDisplay(""," ");
    }

    if(millis() > maxTimer+MAXRESETTIME) {
      maxE = 0;
      maxTimer = millis();
    }

    printEMF();

  }
  else {
    if(emfMode) {
      lcdDisplay(oldWord[0],oldWord[1]);
    }
    emfMode = LOW;
  }



}

void countEMFclicks() {
  char buf[5];
  int t = emfThreshold;
  emfThreshold = analogRead(EMFTHRESHOLDPIN);
  emfThreshold = map(emfThreshold,0,1023,0,EMFTHRESHSENSE);
  while(t!=emfThreshold){
    emfThreshold = analogRead(EMFTHRESHOLDPIN);
    emfThreshold = map(emfThreshold,0,1023,0,EMFTHRESHSENSE);
    t=emfThreshold;
    itoa(t,buf,10);
    lcdDisplay("Threshold",buf);
    delay(500);
    needRefresh = HIGH;
  }
  if(millis() > emfTimer + EMFREQUENCY){
    emfTimer = millis();
    if(emfAverage >= (oldEmfAverage + emfThreshold)){ 
      oldEmfAverage = emfAverage;
      radHits++;
      if(radHits >=RADTHRESHOLD){ 
        radHits=0;
        radState=HIGH;
      }
    }
    if((millis() > emfTimer + (OLDAVERESETTIME)) || (emfAverage < oldEmfAverage)){
      oldEmfAverage = emfAverage;

    }
  }
}

void printEMF() {

  char buf[16];
  char num[6];
  int x;



  lcd.setCursor(0, 0);
  strcpy(buf,"EM=");
  itoa(emfAverage,num,10);
  strcat(buf,num);
  int l = strlen(buf);
  for(x=0;x<8-l;x++){
    strcat(buf," ");
  }
  strcat(buf,"MAX=");
  itoa(maxE,num,10);
  strcat(buf,num);
  lcd.print(buf);

  lcd.setCursor(0,1);
  strcpy(buf,"");
  for( x=0;x<16;x++) {
    if(x<=emfAverage){
      strcat(buf,"-");
    }
    else {
      strcat(buf," ");
    }
  }
  lcd.setCursor(0,1);
  lcd.print(buf);

}

void rotateQueue(){
  for(int x=MAXSAVEDWORDS-1;x>0;x--) {
    strcpy(oldWord[x],oldWord[x-1]);
  }


}

void speakWordQueue(){
  for(int x=wordCount-1;x>=0;x--){
    Serial.println(oldWord[x]);
    lcdDisplay(oldWord[x],oldWord[x+1]);
    radState = LOW;
    delay(1000);
  } 
  radState = LOW;
}


void initializeQueues(){

  for(int x=0;x<MAXSAVEDWORDS;x++){
    for(int y=0;y<MAXWORDSIZE;y++){
      oldWord[x][y]='\0';
    }
  }

  for(int x=0;x<EMFRECORDS;x++) {
    emfRecords[x]=0;
  }

}

void calculateEmfAverage(){
  for(int x=0;x<EMFRECORDS;x++) {
    emfAverage +=emfRecords[x];
  }
  emfAverage = emfAverage/EMFRECORDS;
  if(emfAverage > maxE) {
    maxE = emfAverage;
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

  randWord = LOW;
  radState = LOW;
  wordNumber = 0;
  radHits = 0;

  localKey = 0;
  wordNum = 0;
  waitTime = 0;
  displayedWord = 0;
  keyString = "";
  lcdDisplay(" "," ");
}











































