


#include <avr/pgmspace.h> 

#include <LiquidCrystal.h>
#include <DFR_Key.h>
#include <JOBDictionary.h>

#define MAXWORDS 2000 //10000
#define THRESHOLD 1
#define MAXSAVEDWORDS 20
#define MAXWORDSIZE   21
#define UP 3
#define DOWN 4
#define LEFT 2
#define RIGHT 5

void printWord(char* newWord);
void lcdDisplay( char* line1, char* line2);
void chooseWord();


char newWord[30] ="";
char newWord2[30]="";
char oldWord[MAXSAVEDWORDS][MAXWORDSIZE];
int  wordCount=0;

volatile unsigned char radState = LOW;
static unsigned int wordNumber = 0;
volatile int radHits = 0;
volatile uint_farptr_t index_base;
volatile uint_farptr_t word_base;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 

DFR_Key keypad;

int localKey = 0;
long wordNum = 0;
int waitTime = 0;
int displayedWord = 0;
String keyString = "";
volatile unsigned int theIndex = 0;



void setup() 
{ 
  initializeOldWordQueue();
  attachInterrupt(0,chooseWord,RISING);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("JoeObjBox V0.1");
  Serial.println("JobObjBox V0.1");

  //dictionary0.init();
  delay(1500);
  randomSeed(analogRead(A5));
  /*
  OPTIONAL
   keypad.setRate(x);
   Sets the sample rate at once every x milliseconds.
   Default: 10ms
   */

  lcd.clear();
  lcd.print("Ready:");

  delay(1000);
}

void loop() 
{ 


  localKey = keypad.getKey();
  if(localKey>0) doKey();

  if(wordNumber >= MAXWORDS)
    wordNumber = 0;
  if(radState){


    strcpy_P(newWord,(char*)pgm_read_word(&(dictionary0[wordNumber]))); 
  }

  rotateQueue();
  displayedWord = 0;
  strcpy(oldWord[0],newWord);
  strncpy(newWord,"",1);
  if(wordCount == 0) lcdDisplay(oldWord[0],"");
  else lcdDisplay(oldWord[0],oldWord[1]);

  //Serial.println(oldWord[0]);
  delay(500);
  radState = LOW;
  if(wordCount < MAXSAVEDWORDS-1) wordCount++;

  //  }
  wordNumber++;
}

void lcdDisplay(char* line1, char* line2){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void chooseWord() {
  radHits++;
  if(radHits>=THRESHOLD){
    radState = HIGH;
    radHits=0;
  }

}

char* strcpy_PX(char* des, uint_farptr_t src,unsigned long base)
{
  unsigned long p = base + src;
  char junk[100];
  char* j = junk;
  char* s = des;
  do
  {
    *j = pgm_read_byte(p++);   
  }  
  while (*j++);

  do {
    *s = pgm_read_byte(p++);
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
      if(displayedWord<=(MAXSAVEDWORDS-1)) displayedWord++;
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
      break;
    }
  }

  delay(500);
}

void rotateQueue(){
  for(int x=(MAXSAVEDWORDS-1);x>0;x--) strcpy(oldWord[x],oldWord[x-1]);

}

void speakWordQueue(){
  for(int x=0;x<wordCount;x++){
    Serial.println(oldWord[x]);
    delay(1000);
  } 
}


void initializeOldWordQueue(){

  for(int x=0;x<MAXSAVEDWORDS;x++){
    for(int y=0;y<MAXWORDSIZE;y++){
      oldWord[x][y]='\0';
    }
  }

}





