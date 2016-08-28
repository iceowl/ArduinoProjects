char incomingByte = 0;   // for incoming serial data

void setup() {
        delay(60000);
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        Serial.println("Arduino serial started");

}

void loop() {

        // send data only when you receive data:
        if (Serial.available() > 0) {
                // read the incoming byte:
          //      incomingByte = Serial.read();

                // say what you got:
                Serial.print(incomingByte);
                Serial.flush();
        }
}

