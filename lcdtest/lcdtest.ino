
#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2);

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  lcd.clear();
  lcd.print("This is working");

}

void loop() {
  delay(1000);
  lcd.print("This is looping");
  delay(1000);
  lcd.clear();
  // put your main code here, to run repeatedly:

}
