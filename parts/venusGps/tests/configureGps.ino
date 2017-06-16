//purpose of this program is to be able to detect the
//end of message flag and terminate the line with a newline

#include <SoftwareSerial.h>
SoftwareSerial venusGps(10,11); //rx tx

char prevChar = '\0';

void setup() {
    Serial.begin(9600);
    venusGps.begin(9600);
}



void loop() {
    if(venusGps.available()){
        byte currentChar = venus.read();
        Serial.print(currentChar, HEX);
        if(currentChar == 0x0A && prevChar == 0x0D) Serial.print('\n');
        else Serial.print(' ');
        prevChar = currentChar;
    }
}
