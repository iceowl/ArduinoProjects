
#define geoPin A7
#define ledPin 13

int geoIn = 0;


void setup() {

  pinMode(geoPin,INPUT);
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);
 // Serial.begin(57600);
}

void loop() {
  geoIn = analogRead(geoPin);
  if(geoIn > 0)digitalWrite(ledPin,HIGH);
  else digitalWrite(ledPin,LOW);
  //Serial.println();
  //Serial.print(geoIn);
}
