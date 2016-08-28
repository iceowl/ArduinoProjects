/********************************************************************
* Arduino GPS Distance Calculator
* Distributed under GPL v2.0
* Copyright (c) 2012 Stanley Huang All rights reserved.
********************************************************************/

#include <arduino.h>
#include <TinyGPS.h>
#include <LCD4884.h>
#include <math.h>

//keypad debounce parameter
#define DEBOUNCE_MAX 15
#define DEBOUNCE_ON  10
#define DEBOUNCE_OFF 3

#define NUM_KEYS 5
// joystick number
#define KEY_LEFT 0
#define KEY_CENTER 1
#define KEY_DOWN 2
#define KEY_RIGHT 3
#define KEY_UP 4

int adc_key_val[5] ={
  50, 200, 400, 600, 800 };

// debounce counters
byte button_count[NUM_KEYS];
// button status - pressed/released
byte button_status[NUM_KEYS];
// button on flags for user program
byte button_flag[NUM_KEYS];

// Convert ADC value to key number
char get_key(unsigned int input)
{
  char k;
  for (k = 0; k < NUM_KEYS; k++) {
    if (input < adc_key_val[k]) {
        return k;
    }
  }

  if (k >= NUM_KEYS)
    k = -1;     // No valid key pressed

  return k;
}

void update_adc_key(){
  int adc_key_in = analogRead(0);
  char key_in = get_key(adc_key_in);
  for(byte i=0; i<NUM_KEYS; i++) {
    if(key_in==i) {
      //one key is pressed
      if(button_count[i]<DEBOUNCE_MAX) {
        button_count[i]++;
        if(button_count[i]>DEBOUNCE_ON) {
          if(button_status[i] == 0) {
            button_flag[i] = 1;
            button_status[i] = 1; //button debounced to 'pressed' status
          }
        }
      }
    } else {
      // no button pressed
      if (button_count[i] >0) {
        button_flag[i] = 0;
        button_count[i]--;
        if(button_count[i]<DEBOUNCE_OFF) {
          button_status[i]=0;   //button debounced to 'released' status
        }
      }
    }

  }
}

TinyGPS gps;
bool ready = false;

byte CheckPressedKey()
{
    for(int i=0; i<NUM_KEYS; i++){
      if(button_flag[i] !=0){
        button_flag[i]=0;  // reset button flag
        return i;
      }
    }
    return -1;
}

void setup()
{
    // setup interrupt-driven keypad arrays
    // reset button arrays
    for(byte i=0; i<NUM_KEYS; i++){
        button_count[i]=0;
        button_status[i]=0;
        button_flag[i]=0;
    }

    // Setup timer2 -- Prescaler/256
    TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
    TCCR2B &= ~(1<<WGM22);
    TCCR2B = (1<<CS22)|(1<<CS21);

    ASSR |=(0<<AS2);

    // Use normal mode
    TCCR2A =0;
    //Timer2 Overflow Interrupt Enable
    TIMSK2 |= (0<<OCIE2A);
    TCNT2=0x6;  // counting starts from 6;
    TIMSK2 = (1<<TOIE2);

    SREG|=1<<SREG_I;

    lcd.LCD_init();
    lcd.LCD_clear();
    lcd.LCD_write_string(0, 0, " WAITING GPS  ", MENU_HIGHLIGHT);

    Serial.begin(9600);
    delay(2000);
}

void loop()
{
    static unsigned long lastTime = 0;
    static long originLat = 0;
    static long originLon = 0;
    static long lat = 0;
    static long lon = 0;

    switch (CheckPressedKey()) {
    case KEY_CENTER:
        originLat = lat;
        originLon = lon;
        lcd.LCD_write_string(0, 0, " Location Set ", MENU_HIGHLIGHT);
        delay(1000);
        break;
    case KEY_DOWN:
        lcd.backlight(0);
        break;
    case KEY_UP:
        lcd.backlight(1);
        break;
    }

    if (!Serial.available()) return;

    char c = Serial.read();

    // push NMEA characters to TinyGPS
    if (!gps.encode(c)) {
        // parsed data not ready
        if (!ready) {
            lcd.LCD_putchar(c);
        }
        return;
    }

    // update screen every 500ms
    if (millis() - lastTime > 500) {
        char buf[16];
        unsigned long fix_age;
        unsigned long speed;
        unsigned long date, time;

        if (!ready) {
            // put up the ready sign and clear the screen
            ready = true;
            lcd.LCD_clear();
        }

        gps.get_datetime(&date, &time, &fix_age);
        sprintf(buf, "%06ld %08ld", date, time);
        lcd.LCD_write_string(0, 0, buf, MENU_HIGHLIGHT);

        gps.get_position(&lat, &lon, &fix_age);
        sprintf(buf, "LAT:%d.%05ld",
            (int)(lat / 100000), lat % 100000);
        lcd.LCD_write_string(0, 1, buf, MENU_NORMAL);
        sprintf(buf, "LON:%d.%05ld",
            (int)(lon / 100000), lon % 100000);
        lcd.LCD_write_string(0, 2, buf, MENU_NORMAL);
        sprintf(buf, "ALT:%dm SAT:%d ",
            (int)(gps.altitude() / 100), gps.satellites());
        lcd.LCD_write_string(0, 3, buf, MENU_NORMAL);

        speed = gps.speed();
        if (speed > 1000) speed = 0;
        // 1knot = 1.852km
        sprintf(buf, "SPEED:%d km/h  ", (int)(speed * 1852 / 100000));
        lcd.LCD_write_string(0, 4, buf, MENU_NORMAL);

        // distance
        if (originLat != 0) {
            long distance = sqrt((lat - originLat) * (lat - originLat) + (lon - originLon) * (lon - originLon));
            sprintf(buf, "DST: %ld m    ", distance);
            lcd.LCD_write_string(0, 5, buf, MENU_NORMAL);
        }

        lastTime = millis();
    }
}

// Timer2 interrupt routine -
// 1/(160000000/256/(256-6)) = 4ms interval
ISR(TIMER2_OVF_vect) {
  TCNT2  = 6;
  update_adc_key();
}
