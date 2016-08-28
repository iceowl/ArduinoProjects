#include <TinyGPS++.h>
//#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <math.h>

#define debug       1
#define gpsSettings 1

#define LISTSIZE             3

#define HDOPMAX              200 // should be no greater than 100 for accuracy
#define ALARMPIN             8
#define LIGHTPIN             9
#define STABILIZATION_WAIT   LISTSIZE * 3
#define ALARMDELAY           15 // x * 3 seconds e.g. 10 = 30 secs
#define ANCHORTHRESHOLD      20 // meters
#define GPSNOISETHRESHOLD    20 * ANCHORTHRESHOLD
#define GPSMINERRORTHRESHOLD 10 // US Govt. max threshold 
 
static const int RXPin = 0, TXPin = 1;

static const   uint32_t GPSBaud   = 9600;
HardwareSerial ss = Serial1;


// The TinyGPS++ object
TinyGPSPlus gps;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);

//boolean start = true;

static unsigned long curr_ave_lat  = 0;
static unsigned long curr_ave_lng  = 0;
static unsigned long curr_hdop_sum = 0;
static unsigned long curr_ave_hdop = 0;

static unsigned long max_lat_delta = 0;
static unsigned long max_lng_delta = 0;

static unsigned long l_lat_list[LISTSIZE];
static unsigned long l_lng_list[LISTSIZE];
static unsigned long hdop_list [LISTSIZE];
static unsigned long hdop_list2[LISTSIZE];

static bool hdop_ave_trigger = false;

static int    coord_list_index   = 0;
static int    hdop_list_index    = 0;

static unsigned long max_dist    = 0;

static unsigned long l_start_lat = 0;
static unsigned long l_start_lng = 0;
static unsigned long l_lat       = 0;
static unsigned long l_lng       = 0;
static unsigned long dLongFactor = 0;

void setup()
{
  
  Serial.begin(115200);
  ss.begin(GPSBaud);
  
  pinMode(ALARMPIN,OUTPUT);
  pinMode(LIGHTPIN,OUTPUT);
  digitalWrite(ALARMPIN,LOW);
  digitalWrite(LIGHTPIN,LOW);

  lcd.begin(16, 2);
  lcd.clear();
  // Print a message to the LCD.
  lcd.print("Start:");
  Serial.println(F("Start:"));
  for(int i = 0;i < LISTSIZE; i++) {
    l_lat_list[i] = l_lng_list[i] = hdop_list[i] = hdop_list2[i] = 0;
  }

#ifdef gpsSettings
doGPSSettings();
#endif

  smartDelay(1000);
  
  while(!gps.location.isValid()) {
#ifdef debug  
    Serial.println(F("Wait for data to become Valid")); 
#endif
    lcd.clear();
    lcd.print("Searching...");
    analogWrite(LIGHTPIN,0);
    for(int i = 0; i < 5;i++) {
      smartDelay(1000);
      lcd.print(".");
      if(gps.location.isValid()) break;
    }
  }


  //Serial.println(F("Data Valid"));
  analogWrite(LIGHTPIN,0);
  lcd.clear();
  lcd.print("GPS Fixed");
  smartDelay(1000);

  int isw = 0;

  while ((isw < STABILIZATION_WAIT) || (curr_ave_hdop >= HDOPMAX)) {

    updateLists();

    lcd.clear();
    lcd.print("lat : ");
    lcd.print(gps.location.lat());
    lcd.setCursor(0,1);
    lcd.print("lng : ");
    lcd.print(gps.location.lng());
    smartDelay(1000);

    if(isw < STABILIZATION_WAIT) isw++;

  }

  dLongFactor = 111319 * cos(gps.location.lat()); // this is 111319 meters/degree at equator * cos(latitude) = meters/degreeLongitude at latitude
  
  l_start_lat = gps.location.l_lat();
  l_start_lng = gps.location.l_lng();
  
  curr_ave_lat = l_start_lat;
  curr_ave_lng = l_start_lng;
  
#ifdef debug
    Serial.print("Lat = ");
    Serial.print(gps.location.lat());
    Serial.print("   Lng = ");
    Serial.println(gps.location.lng());
    Serial.print(F(" l_start_lat = ")); 
    Serial.print(l_start_lat);
    Serial.print(F(" l_start_lng = ")); 
    Serial.println(l_start_lng);
    Serial.print("  dLongFactor = ");
    Serial.println(dLongFactor);   
#endif
  

}

void loop()
{


  smartDelay(1000);

  if(gps.location.isValid()) {

    analogWrite(LIGHTPIN,0);

    updateLists();

    unsigned long dist = TinyGPSPlus::distanceBetween3(
    curr_ave_lat,
    curr_ave_lng,
    l_start_lat, 
    l_start_lng,
    dLongFactor);

#ifdef debug
Serial.print("  Dist = ");
Serial.println(dist);
#endif

    if(dist > max_dist) max_dist = dist;

    unsigned long latitude_delta = curr_ave_lat - l_start_lat;
    unsigned long long_delta     = curr_ave_lng - l_start_lng;

    if(latitude_delta < 0 ) latitude_delta = -latitude_delta;
    if(long_delta < 0 )     long_delta     = -long_delta;

    if(latitude_delta  > max_lat_delta) max_lat_delta = latitude_delta;
    if(long_delta > max_lng_delta)      max_lng_delta = long_delta;



    if (dist > GPSNOISETHRESHOLD){
      lcd.clear();
      lcd.print("*GPS Noise*");
      lcd.setCursor(5,1);
      lcd.print(dist);

      smartDelay(500);
      return;
    } 
    else  if  (dist < GPSMINERRORTHRESHOLD) {
      // distanceFromStart = 0.0;
      lcd.clear();
      lcd.print(" Below Threshold ");
      lcd.setCursor(5,1);
      lcd.print(dist);
      return;
    }  
    else if(dist <= ANCHORTHRESHOLD) {
      // printInt(distanceFromStart, gps.location.isValid(), 9);
      lcd.clear();
      lcd.print("Distance in Meters: ");
      lcd.setCursor(5,1);
      lcd.print(dist);
      return;
    } 
    else if(dist > ANCHORTHRESHOLD) {
      lcd.clear();
      lcd.setCursor(1,1);
      lcd.print("**Alarm Wait**");
      lcd.setCursor(0,0);
      for(int i = 0; i < ALARMDELAY;i++) {
        smartDelay(3000);
        updateLists();
       
        unsigned long dist = TinyGPSPlus::distanceBetween3(
        curr_ave_lat,
        curr_ave_lng,
        l_start_lat, 
        l_start_lng,
        dLongFactor);
        
        
#ifdef debug
        Serial.print(F(" distance from start = "));
        Serial.print(dist);
        Serial.print(F("  hdop = "));
        Serial.println(gps.hdop.value());
#endif
        if(dist < ANCHORTHRESHOLD) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("*Alarm Cleared*");
          smartDelay(500);
          return;
        }

        lcd.print("!");
      }


      while(1) {
        lcd.clear();
        lcd.setCursor(4,1);
        digitalWrite(ALARMPIN,HIGH);
        lcd.print("**ALARM**");
        smartDelay(5000);
      }
    }
    //else Serial.println(F("*"));
  }


  smartDelay(5000);

  if (millis() > 15000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
  lcd.clear();
  lcd.print("NO GPS");
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } 
  while (millis() - start < ms);
}


static void updateLists() {

  setAveHDOP();
  
  l_lat = gps.location.l_lat();
  l_lng = gps.location.l_lng();

  if(gps.hdop.value() < HDOPMAX) {

    l_lat_list [coord_list_index]  = gps.location.l_lat();
    l_lng_list [coord_list_index]  = gps.location.l_lng();
    hdop_list  [coord_list_index]  = HDOPMAX - gps.hdop.value(); // hdop is inverse goodness

    curr_ave_lat = curr_ave_lng = curr_hdop_sum = 0.0;

    for(int i = 0; i < LISTSIZE ; i++) {

      curr_ave_lat  += (l_lat_list[i] * hdop_list[i]);
      curr_ave_lng  += (l_lng_list[i] * hdop_list[i]);
      curr_hdop_sum += hdop_list[i] ;

    }

    curr_ave_lat /= curr_hdop_sum;
    curr_ave_lng /= curr_hdop_sum;

    coord_list_index++;
    if(coord_list_index >= LISTSIZE) coord_list_index = 0;


  } 


  else {
    lcd.clear();
    lcd.print("Excluding coordinate");
    lcd.setCursor(0,1);
    lcd.print("HDOP= ");
    lcd.print(gps.hdop.value());

#ifdef debug
    Serial.print(F("HDOP too large excluding coordinate "));
    Serial.print(F("HDOP = "));
    Serial.println(gps.hdop.value());
#endif

    smartDelay(1000);

  }



}

static void setAveHDOP() {

  hdop_list2[hdop_list_index] = gps.hdop.value();
  curr_ave_hdop = 0;
  for(int i = 0; i < LISTSIZE ; i++) {
    curr_ave_hdop += hdop_list2[i];
  }

  if(hdop_list_index >= LISTSIZE) {
    hdop_list_index = 0; 
    hdop_ave_trigger = true;
  }

  if(hdop_ave_trigger) curr_ave_hdop /= LISTSIZE;
  else curr_ave_hdop /= hdop_list_index+1;

  hdop_list_index++;

}

#ifdef gpsSettings

static void doGPSSettings() {
  
  ss.print("$PMTK314,1,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n");
  //ss.print("$PMTK314,-1*04"); //set default reporting
  smartDelay(100);
  ss.print("$PMTK220,200*2C\r\n"); // set update rate to 1000 milliseconds (e.g. once per second) must be slower than 5hz for SBAS to work
  smartDelay(100);
  ss.print("$PMTK300,200,0,0,0,0*2F\r\n"); // position fix interval in milliseconds (must be larger than 200);
  smartDelay(100);
  ss.print("$PMTK313,1*2E\r\n");  // Enable to search a SBAS satellite
  smartDelay(100);
  //ss.print("$PMTK513,1*28\r\n"); //another SBAS Enable
  smartDelay(100);
  ss.print("$PMTK301,2*2E\r\n");  // Enable WAAS as DGPS Source
  smartDelay(100);
  ss.print("$PMTK397,0*23\r\n"); // speed threshold disabled
  smartDelay(100);
  //ss.print("$PMTK319,1*24\r\n"); // SBAS satellite "integrity" mode
  smartDelay(100);
  ss.print("$PMTK286,1*23\r\n");//enable interference canceling
  smartDelay(100);
  ss.print("$PMTK527,0*2E\r\n"); //nav threshold disabled
  
  
}

#endif





