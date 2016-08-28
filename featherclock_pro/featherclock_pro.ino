

#include <SPI.h>                              // SPI interface API
#include <Encoder.h>
#include <Wire.h>                             // Wire support library
#include <DS3231.h>
#include <MAX17043.h>                         //Battery fuel gauge
#include <Time.h>
#include "Adafruit_LEDBackpack.h"             // Support for the LED Backpack FeatherWing
#include "Adafruit_GFX.h"                     // Adafruit's graphics library
#define ENCODER_DO_NOT_USE_INTERRUPTS
#define SW                4
#define DISPLAY_ADDRESS   0x70
#define DEBOUNCETIME      125
#define SWPOWER           10
#define BRIGHTNESSDEFAULT 0
#define BRIGHTNESSPIN     9
#define TIMESETEXIT       60000 //exit time set mode in 1 minute
#define RED               A2
#define GREEN             A0
#define BLUE              A1

// Create display object


uint8_t          hours   = 0;                      // Track hours
uint8_t          minutes = 0;                    // Track minutes
uint8_t          seconds = 0;                    // Track seconds
enum             enc_state {up, down, unknown};
enum             clk_state {setHour, setMinute, runit};
long             position   = -999;
unsigned long    theTime    = 0;
boolean          blinkColon = false;            // Track the status of the colon to blink every second
boolean          ctlTurned  = false;
int              displayValue = 0;
int              brightness = BRIGHTNESSDEFAULT;
MAX17043         batteryMonitor;
Encoder          myEnc(5, 6);
DS3231           clock;
RTCDateTime      dt = {2000, 1, 1, 1, 1, 1, 0};
Adafruit_7segment clockDisplay = Adafruit_7segment();
enc_state eState  = up;
clk_state cState  = setHour;


void setup() {


  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  Serial.begin(9600);
  batteryMonitor.reset();
  batteryMonitor.quickStart();
  clockDisplay.begin(0x70);
  clockDisplay.setBrightness(brightness);
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH); // set pullup
  pinMode(SWPOWER, OUTPUT);
  pinMode(SWPOWER, LOW);
  pinMode(BRIGHTNESSPIN, INPUT);
  digitalWrite(BRIGHTNESSPIN, HIGH); //setpullup
  //rtc.begin();
  clock.setBattery(true, false);
  // Set sketch compiling time
  //clock.setDateTime(__DATE__, __TIME__);
  clearEOSCBit();
  readChronoDot();
  setDisplayValue();
  //checkBattery();
  //Serial.begin(9600);
}


void loop() {
  // Refresh the time at the top of every hour, or every five minutes because
  // the clock drift on the bare Feather M0 is pretty wicked.
  // Display the time
  //checkBattery();
  if (cState != runit) {
    doSetTime();
  }
  else {
    boolean BR_t = digitalRead(BRIGHTNESSPIN);
    boolean SW_t = digitalRead(SW);
    if (!SW_t) {
      if (debounce()) {
        // Serial.println("setHour");
       cState = setHour;
      }
    }
    if (!BR_t) {
      if (debounce()) {
        doSetBrightness();
      }
    }



    setDisplayValue();
    delay(1000);

    // Now increase the seconds by one.
    seconds += 1;
    if (seconds > 59) {
      checkBattery(); // check battery every minute
      seconds = 0;
      minutes += 1;
      if (!(minutes % 15)) readChronoDot(); // sync with chronoDot every 15 mins.
      if (minutes > 59) {
        minutes = 0;
        hours += 1;
        if (hours > 23) {
          hours = 0;
        }
        //readChronoDot(); // sync with chronoDot once per hour
      }
    }
  }
}

void checkBattery() {
  //float cellVoltage = batteryMonitor.getVCell();
  //Serial.print("Voltage:\t\t");
  //Serial.print(cellVoltage, 4);
  //Serial.println("V");
  float stateOfCharge = batteryMonitor.getSoC();
  //Serial.println(stateOfCharge);
  if (stateOfCharge >= 75.0) {
    analogWrite(GREEN, 150);
    analogWrite(RED, 0);
    analogWrite(BLUE, 0);
  }
  else if ((stateOfCharge < 75.0) && (stateOfCharge >= 30.0)) {
    analogWrite(GREEN, 0);
    analogWrite(RED, 0);
    analogWrite(BLUE, 150);
  }
  else if (stateOfCharge < 30.0) {
    analogWrite(RED, 150);
    analogWrite(BLUE, 0);
    analogWrite(GREEN, 0);
  }
  //Serial.print("State of charge:\t");
  //Serial.print(stateOfCharge);
  //Serial.println("%");
}

void doSetTime() {

  unsigned long entryTime = millis();
  ctlTurned               = false;
  digitalWrite(SWPOWER, HIGH);

  while (cState != runit) {
    if ( (entryTime + TIMESETEXIT) < millis() ) cState = runit;
    long newPos = myEnc.read();
    boolean SW_t = digitalRead(SW);

    if (newPos != position) {
      if (newPos > position) {
        if (eState == up && debounce() ) {
          //Serial.println("up");
          if (cState == setHour) {
            hours++;
            if (hours > 23) hours = 0;
          }
          else {
            minutes++;
            if (minutes > 59) minutes = 0;
          }
        }
        eState = up;
        ctlTurned = true;
      }
      else if ( newPos < position) {
        if (eState == down && debounce()) {
          //Serial.println("down");
          if (cState == setHour) {
            hours--;
            if (hours < 0) hours = 23;
          }
          else {
            minutes--;
            if (minutes < 0) minutes = 59;
          }
        }
        eState = down;
        ctlTurned = true;
      }

    }
    position = newPos;

    if (!SW_t && debounce()) {
      if (cState == setHour) {
        cState = setMinute;
        // Serial.println("setMinute");
      } else {
        cState = runit;
        //  Serial.println("runit");
      }

    }
    setDisplayValue();

  }
  if (ctlTurned) {
    seconds = 0;
    setChronoDot(); // ONLY set ChronoDot if we have actually tried to set the time.  Otherwise presume the ChronoDot has the right time.
    ctlTurned = false;
  }
  readChronoDot();
  digitalWrite(SWPOWER, LOW);
  checkBattery();
}
boolean debounce() {
  boolean time =  millis() > (theTime + DEBOUNCETIME);
  theTime = millis();
  return time;
}

void setDisplayValue() {

  displayValue = hours * 100 + minutes;
  clockDisplay.print(displayValue, DEC);
  if (hours < 10) {
    // Pad hour 0.
    clockDisplay.writeDigitNum(0, 0); // 0 = leftmost digit 4=rightmost digit despite what Adafruit docs say
    if (hours == 0) clockDisplay.writeDigitNum(1, 0);
    // Also pad when the 10's minute is 0 and should be padded.
    if (minutes < 10) {
      clockDisplay.writeDigitNum(3, 0);
    }

  }
  blinkColon = !blinkColon;
  clockDisplay.drawColon(blinkColon);
  clockDisplay.writeDisplay();
}

void setChronoDot() {
  dt = clock.getDateTime();
  clock.setDateTime(dt.year, dt.month, dt.day, hours, minutes, seconds);

}

void readChronoDot() {

  dt = clock.getDateTime();
  seconds = dt.second;
  hours   = dt.hour;
  minutes = dt.minute;
  setDisplayValue();

}

void doSetBrightness() {
  brightness++;
  if (brightness > 15) {
    brightness = 0;
  }
  clockDisplay.print(brightness, DEC);
  clockDisplay.setBrightness(brightness);
  clockDisplay.writeDisplay();
  delay(1000);
}

void clearEOSCBit() {
  // clear /EOSC bit
  // Sometimes necessary to ensure that the clock
  // keeps running on just battery power. Once set,
  // it shouldn't need to be reset but it's a good
  // idea to make sure.
  Wire.beginTransmission(0x68); // address DS3231
  Wire.write(0x0E); // select register
  Wire.write(0b00011100); // write register bitmap, bit 7 is /EOSC
  Wire.endTransmission();
}

