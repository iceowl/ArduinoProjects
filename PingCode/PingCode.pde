/Parallax sonar detector
//Open source software and license from Matt at http://antipastohw.blogspot.com/
void setup()
{
Serial.begin(9600);
Serial.println("Sonar detector");
delay(100);
}

//Here's the readSonar function - pass it the pin of the signal wire from the Sonar sensor, and it will return a long of the distance. If you wanted to use the Sonar module in your own projects, just grab this function, and drop it into your own code!
long readSonar( int signalpin) {
int readsignal = 0;
long counttime = 0;

pinMode(signalpin, OUTPUT);
digitalWrite(signalpin, LOW);
digitalWrite(signalpin, HIGH);
delayMicroseconds(4);
digitalWrite(signalpin, LOW);

pinMode(signalpin, INPUT);

readsignal = digitalRead(signalpin);
while (readsignal == LOW) {
readsignal = digitalRead(signalpin);
}

counttime=0;
while (readsignal == HIGH) {
readsignal = digitalRead(signalpin);
counttime++;
}

return counttime;
}

//Here's the main loop:
void loop()
{
long distance = 0;

distance = readSonar(8); //here's where I call the function

Serial.print("Value: ");
Serial.println(distance);

delay(100);
}
