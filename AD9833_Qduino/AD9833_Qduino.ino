/*
AD9837 Pro Generator sample code
This was written in Arduino 1.0.1,
for an Arduino Pro Mini, 5V, 16MHz
Pete Dokter, 9/2/12
Remixed by Anne Mahaffey, 10/8/12
ReRemixed by sinneb, 15th of april 2013

The connections to the AD9837 board are:

FSYNC -> 2
SCLK -> 13 (SCK)
SDATA -> 11 (MOSI)
MCLK -> 9 (Timer1) *** NOT USING ** USING A CRYSTAL
+Vin = VCC on Pro Micro
GND -> GND

This code bears the license of the beer. If you make money off of this,
you gotta beer me.
*/

//#define PROCESSORCLOCK 998999 //works for square with 16Mhz clock
#define PROCESSORCLOCK 12200000 


//int freq; //32-bit global frequency variable
long f = 440;
int expo = -24;
boolean updown = true;
boolean initial = true;
#include <SPI.h>



// Define the FSYNC (used for SD funtion)
#define FSYNC 8

// SPISettings mySettings(8000000,MSBFIRST,SPI_MODE2); // for Qduino

void setup()
{
  
  pinMode(FSYNC, OUTPUT); //FSYNC

  // Serial.begin(9600); // start serial communication at 9600bps
  // Serial.println("\n\nStartup\n\n");
  digitalWrite(FSYNC, HIGH);

  SPI.setDataMode(SPI_MODE2); // requires SPI Mode for AD9837
  //SPI.begin();
  SPI.begin();
  //SPI.beginTransaction(SPISettings(8000000,MSBFIRST,SPI_MODE2));

  delay(1000); //A little set up time, just to make sure everything's stable

  //Initial frequency
 // freq = 440;
  ResetRegisterAD9837();
  WriteRegisterAD9837(0x2100);
 // WriteFrequencyAD9837(freq);

  //Serial.print("Frequency is ");
  //Serial.print(freq);
  //Serial.println("");

}

void loop()
{
  
  if(initial) WriteFrequencyAD9837(f);
  initial = false;
  //f = 440.0 * pow(1.059463094359,expo);
  //if(updown)expo++;
  //else expo--;
  //if(expo > 35) updown = false;
  //else if(expo < -23) updown = true;
  
}
void WriteFrequencyAD9837(long frequency)
{
  //
  int MSB;
  int LSB;
  int phase = 0;

  //We can't just send the actual frequency, we have to calculate the "frequency word".
  //This amounts to ((desired frequency)/(reference frequency)) x 0x10000000.
  //calculated_freq_word will hold the calculated result.
  long calculated_freq_word;
  double AD9837Val = 0.0;
  long twotwentyeight = 2;
  twotwentyeight <<= 28;

  AD9837Val = (double)frequency * (double)(twotwentyeight / PROCESSORCLOCK) ;
  calculated_freq_word = (long)AD9837Val;

  /*
  Serial.println("");
  Serial.print("Frequency word is ");
  Serial.print(calculated_freq_word);
  Serial.println("twotwentyeight is");
  Serial.println(twotwentyeight);
  Serial.println("");
  */
  //Once we've got that, we split it up into separate bytes.

  calculated_freq_word &= 0x0FFFFFFF; //zero out 4 high bits
  long LMSB = calculated_freq_word & 0x0FFFC000; // take top 14 bits
  MSB = (int)((LMSB >> 14) & 0x0000FFFF); // and put them into a 16 bit int
  long LLSB = calculated_freq_word & 0x00003FFF;//take lower 14 bits
  LSB = (int)(LLSB & 0x0000FFFF); //put in 16 bit int


  //Set control bits DB15 ande DB14 to 0 and one, respectively, for frequency register 0
  LSB |= 0x4000;
  MSB |= 0x4000;

  phase |= 0xC000;

  //WriteRegisterAD9837(0x2100);

  //delay(500);

  //Set the frequency==========================
  WriteRegisterAD9837(LSB); //lower 14 bits

  WriteRegisterAD9837(MSB); //upper 14 bits

  WriteRegisterAD9837(phase); //mid-low

  //Power it back up
  WriteRegisterAD9837(0x2020); //square
  //WriteRegisterAD9837(0x2000); //sin
  //WriteRegisterAD9837(0x2002); //triangle

}

//This is the guy that does the actual talking to the AD9837
void WriteRegisterAD9837(int dat)
{
  digitalWrite(FSYNC, LOW); //Set FSYNC low
  SPI.transfer(highByte(dat)); Serial.println(highByte(dat));
  SPI.transfer(lowByte(dat)); Serial.println(lowByte(dat));
  digitalWrite(FSYNC, HIGH); //Set FSYNC high
  delay(1);
  
}


void ResetRegisterAD9837()
{

  Serial.println("Resetting registers");
  digitalWrite(FSYNC, LOW); //Set FSYNC low
  delay(10);

  SPI.transfer(0x01);
  SPI.transfer(0x00);

  digitalWrite(FSYNC, HIGH); //Set FSYNC high
}

