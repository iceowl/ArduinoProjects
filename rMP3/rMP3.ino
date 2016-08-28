/******************************************
 * rMP3 Trigger with Time-Out Example
 * 
 * rMP3 Control Requirements
 * Play songs in order when trigger is set.
 * If trigger clears, start timer and stop
 * playback if timer runs out.
 * 
 * Assumptions
 * When timer runs out, playback starts
 * at the beginning.
 * Playback will continue if trigger re-sets
 * before timer runs out.
 * 
 ******************************************/

#include <RogueSD.h>
#include <RogueMP3.h>
//#include <SoftwareSerial.h>


// 30 second timeout
#define TIMER_MAX 30000
#define INPUT_PIN 8

// Objects


RogueMP3 rmp3(Serial1);
RogueSD filecommands(Serial1);


// global variables
int numberOfSongs;
int currentSong = 0;
boolean triggered = false;
boolean playing = false;
uint32_t triggerTimer = 0xffffffff - TIMER_MAX;
char filePath[96];

// consts

void setup()
{

  pinMode(INPUT_PIN, INPUT);
  digitalWrite(INPUT_PIN, HIGH);

  Serial.begin(9600);
  while(!Serial){
  }
  Serial1.begin(9600);
  Serial.println("Startup");

  // synchronize audio player
  rmp3.sync();


  Serial.println("rmp3Sync");
  //rmp3.stop();
  // Serial.println("rmp3stop");

  // synchronize file system controller
  filecommands.sync();

  Serial.println("rMP3 Synchronized.");
  int vers = rmp3.version();
  Serial.print("Version :");
  Serial.println(vers);

  // get the number of songs available
  strcpy(filePath, "/*.mp3");
  //strcat(filePath, "/");
  //strcat(filePath, "*.wav");

  numberOfSongs = filecommands.filecount(filePath);

  if (numberOfSongs < 0)
  {
    // rMP3 error
    if (filecommands.LastErrorCode == 8)
      Serial.println("No card inserted.");
    else
    {
      Serial.print("rMP3 Error Code: ");
      Serial.println(filecommands.LastErrorCode, HEX);
    }

    Serial.println("Reset required to continue.");
    for (;;);
  }

  Serial.print(numberOfSongs, DEC);
  Serial.println(" files available.");

  // rewind directory
  filecommands.opendir("/");

  Serial.println("Awaiting trigger.");
  Serial.print("rMP3 Error Code: ");
  Serial.println(filecommands.LastErrorCode, HEX);
}

// Play next song, if we can
void playNextSong()
{
  char filename[80];
  char playback;

  if (playing == true)
  {
    if (filecommands.status() == 0)
    {
      // card is inserted and good to go
      if (currentSong == 0)
      {
        // rewind directory
        filecommands.opendir("/");
      }

      if (currentSong < numberOfSongs)
      {
        // get the next song
        filecommands.readdir(filename, "*.mp3");
        
        rmp3.playfile(" ",filename,0);
        char p = rmp3.getplaybackstatus();
        Serial.print("playback status: ");
        Serial.println(p);
        Serial.print("Playing: ");
        //Serial.print(directory);
        //Serial.print('/');
        Serial.println(filename);
        currentSong++;
      }
      else
      {
        playing = false;
        currentSong = 0;
      }
    }
    else
    {
      if (filecommands.LastErrorCode == 8)
      {
        Serial.println("No card inserted.");
      }
      else
      {
        Serial.print("rMP3 Error Code: ");
        Serial.println(filecommands.LastErrorCode, HEX);
      }

      Serial.println("Reset required to continue.");
      for (;;);
    }
  }
}


// This is the function to check the input
boolean checkTrigger(void)
{
  if (digitalRead(INPUT_PIN) == HIGH)
    return true;
  else
    return false;
}


/******************************************
 * Main loop
 ******************************************/

void loop()
{
  char rMP3Status = rmp3.getplaybackstatus();
  //rmp3.setvolume(0);
  // First, check the trigger

  if (checkTrigger())
  {
    if (triggered == false)
    {
      Serial.println("Trigger set.");
      if (playing == false)
      {
        // Start from the top
        playing = true;
        currentSong = 0;
      }
    }
    triggered = true;
  }
  else
  {
    if (triggered == true)
    {
      Serial.println("Trigger cleared.");

      triggered = false;
      triggerTimer = millis();
    }
    else
    {
      if ((millis() - triggerTimer) > TIMER_MAX)
      {
        if (playing == true)
        {
          playing = false;
          Serial.println("Playback stopped.");
        }

        if (rMP3Status == 'P')
        {
          // stop playback
          rmp3.stop();
        }
      }
    }
  }

  if (triggered == true || ((millis() - triggerTimer) < TIMER_MAX))
  {
    if (rMP3Status != 'P')
      playNextSong();
  }

  // Arbitrary delay
  delay(1000);
}




