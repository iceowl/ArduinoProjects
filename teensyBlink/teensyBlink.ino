
int ledpin = 13;
void setup() {
  // put your setup code here, to run once:
 pinMode(ledpin,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
 digitalWrite(ledpin,HIGH);
 delay(100);
 digitalWrite(ledpin,LOW);
 delay(2000);

}
