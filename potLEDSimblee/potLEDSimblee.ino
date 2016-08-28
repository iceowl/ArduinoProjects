int potPin = 6; // Analog input from potentiometer
int ledPin = 2; // PWM output driving LED
void setup() {
 Serial.begin(9600); // Initialize serial communications
}
void loop() {
 int potValue;
 potValue = analogRead(potPin); // Read analog input
 potValue = potValue / 4; // Scale value for PWM output
 Serial.print("PWM Value = "); // Display PWM value on Serial Monitor
 Serial.println(potValue);
 analogWrite(ledPin,potValue); // Use the PWM value to control the LED

 delay(100);
}
