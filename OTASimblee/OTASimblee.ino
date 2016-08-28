#include <ota_bootloader.h>
int led_green = 3;
int buttonA = 5;
void setup() {
 pinMode(led_green, OUTPUT);
 pinMode(buttonA, INPUT);
}
void loop() {
 digitalWrite(led_green, HIGH);
 if(digitalRead(buttonA) == 1){
 ota_bootloader_start(); //begins OTA enabled state
 }
}
