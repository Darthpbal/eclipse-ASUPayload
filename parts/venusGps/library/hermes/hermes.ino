#include "Hermes.h"
#include <SoftwareSerial.h>

SoftwareSerial venusSerial(10,11);
Hermes venus(&venusSerial);


void setup() {
    Serial.begin(9600);
    Serial.println(F("Startup finished."));
}


void loop() {

}
