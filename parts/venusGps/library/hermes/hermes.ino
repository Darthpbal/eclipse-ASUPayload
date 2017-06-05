/*
This file currently compiles and reads sentences properly only if the tag is a dollar 
sign. Using the tag the way I intended causes restarts and freezing. 

I think I need to redesign the reading and getting functions or something so that this all makes more sense.
*/





#include "Hermes.h"
#include <SoftwareSerial.h>

SoftwareSerial venusSerial(10,11);
Hermes venus(&venusSerial);


void setup() {
    Serial.begin(9600);
    venus.begin(9600);
    Serial.println(F("Startup finished."));
}


void loop() {
    venus.readSentence();
}






