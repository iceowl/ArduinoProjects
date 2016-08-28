#include "WProgram.h"
//Clock.pde - Peggy 2.0 Digital Clock
//  Version 1.0 - 06/13/2008
//  Copyright (c) 2008 Arthur J. Dahm III.  All right reserved.
//  Email: art@mindlessdiversions.com
//  Web: mindlessdiversions.com/peggy2
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
 //   but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

  //  You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
//	USAGE:
//	 - Press the "Any" button to cycle through time display, set hours, minutes, month and day.
//	 - Press the "Off/Select" button to set the  hours, minutes, month or day.

//  TODO:
//  - 12/24 Hour Display
//  - Alarm
//  - Timer
//  - Set time from computer over serial port


#include <Display.h>

void setup();
void loop();
void Clock();
void SetTime();
void DisplayTime();
void DisplaySeconds();
void DisplaySecondsBar(uint8_t digit, uint8_t xpos, uint8_t ypos, uint8_t bar);
void DisplayTimeDigit(uint8_t num, uint8_t pos);
void DisplayDateDigit(uint8_t num, uint8_t pos);
void DisplayDigit(uint8_t dig, uint8_t xpos, uint8_t ypos);
void PollButtons();
uint32_t SafeMillis();
Peggy2Display Display;

float rate = 60.0;

#define TIME_X_OFFSET 0
#define TIME_Y_OFFSET 5

#define BUTTON_DEBOUNCE 20UL	// Time in mS to wait until next poll
#define BUTTON_ANY 1			// b1 "any" button
#define BUTTON_LEFT 2			// b2 "left" button
#define BUTTON_DOWN 4			// b3 "down" button
#define BUTTON_UP 8				// b4 "up" button
#define BUTTON_RIGHT 16		// b5 "right" button
#define BUTTON_OFF_SEL 32	// s2 "off/select" button
#define BUTTONS_CURRENT (PINC & B00011111) | ((PINB & 1)<<5)

enum mode_t {MODE_RUN, MODE_SET_HRS, MODE_SET_MINS, MODE_SET_MONTHS, MODE_SET_DAYS};
enum format_t {FORMAT_HRS_12, FORMAT_HRS_24};

struct time_t {
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
	uint8_t mo;
	uint8_t dd;
};

// Button variables
uint32_t buttonDebounce;
uint8_t buttonPollState;
uint8_t buttonPollStatePrev;
uint8_t buttonsPressed;
uint8_t buttonsReleased;

// Timer variables
int32_t currentTime;	//the current time, so the reading stays consistant through a whole update cycle
int32_t lastMillis;				//used to prevent a breakdown when SafeMillis() goes back to 0
int32_t lastTimeUpdate;		//used to see when a second is up
const int32_t oneSec=1000L;	//the length of 1 second in ms

// Clock variables
mode_t mode = MODE_RUN;
format_t format = FORMAT_HRS_12;
time_t time = {12, 0, 0, 1, 1};
int8_t daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Display variables
uint8_t digits[15][5] =	{
												{B11100000, B10100000, B10100000, B10100000, B11100000},	// 0
												{B01000000, B11000000, B01000000, B01000000, B11100000},	// 1
												{B11100000, B00100000, B11100000, B10000000, B11100000},	// 2
												{B11100000, B00100000, B01100000, B00100000, B11100000},	// 3
												{B10100000, B10100000, B11100000, B00100000, B00100000},	// 4
												{B11100000, B10000000, B11100000, B00100000, B11100000},	// 5
												{B11100000, B10000000, B11100000, B10100000, B11100000},	// 6
												{B11100000, B00100000, B00100000, B01000000, B01000000},	// 7
												{B11100000, B10100000, B11100000, B10100000, B11100000},	// 8
												{B11100000, B10100000, B11100000, B00100000, B11100000},	// 9
												{B00000000, B01000000, B00000000, B01000000, B00000000},	// :
												{B00000000, B00100000, B01000000, B10000000, B00000000},	// /
												{B00000000, B00000000, B00000000, B00000000, B00000000},	// ' '
												{B01100000, B01100000, B00000000, B00000000, B00000000},	// A
												{B00000000, B00000000, B00000000, B01100000, B01100000},	// P
												};

#define CHAR_COLON 10
#define CHAR_SLASH 11
#define CHAR_SPACE 12
#define CHAR_AM 13
#define CHAR_PM 14

void setup()
{
	// Set up input buttons
	PORTB = B00000001;	// Pull up on ("OFF/SELECT" button)
	PORTC = B00011111;	// Pull-ups on C
	DDRB = B11111110;		// B0 is an input ("OFF/SELECT" button)
	DDRC = B11100000;		// All inputs

	buttonPollStatePrev = BUTTONS_CURRENT;
	buttonDebounce = SafeMillis();
	
	// Set up display
	Display = Peggy2Display();
	Display.SetRefreshRate(rate);
	
	currentTime = lastMillis = lastTimeUpdate = SafeMillis();
}

void loop()
{
	int32_t oldticks = 0;
	
	PollButtons();
	
	if (mode == MODE_RUN)
		Clock();
	else
		SetTime();

  // Write time to frame buffer
  DisplayTime();
}

void Clock()
{
	//for when millis resets, this method results in a loss of less than 1 min over a year
  lastMillis = currentTime;
  currentTime = SafeMillis();

	//reset lastTimeUpdate when the timer resets
  if (currentTime < lastTimeUpdate)
    lastTimeUpdate -= lastMillis;

	//update the time
  if (currentTime >= (lastTimeUpdate + oneSec))
  {
		//update the counter
    lastTimeUpdate += oneSec;

		//get the new time
		time.ss++;
		if (time.ss > 59)
		{
			time.ss = 0;
			time.mm++;
			if (time.mm > 59)
			{
				time.mm = 0;
				time.hh++;
				if (time.hh > 24)
				{
					time.hh = 1;
				}
				else if (time.hh == 24)
				{
					time.dd++;
					if (time.dd > daysInMonth[time.mo-1])
					{
						time.dd = 1;
						time.mo++;
						if (time.mo > 12)
						{
							time.mo = 1;
						}
					}
				}
			}
		}
  }

	if (buttonsReleased & BUTTON_ANY)
	{
		mode = MODE_SET_HRS;
		time.ss = 0;
	}
}

void SetTime()
{
	if (buttonsReleased & BUTTON_ANY)
	{
		if (mode == MODE_SET_HRS)
			mode = MODE_SET_MINS;
		else if (mode == MODE_SET_MINS)
			mode = MODE_SET_MONTHS;
		else if (mode == MODE_SET_MONTHS)
			mode = MODE_SET_DAYS;
		else
		{
			mode = MODE_RUN;
			lastTimeUpdate = SafeMillis();
		}
	}

	if (buttonsReleased & BUTTON_OFF_SEL)
	{
		if (mode == MODE_SET_HRS)
		{
			time.hh++;
			if (time.hh > 24)
				time.hh = 1;
		}
		else if (mode == MODE_SET_MINS)
		{
			time.mm++;
			if (time.mm > 59)
				time.mm = 0;
		}
		else if (mode == MODE_SET_MONTHS)
		{
			time.mo++;
			if (time.mo > 12)
				time.mo = 1;
		}
		else if (mode == MODE_SET_DAYS)
		{
			time.dd++;
			if (time.dd > ((time.mo == 2)?(29):(daysInMonth[time.mo])))
				time.dd = 1;
		}
	}
}


// Display functions

void DisplayTime()
{
	uint8_t temp;
	
	// am/pm indicator
	if ((time.hh < 12) || (time.hh == 24))
  	DisplayTimeDigit(CHAR_AM, 5);
	else
  	DisplayTimeDigit(CHAR_PM, 5);	

	if (mode != MODE_RUN)
	  currentTime=SafeMillis();
	  
	// hours
	temp = time.hh;
	if (temp > 12)
		temp -= 12;	
	
	if ((mode == MODE_SET_HRS) && (currentTime%1000 >= 500))
	{
		DisplayTimeDigit(CHAR_SPACE, 0);
		DisplayTimeDigit(CHAR_SPACE, 1);
	}
	else
	{
		if (temp > 9)
	    DisplayTimeDigit(1, 0);
		else
	    DisplayTimeDigit(CHAR_SPACE, 0);
		
	  DisplayTimeDigit(temp%10, 1);
	}
	
	// minutes
	if ((mode == MODE_SET_MINS) && (currentTime%1000 >= 500))
	{
		DisplayTimeDigit(CHAR_SPACE, 3);
		DisplayTimeDigit(CHAR_SPACE, 4);
	}
	else
	{
	  DisplayTimeDigit(time.mm/10, 3);
  	DisplayTimeDigit(time.mm%10, 4);
  }
	
	// blinking colon
  if((mode == MODE_RUN) && (currentTime%1000 >= 500))
		DisplayTimeDigit(CHAR_SPACE, 2);
  else
		DisplayTimeDigit(CHAR_COLON, 2);
		
	// seconds
	DisplaySeconds();
		
	// month
	if ((mode == MODE_SET_MONTHS) && (currentTime%1000 >= 500))
	{
		DisplayDateDigit(CHAR_SPACE, 0);
		DisplayDateDigit(CHAR_SPACE, 1);
	}
	else
	{
		if (time.mo > 9)
	    DisplayDateDigit(1, 0);
		else
	    DisplayDateDigit(CHAR_SPACE, 0);
		
	  DisplayDateDigit(time.mo%10, 1);
	}
	
	// day
	if ((mode == MODE_SET_DAYS) && (currentTime%1000 >= 500))
	{
		DisplayDateDigit(CHAR_SPACE, 3);
		DisplayDateDigit(CHAR_SPACE, 4);
	}
	else
	{
		if (time.dd > 9)
	    DisplayDateDigit(time.dd/10, 3);
		else
	    DisplayDateDigit(CHAR_SPACE, 3);
		
	  DisplayDateDigit(time.dd%10, 4);
	}
	
	// slash
	DisplayDateDigit(CHAR_SLASH, 2);
}

void DisplaySeconds()
{
	uint8_t temp;

	temp = time.ss%10;

	// Display 1s
	for (uint8_t x=0; x<10; x++)
	{
		if (temp == x)
			DisplaySecondsBar(1, x<<1, TIME_Y_OFFSET+8, 0xc0);
		else		
			DisplaySecondsBar(0, x<<1, TIME_Y_OFFSET+8, 0xc0);
	}
		
	// Display 10s
	temp = time.ss/10;
	
	for (uint8_t x=1; x<5; x++)
	{
		if (temp == x)
			DisplaySecondsBar(1, (x-1)<<2, TIME_Y_OFFSET+6, 0x70);
		else		
			DisplaySecondsBar(0, (x-1)<<2, TIME_Y_OFFSET+6, 0x70);
	}
}

void DisplaySecondsBar(uint8_t digit, uint8_t xpos, uint8_t ypos, uint8_t bar)
{
	uint16_t widebar;
	
	widebar = uint16_t(bar)<<(8-(xpos%8));
//	widebar >>= xpos%8;
	
	if (digit)
	{
		Display.framebuffer[(ypos<<2)+(xpos>>3)] |= (widebar>>8);
		Display.framebuffer[(ypos<<2)+(xpos>>3)+1] |= (widebar);
	}
	else
	{
		Display.framebuffer[(ypos<<2)+(xpos>>3)] &= ~(widebar>>8);
		Display.framebuffer[(ypos<<2)+(xpos>>3)+1] &= ~(widebar);
	}
}

void DisplayTimeDigit(uint8_t num, uint8_t pos)
{
	DisplayDigit(num, pos + TIME_X_OFFSET, TIME_Y_OFFSET);
}

void DisplayDateDigit(uint8_t num, uint8_t pos)
{
	DisplayDigit(num, pos + TIME_X_OFFSET, TIME_Y_OFFSET+10);
}

void DisplayDigit(uint8_t dig, uint8_t xpos, uint8_t ypos)
{
	for (int j=0; j<5; j++)
	{
		uint8_t temp;
		
		temp = Display.framebuffer[((j+ypos)<<2)+(xpos>>1)] & ((xpos&1)?(0xf0):(0x0f));
		Display.framebuffer[((j+ypos)<<2)+(xpos>>1)] = temp | ((xpos&1)?(digits[dig][j]>>5):(digits[dig][j]>>1));
	}
}

// Process button input

void PollButtons()
{
	uint32_t debouncetime;
	debouncetime = SafeMillis();
	
	if (debouncetime > (buttonDebounce + BUTTON_DEBOUNCE))
	{
		buttonDebounce = debouncetime;
		
		buttonPollState = BUTTONS_CURRENT;
		buttonPollStatePrev ^= buttonPollState;		// buttonPollStatePrev is nonzero if there has been a change.
	
		buttonsReleased = buttonPollStatePrev & buttonPollState;
		buttonsPressed = buttonPollStatePrev & ~buttonPollState;
	
		buttonPollStatePrev = buttonPollState;
	}
	else
	{
		buttonsReleased = 0;
		buttonsPressed = 0;
	}
}

uint32_t SafeMillis()
{
	uint32_t result;
	
	cli();
	result = millis();
	sei();

	return result;
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

