/*
 ML8511 UV Sensor Read Example
 By: Nathan Seidle
 SparkFun Electronics
 Date: January 15th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 The ML8511 UV Sensor outputs an analog signal in relation to the amount of UV light it detects.

 Connect the following ML8511 breakout board to Arduino:
 3.3V = 3.3V
 OUT = A0
 GND = GND
 EN = 3.3V
 3.3V = A1
 These last two connections are a little different. Connect the EN pin on the breakout to 3.3V on the breakout.
 This will enable the output. Also connect the 3.3V pin of the breakout to Arduino pin 1.

 This example uses a neat trick. Analog to digital conversions rely completely on VCC. We assume
 this is 5V but if the board is powered from USB this may be as high as 5.25V or as low as 4.75V:
 http://en.wikipedia.org/wiki/USB#Power Because of this unknown window it makes the ADC fairly inaccurate
 in most cases. To fix this, we use the very accurate onboard 3.3V reference (accurate within 1%). So by doing an
 ADC on the 3.3V pin (A1) and then comparing this against the reading from the sensor we can extrapolate
 a true-to-life reading no matter what VIN is (as long as it's above 3.4V).

 Test your sensor by shining daylight or a UV LED: https://www.sparkfun.com/products/8662

 This sensor detects 280-390nm light most effectively. This is categorized as part of the UVB (burning rays)
 spectrum and most of the UVA (tanning rays) spectrum.

 There's lots of good UV radiation reading out there:
 http://www.ccohs.ca/oshanswers/phys_agents/ultravioletradiation.html
 https://www.iuva.org/uv-faqs

*/
#include <SimbleeForMobile.h>
int            UVOUT = 5; //Output from the sensor
int            lowUV = 2048;
double         high  = 0.0;
double         accum_exposure       = 0.0;
uint8_t        intensity;
uint8_t        accumulated_exposure;
uint8_t        high_exposure;
unsigned long  savedTime   = 0;
// include newlib printf float support (%f used in sprintf below)
asm(".global _printf_float");

void setup() {


  pinMode(UVOUT, INPUT);

  SimbleeForMobile.advertisementData = "uv";

  // use a subdomain to create an application specific cache
  SimbleeForMobile.domain = "temp.Simblee.com";

  // establish a baseline to use the cache during development to bypass uploading
  // the image each time
  SimbleeForMobile.baseline = "Feb 24 2016";

  // start SimbleeForMobile
  SimbleeForMobile.begin();

  savedTime = millis();

  for (int i = 1; i < 10; i++) {
    int a = averageAnalogRead(UVOUT);
    if (a < lowUV) lowUV = a;
    delay(1000);
  }



}

void loop()
{


  int uvLevel  = averageAnalogRead(UVOUT);
  // int refLevel = averageAnalogRead(REF_3V3);


  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor

  uvLevel--; // make it say 0 at gnd level

  float outputVoltage =   (0.99 / lowUV) * (float)uvLevel;
  //float uvIntensity = mapfloat(uvLevel, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
  float uvIntensity = (outputVoltage - 0.99) * 8.287;
  
  accumulateIntensity(uvIntensity);
  if (uvIntensity > high) high = uvIntensity;


  if (SimbleeForMobile.updatable) {
    char buf[16];
    char buf2[24];
    char buf3[16];
    sprintf(buf,  "%.02f", uvIntensity);
    sprintf(buf2, "%.04f", accum_exposure);
    sprintf(buf3, "%.02f", high);
    SimbleeForMobile.updateText(intensity, buf);
    SimbleeForMobile.updateText(accumulated_exposure, buf2);
    SimbleeForMobile.updateText(high_exposure, buf3);
  }

  // process must be called in the loop for SimbleeForMobile
  SimbleeForMobile.process();
  Simblee_ULPDelay(500);
}


//Takes an average of readings on a given pin
//Returns the average
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0;

  for (int x = 0 ; x < numberOfReadings ; x++) {
    runningValue += analogRead(pinToRead);
  }

  runningValue /= numberOfReadings;

  return (runningValue);
}

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
//float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
//{
//return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

//}


void accumulateIntensity(float uvI) {

  if (uvI >= 0.01) {
    unsigned long exposedTime = millis() - savedTime;
    double accumulated_hours = (exposedTime / 3600000.0);
    accum_exposure += (uvI * accumulated_hours);
    
  }
  savedTime = millis();
  return;
}

void ui() {

  SimbleeForMobile.beginScreen(BLACK);
  SimbleeForMobile.drawText(160, 160, "mW/cm^2", WHITE, 20);
  intensity = SimbleeForMobile.drawText(10, 140, "", WHITE, 45);
  SimbleeForMobile.drawText(160, 240, "mW/cm^2 * hour", WHITE, 20);
  accumulated_exposure = SimbleeForMobile.drawText(10, 240, "", WHITE, 20);
  SimbleeForMobile.drawText(160, 340, "mW/cm^2 MAX", WHITE, 20);
  high_exposure = SimbleeForMobile.drawText(10, 320, "", WHITE, 45);

  SimbleeForMobile.endScreen();

}
void SimbleeForMobile_onConnect() {
}
void ui_event(event_t &event) {
}
