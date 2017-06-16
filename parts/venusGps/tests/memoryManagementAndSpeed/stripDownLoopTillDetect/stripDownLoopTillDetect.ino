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
    Serial.println(F("starting"));


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


        Serial.println(sentence);
        if(strstr(sentence, tag) != NULL) tagDetected = true;
        else tagDetected = false;
    }
    while(tagDetected == false);



    Serial.println(charPos);


    Serial.print(F("char pos "));    Serial.print(charPos - 6);    Serial.print(F(" "));     Serial.println(sentence[charPos - 6]);
    Serial.print(F("char pos "));    Serial.print(charPos - 5);    Serial.print(F(" "));     Serial.println(sentence[charPos - 5]);
    Serial.print(F("char pos "));    Serial.print(charPos - 4);    Serial.print(F(" "));     Serial.println(sentence[charPos - 4]);
    Serial.print(F("char pos "));    Serial.print(charPos - 3);    Serial.print(F(" "));     Serial.println(sentence[charPos - 3]);
    Serial.print(F("char pos "));    Serial.print(charPos - 2);    Serial.print(F(" "));     Serial.println(sentence[charPos - 2]);
    Serial.print(F("char pos Hex "));    Serial.print(charPos - 1);    Serial.print(F(" "));     Serial.println(sentence[charPos - 1], HEX);

    Serial.println(F("end of startup\n"));

    Serial.print(tag);
    Serial.print(F(" "));
    if(tagDetected) Serial.println(F("matched!!!"));
    else Serial.println(F("mis match..."));
    Serial.println(F("\n\n"));
}



void loop() {
    delay(5000);
}




/*
results: arduino serial com capture


starting
$GPGSV,3,1,12,32,80,291,41,14,62,237,34,10,60,035,41,18,43,072,34*76
$GPGSV,3,2,12,27,29,238,37,21,28,145,33,08,27,278,38,31,18,175,30*71
$GPGSV,3,3,12,24,17,047,31,11,13,316,28,22,01,285,,01,00,319,*73
$GPRMC,074338.149,A,3326.5276,N,11224.3108,W,000.0,000.0,070617,,,A*70
$GPGSA,A,3,32,18,10,11,14,27,31,21,08,24,,,1.5,0.8,1.2*3B
$GPGSV,3,1,12,32,80,291,41,14,62,237,34,10,60,035,41,18,43,072,34*76
$GPGSV,3,2,12,27,29,238,37,21,28,145,33,08,27,278,38,31,18,175,30*71
$GPGSV,3,3,12,24,17,047,30,11,13,316,29,22,01,285,,01,00,319,*73
$GPRMC,074339.149,A,3326.5276,N,11224.3108,W,000.0,000.0,070617,,,A*71
$GPGSA,A,3,32,18,10,11,14,27,31,21,08,24,,,1.5,0.8,1.2*3B
$GPGSV,3,1,12,32,80,291,41,14,62,237,34,10,60,035,41,18,43,072,34*76
$GPGSV,3,2,12,27,29,238,37,21,28,145,33,08,27,278,38,31,18,175,30*71
$GPGSV,3,3,12,24,17,047,30,11,13,316,28,22,01,285,,01,00,319,*72
$GPRMC,074340.149,A,3326.5276,N,11224.3108,W,000.0,000.0,070617,,,A*7F
$GPGSA,A,3,32,18,10,11,14,27,31,21,08,24,,,1.5,0.8,1.2*3B
$GPGSV,3,1,12,32,80,291,41,14,62,237,33,10,60,035,41,18,43,072,34*71
$GPGSV,3,2,12,27,29,238,37,21,28,145,33,08,27,278,38,31,18,175,30*71
$GPGSV,3,3,12,24,17,047,30,11,13,316,29,22,01,285,,01,00,319,*73
$GPRMC,074341.149,A,3326.5276,N,11224.3108,W,000.0,000.0,070617,,,A*7E
$GPGSA,A,3,32,18,10,11,14,27,31,21,08,24,,,1.5,0.8,1.2*3B
$GPGSV,3,1,12,32,80,291,42,14,62,237,34,10,60,035,41,18,43,072,34*75
$GPGSV,3,2,12,27,29,238,37,21,28,145,33,08,27,278,38,31,18,175,31*70
$GPGSV,3,3,12,24,17,047,30,11,13,316,30,22,01,285,,01,00,319,*7B
$GPRMC,074342.149,A,3326.5276,N,11224.3108,W,000.0,000.0,070617,,,A*7D
$GPGGA,074343.149,3326.5276,N,11224.3108,W,1,10,0.8,310.7,M,-28.3,M,,0000*61
77
char pos 71 0
char pos 72 0
char pos 73 *
char pos 74 6
char pos 75 1
char pos Hex 76 0
end of startup

GPGGA matched!!!


*/
