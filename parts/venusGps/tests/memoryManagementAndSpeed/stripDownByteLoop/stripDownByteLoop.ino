#include <SoftwareSerial.h>
SoftwareSerial venus(3,4);


bool tagDetected = false;

int charPos;
const int sentenceSize = 20;

byte sentenceFlag = 0xA0,
        readChar;

byte sentence[sentenceSize];


void setup() {
    Serial.begin(9600);
    venus.begin(9600);
    Serial.println(F("\nFinished startup"));
}



void loop() {
    Serial.println("Starting loop");
    tagDetected = false;

    do{
        charPos = 0;
        memset(sentence, 0x0, sentenceSize);
        while( readChar != sentenceFlag ) readChar = venus.read();
        sentence[charPos] = readChar;
        charPos++;

        do{
            while(venus.available() == 0);
            readChar = venus.read();
            sentence[charPos] = readChar;
            charPos++;
        }
        while( (readChar != 0x0A) && (charPos < sentenceSize) );




        tagDetected = true;
    }
    while(tagDetected == false);

    for(int i = 0; i < charPos; i++){
      Serial.print(sentence[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
}
