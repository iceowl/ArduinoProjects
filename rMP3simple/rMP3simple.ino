#include <SoftwareSerial.h>
#include <RogueMP3.h>
#include <RogueSD.h>


// 30 second timeout
#define TIMER_MAX 30000
#define INPUT_PIN 8

SoftwareSerial rmp3_serial(6, 7);
RogueMP3 rmp3(rmp3_serial);
RogueSD filecommands(rmp3_serial);
int numberOfSongs;
int currentSong = 0;
boolean triggered = false;
boolean playing = false;
uint32_t triggerTimer = 0xffffffff - TIMER_MAX;
char filePath[96];


void setup()
{
  const char *directory = "/rMP3";
  Serial.println("Startup");
  pinMode(INPUT_PIN, INPUT);
  digitalWrite(INPUT_PIN, HIGH);

  Serial.begin(9600);

  rmp3_serial.begin(9600);

  // synchronize audio player
  rmp3.sync();
  rmp3.stop();

  // synchronize file system controller
  filecommands.sync();

  Serial.println("rMP3 Synchronized.");

  // get the number of songs available
  strcpy(filePath, directory);
  strcat(filePath, "/");
  strcat(filePath, "*.wav");

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
  filecommands.opendir(directory);

  Serial.println("Awaiting trigger.");
}

void loop()
{
  //rmp3.playfile("/0.wav");
  delay(2000);
}


