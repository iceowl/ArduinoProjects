#include <TinyGPS++.h>
#include <SoftwareSerial.h>

/*
   This sample demonstrates TinyGPS++'s capacity for extracting custom
   fields from any NMEA sentence.  TinyGPS++ has built-in facilities for
   extracting latitude, longitude, altitude, etc., from the $GPGLL and 
   $GPRMC sentences.  But with the TinyGPSCustom type, you can extract
   other NMEA fields, even from non-standard NMEA sentences.

   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(RX) and 3(TX).
*/
static const int RXPin = 0, TXPin = 1;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);

HardwareSerial ss = Serial1;

/*
   By declaring TinyGPSCustom objects like this, we announce that we
   are interested in the 15th, 16th, and 17th fields in the $GPGSA 
   sentence, respectively the PDOP (F("positional dilution of precision")),
   HDOP (F("horizontal...")), and VDOP (F("vertical...")).

   (Counting starts with the field immediately following the sentence name, 
   i.e. $GPGSA.  For more information on NMEA sentences, consult your
   GPS module's documentation and/or http://aprs.gids.nl/nmea/.)

   If your GPS module doesn't support the $GPGSA sentence, then you 
   won't get any output from this program.
*/

TinyGPSCustom pdop(gps, "GPGSA", 15); // $GPGSA sentence, 15th element
TinyGPSCustom hdop(gps, "GPGSA", 16); // $GPGSA sentence, 16th element
TinyGPSCustom vdop(gps, "GPGSA", 17); // $GPGSA sentence, 17th element

TinyGPSCustom fix(gps, "GPGGA", 7);

void setup() 
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println(F("UsingCustomFields.ino"));
  Serial.println(F("Demonstrating how to extract any NMEA field using TinyGPSCustom"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
}

void loop() 
{
  // Every time anything is updated, print everything.
  if (gps.altitude.isUpdated() || gps.satellites.isUpdated() ||
    pdop.isUpdated() || hdop.isUpdated() || vdop.isUpdated())
  {
    Serial.print(F("ALT="));   Serial.print(gps.altitude.meters()); 
    Serial.print(F(" PDOP=")); Serial.print(pdop.value()); 
    Serial.print(F(" HDOP=")); Serial.print(hdop.value()); 
    Serial.print(F(" VDOP=")); Serial.print(vdop.value());
    Serial.print(F(" FIX="));  Serial.print(fix.value());
    Serial.print(F(" SATS=")); Serial.print(gps.satellites.value());
    Serial.print(F(" LAT="));  Serial.print(gps.location.lat(),7);
    Serial.print(F(" LNG="));  Serial.println(gps.location.lng(),7);
  }

  while (ss.available() > 0)
    gps.encode(ss.read());
}

