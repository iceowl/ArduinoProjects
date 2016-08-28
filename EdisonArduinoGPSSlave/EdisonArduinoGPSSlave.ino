
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

//#define ALARMPIN    2
//#define LIGHTPIN    7
#define RX 1
#define TX 0
#define EDISON_ALIVE 6
#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define TFT_DC     8
#define TFT_SCLK   13
#define TFT_MOSI   11
#define RELAY_ENABLE A1
#define ROTATION   5

#define CLEARSCREEN  '('
#define REDCOLOR     '!'
#define YELLOWCOLOR  '@'
#define GREENCOLOR   '$'
#define WHITECOLOR   '*'
#define BIG          '&'
#define NOTBIG       '^'
#define HANDSHAKE    "_"
#define CHAND        '_'
#define ENUMBER      ')'
#define RESETLOC     ';'
#define SETALARM     ':'
#define RESETALARM   '{'


#define MAXCHARSONSCREEN 180
#define MAXLINESONSCREEN   5



enum drawingState {undefined, big, notBig};
enum tftColor     {tft_white, tft_yellow, tft_green, tft_red, tft_black};

static void setTextColor(tftColor color);

static drawingState theState;
static tftColor     currentColor;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);


static int charsOnScreen = 0;
static int linesOnScreen = 0;
static char currentNumber[10];
static int cx = 0;


void setup() {

  //leave RX/TX in high impedance state so as not to interfere with EDISON boot up
  pinMode(RX, INPUT);
  pinMode(TX, OUTPUT);


  // put your setup code here, to run once:
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_RST, OUTPUT);
  pinMode(EDISON_ALIVE, INPUT);
  pinMode(RELAY_ENABLE,OUTPUT);
  
  digitalWrite(RELAY_ENABLE,LOW);


  // pinMode(LIGHTPIN, OUTPUT);
  // pinMode(ALARMPIN, OUTPUT);

  // digitalWrite(ALARMPIN, LOW);
  // digitalWrite(LIGHTPIN, LOW);

  delay(10);

  theState = notBig;
  setTextColor(tft_white);

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  clearTFT();
  tft.setTextWrap(true);
  tft.setRotation(ROTATION);
  tft.println("Waiting for Edison:");
  while (digitalRead(EDISON_ALIVE)) {
    delay(100);
  }

  tft.println("..begin handshake");
  delay(500);
  Serial.begin(9600);
  delay(500);
  while (Serial.available()) Serial.read();
  tft.println("cleared buffer...");
  while (!digitalRead(EDISON_ALIVE)) {
    delay(100);
    Serial.write(HANDSHAKE);
  }
  tft.println("\nEdison Awake - Arduino linked");

}

void loop() {

  char incomingByte;
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    if (incomingByte == '\n') {
      if ((linesOnScreen++ > MAXLINESONSCREEN) && (theState != big)) {
        clearTFT();
      } else if (((charsOnScreen++ > MAXCHARSONSCREEN) && incomingByte == '\n') && (theState != big)) {
        clearTFT();
      }
    }

    if (checkByte(incomingByte)) {
      if (theState == big) currentNumber[cx++] = incomingByte;
      tft.print(incomingByte);
    }

  }

}

static void clearTFT() {

  tft.fillScreen(ST7735_BLACK);

  if (theState == notBig) {
    setTFT(10, 3);
    charsOnScreen = 0;
    linesOnScreen = 0;

  } else setTFT(25, 3);

}

static void setTFT (int startx, int rot) {



  tft.setTextWrap(false);
  tft.setRotation(rot);

  if (theState == notBig) {
    tft.setTextSize(1);
    tft.setCursor(startx, 30);

  } else {
    tft.setTextSize(6);
    tft.setCursor(startx, 40);
  }

}



static void sendHandshake() {
  Serial.write(HANDSHAKE);
  while (!digitalRead(EDISON_ALIVE)) {
    delay(10);
    Serial.write(HANDSHAKE);
  }
}

static bool checkByte(char charIn) {

  bool ret = false;

  switch (charIn) {

    case CHAND:
      return false;
      break;
    case RESETLOC:
      resetLocation();
      ret = false;
      break;
    case CLEARSCREEN:
      clearTFT();
      cx = 0;
      ret = false;
      break;
    case REDCOLOR:
      tft.setTextColor(ST7735_RED);
      currentColor = tft_red;
      ret = false;
      break;
    case YELLOWCOLOR:
      tft.setTextColor(ST7735_YELLOW);
      currentColor = tft_yellow;
      ret = false;
      break;
    case GREENCOLOR:
      tft.setTextColor(ST7735_GREEN);
      currentColor = tft_green;
      ret = false;
      break;
    case WHITECOLOR:
      tft.setTextColor(ST7735_WHITE);
      currentColor = tft_white;
      ret = false;
      break;
    case BIG:
      theState = big;
      ret = false;
      break;
    case NOTBIG:
      theState = notBig;
      ret = false;
      break;
    case RESETALARM:
      resetAlarm();
      ret = false;
      break;
    case SETALARM:
      setAlarm();
      ret = false;
      break;
    case ENUMBER:
      eraseNumber();
      ret = false;
      break;

    default:
      ret = true;
  }
  // tft.println(charIn);
  if (!ret) {
    setState();
    sendHandshake();
  }

  return ret;
}

static void resetLocation() {
  eraseNumber();
  tft.fillScreen(ST7735_GREEN);
  setTextColor(tft_black);
  tft.println("RST");
  delay(250);
  tft.fillScreen(ST7735_BLACK);
  clearTFT();
  setTextColor(tft_white);
  currentColor = tft_white;
}

static void eraseNumber() {

  setTextColor(tft_black);
  setState();
  for (int i = 0; i <= cx; i++)tft.print(currentNumber[i]);

  setTextColor(currentColor);

  setState();
  cx = 0;

}

static void setTextColor(tftColor color) {

  switch (color) {
    case tft_red:
      tft.setTextColor(ST7735_RED);
      break;
    case tft_yellow:
      tft.setTextColor(ST7735_YELLOW);
      break;
    case tft_green:
      tft.setTextColor(ST7735_GREEN);
      break;
    case tft_white:
      tft.setTextColor(ST7735_WHITE);
      break;
    case tft_black:
      tft.setTextColor(ST7735_BLACK);
      break;

  }

}

static void setAlarm() {
  
  pinMode(RELAY_ENABLE,INPUT);
  
}

static void resetAlarm() {
  
  pinMode(RELAY_ENABLE,OUTPUT);
  digitalWrite(RELAY_ENABLE,LOW);
  
}

static void setState() {


  if (theState == big) {
    tft.setTextWrap(false);
    tft.setRotation(ROTATION);
    tft.setTextSize(6);
    tft.setCursor(25, 40);
  } else {
    setTFT(10, ROTATION);
  }



}
