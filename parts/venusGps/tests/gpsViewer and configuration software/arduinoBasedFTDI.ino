//This file is based on the serial passthrough example built into the IDE

#include <SoftwareSerial.h>
SoftwareSerial venusGps(10,11); //rx, tx

void setup() {
  Serial.begin(9600);
  venusGps.begin(9600);
}

void loop() {
  if (Serial.available()) {      // If anything comes in Serial (USB)
    venusGps.write(Serial.read());   // read it and send it out venusGps
  }

  if (venusGps.available()) {     // If anything comes in venusGps
    Serial.write(venusGps.read());   // read it and send it out Serial (USB)
    // Serial.print(venusGps.read(),HEX);   //comment the previous serial write
    // Serial.print(' ');  //and uncomment this line and the next for reading the 
                           //configuration command and echoing to the screen.
  }
}
