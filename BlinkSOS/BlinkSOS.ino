

#define LEDPIN 13

void setup () {

 pinMode(LEDPIN,OUTPUT);
  
}

void loop() {

  digitalWrite(LEDPIN,HIGH);
  delay(200);
  digitalWrite(LEDPIN,LOW);
  delay(100);
  
}

