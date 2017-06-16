#include <SoftwareSerial.h>

SoftwareSerial venus(2,3);



bool tagDetected = false;

int charPos;
const int sentenceSize = 20;

byte sentenceFlag = 0xA0,
        readChar;

byte sentence[sentenceSize];


byte cnfgMsg[16] =
    // {0xA0, 0xA1, 0x0, 0x2, 0x2, 0x1, 0x3, 0xD, 0xA};  //get software version
    {0xA0, 0xA1, 0x0, 0x9, 0x8, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x9, 0xD, 0xA};    //set nmea output to gga only


void setup() {
    Serial.begin(9600);
    venus.begin(9600);
    delay(10000);
    Serial.println("configuring GPS...");
    Serial.println(F("Sending message"));
    venus.write(cnfgMsg, 16);

    Serial.println(F("\nreading response\n"));
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


    Serial.println(F("\nFinished startup"));
}



void loop() {





    delay(5000);
}
