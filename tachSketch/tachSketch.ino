/*

Example 24.1 - LCD demonstration

http://tronixstuff.com/tutorials > Chapter 24  CC by-sa-nc liquidCrystal library originally added 18 Apr 2008 by David A. Mellis

library modified 5 Jul 2009   by Limor Fried (http://www.ladyada.net, http://www.adafruit.com)

*/

#include <LiquidCrystal.h> // we need this library for the LCD commands // initialize the library with the numbers of the interface pins

LiquidCrystal lcd(4,5,6,7,8,9); // define our LCD and which pins to user
//int a = 63;
//int u = 0;
//int d = 1; // used for display delayfloat
int ledPin = 13;  // IR LED connected to digital pin 13
int signalPin = 12;
volatile byte rpmcount;
unsigned int rpm;
unsigned long timeold;

//int val;
//long last=0;
//int stat=LOW;
//int stat2;
//int contar=0;

//int sens=700;  // this value indicates the limit reading between dark and light,
              // it has to be tested as it may change acording on the 
              // distance the leds are placed.
//int nPalas=1; // the number of blades of the propeller

//int milisegundos=500; // the time it takes each reading

//float b = 3.1415926;

void rpm_fun() {
 
   //Each rotation, this interrupt function is run twice, so take that into consideration for 
   //calculating RPM
   //Update count
      rpmcount++;
      //digitalWrite(signalPin,HIGH);
     
 }

void setup()
 {
   lcd.begin(16, 2);  // intialise the LCD

   //Interrupt 0 is digital pin 2, so that is where the IR detector is connected
   //Triggers on FALLING (change from HIGH to LOW)
   attachInterrupt(0, rpm_fun, FALLING);

   //Turn on IR LED
   pinMode(ledPin, OUTPUT);
   pinMode(signalPin,OUTPUT);
   digitalWrite(signalPin,HIGH);
   digitalWrite(ledPin, HIGH);

   rpmcount = 0;
   rpm = 0;
   timeold = 0;
 }

//{
/*  
  void loop() {
    
    
  val=analogRead(1);
  delay(100);
  if(val<sens)
    stat=LOW;
   else
    stat=HIGH;
   digitalWrite(signalPin,stat); //as iR light is invisible for us, the led on pin 13 
                          //indicate the state of the circuit.
  
   if(stat2!=stat){  //counts when the state change, thats from (dark to light) or 
                     //from (light to dark), remmember that IR light is invisible for us.
     contar++;
     stat2=stat;
   }
   if(millis()-last>=milisegundos){
     double rps=((double)contar/nPalas)/2.0*1000.0/milisegundos;
     double rpm=((double)contar/nPalas)/2.0*60000.0/(milisegundos);
     //Serial.print((contar/2.0));Serial.print("  RPS ");Serial.print(rps);
    // Serial.print(" RPM");Serial.print(rpm);Serial.print("  VAL ");Serial.println(val);
    lcd.clear();
    lcd.print(" RPM = ");
    lcd.print(rpm);
  lcd.setCursor(0,1);
    lcd.print(" val = ");
    lcd.print(val);
     contar=0;
     last=millis();
   }
}

*/

 void loop()
 {
   //Update RPM every second
   delay(1000);
   //Don't process interrupts during calculations
   detachInterrupt(0);
   //Note that this would be 60*1000/(millis() - timeold)*rpmcount if the interrupt
   //happened once per revolution instead of twice. Other multiples could be used
   //for multi-bladed propellers or fans
   rpm = 30*1000/(millis() - timeold)*rpmcount;
   timeold = millis();
   rpmcount = 0;

   //Print out result to lcd
   lcd.clear();
   lcd.print("     ");
   lcd.print(rpm);

   //Restart the interrupt processing
   //digitalWrite(signalPin,LOW);
   attachInterrupt(0, rpm_fun, FALLING);
  }




//void loop()

//{

//lcd.clear();

//lcd.setCursor(0,0); // positions starting point on LCD, column 0, row 0 (that is, the top left of our LCD)

//lcd.print("Hello this is Joe"); // prints "Hello" at the LCD's cursor position defined above

//lcd.setCursor(0,1); // positions starting point on LCD, column 0, row 1 (that is, the bottom left of our LCD)

//lcd.println("Soon to be Tachometer"); // note the rest of the line is padded out with spaces

//delay(d);

//lcd.clear();

//lcd.setCursor(0,0);

//lcd.print("u = ");

//lcd.print(u++); // this will immediately follow "a = "

//lcd.setCursor(0,1);

//lcd.print("pi = ");

//lcd.print(b,7);   // the 7 means 7 decimal places. You can also replace this with DEC, HEX, BIN as with other *.print functions, as such:

//delay(d);

//lcd.clear();
//
//lcd.home(); // sets cursor back to position 0,0 - same as lcd.setCursor(0,0);

//lcd.print("a (HEX) = ");
//
//lcd.print(a, HEX); // this will immediately follow "a = "
//
//lcd.setCursor(0,1);
/*
lcd.print("a (BIN) = ");

lcd.print(a,BIN);

delay(d);

lcd.noDisplay(); // turns off the display, leaving currently-displayed data as is

delay(d);        // however this does not control the backlight

lcd.display();   // resumes display
*/
//delay(d);

//}
