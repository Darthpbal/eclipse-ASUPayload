#include <SoftwareSerial.h>
SoftwareSerial venus(10,11);


bool tagDetected = false;

int charPos;
const int sentenceSize = 100;

char sentenceFlag = '$',
        readChar;

char tag[] = "GPGGA",
        sentence[sentenceSize];


void setup() {
    Serial.begin(9600);
    venus.begin(9600);
    Serial.println(F("\nStarting"));
}



void loop() {
    tagDetected = false;

    do{
        charPos = 0;
        memset(sentence, '\0', 100);
        while( readChar != '$' ) readChar = venus.read();
        sentence[charPos] = readChar;
        charPos++;

        do{
            while(venus.available() == 0);
            readChar = venus.read();
            sentence[charPos] = readChar;
            charPos++;
        }
        while( (readChar != '\r') && (charPos < sentenceSize) );

        sentence[charPos - 1] = '\0';


        if(strstr(sentence, tag) != NULL) tagDetected = true;
        else tagDetected = false;
    }
    while(tagDetected == false);
    Serial.println(sentence);
}
