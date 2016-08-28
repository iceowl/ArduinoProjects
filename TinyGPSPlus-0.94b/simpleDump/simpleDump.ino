    #include <SoftwareSerial.h>

    #define rxPin 0
    #define txPin 1

    HardwareSerial ss = Serial1;

    void setup()  {
      Serial.begin(115200);
      ss.begin(9600);
      ss.print("$PMTK314,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0*28/r/n");
      ss.print("$PMTK220,1000*1F\r\n"); // set update rate to 1000 milliseconds (e.g. once per second)
      ss.print("$PMTK313,1*2E\r\n");  // Enable to search a SBAS satellite
      ss.print("$PMTK513,1*28\r\n"); //another SBAS Enable
      ss.print("$PMTK301,2*2E\r\n");  // Enable WAAS as DGPS Source
      ss.print("$PMTK397,0*23\r\n"); // speed threshold set to 0.2 m/s
    }

    void loop() {
      if (ss.available()) {
        Serial.write(ss.read());
      }
    }


