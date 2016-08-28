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
#include <SPI.h>
#include <Waveforms.h>

#define PROCESSORCLOCK 16000000
#define TWOPI4096 0.00153398078789
// Define the FSYNC (used for SD funtion)
#define FSYNC 8


double f = 440.0;
double n = 0.0;

int phase_number = 0;

enum wave_type {W_SINE,W_TRIANGLE,W_SAWTOOTH,W_SQUARE};

boolean initial  = true;






// SPISettings mySettings(8000000,MSBFIRST,SPI_MODE2); // for Qduino

void setup()
{

  pinMode(FSYNC, OUTPUT); //FSYNC
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

  
  long ph = 100 * waveformsTable[W_TRIANGLE][phase_number++];
  double phd = ph * 0.00153398078789;
  int phi = (int)phd;
  
  if(phase_number > 120) phase_number = 5-0;
  
 // delayMicroseconds(10);
  
  delay(85);

  double ff = 440.0 * pow(2.0, n / 12.0);
  n = n - 1.0;
  if (n < 0.0) n = 12.0;
  
    // if(initial) {
  WriteFrequencyAD9837(ff,phi);
  //  initial = false;
  //}

}
void WriteFrequencyAD9837(double frequency,int phase)
{
  //
  int MSB;
  int LSB;
  

  //We can't just send the actual frequency, we have to calculate the "frequency word".
  //This amounts to ((desired frequency)/(reference frequency)) x 0x10000000.
  //calculated_freq_word will hold the calculated result.

  //long twotwentyeight = 0x10000000;
  frequency += (frequency * 0.0487); // adjust for crystal?? rounding??
  frequency *= 16.0; // 2^28/PROCESSORCLOCK = 16
  long calculated_freq_word = (long)frequency;

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

  //Set the frequency==========================
  WriteRegisterAD9837(LSB); //lower 14 bits

  WriteRegisterAD9837(MSB); //upper 14 bits

  WriteRegisterAD9837(phase); //mid-low

  //Power it back up
  //WriteRegisterAD9837(0x2028); //square
 // WriteRegisterAD9837(0x2000); //sin
  WriteRegisterAD9837(0x2002); //triangle

}

//This is the guy that does the actual talking to the AD9837
void WriteRegisterAD9837(int dat)
{
  digitalWrite(FSYNC, LOW); //Set FSYNC low
  SPI.transfer(highByte(dat)); Serial.println(highByte(dat));
  SPI.transfer(lowByte(dat)); Serial.println(lowByte(dat));
  digitalWrite(FSYNC, HIGH); //Set FSYNC high

}


void ResetRegisterAD9837()
{

  //Serial.println("Resetting registers");
  digitalWrite(FSYNC, LOW); //Set FSYNC low
  delay(10);

  SPI.transfer(0x01);
  SPI.transfer(0x00);

  digitalWrite(FSYNC, HIGH); //Set FSYNC high
}

