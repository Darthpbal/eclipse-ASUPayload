#include <SoftwareSerial.h>
SoftwareSerial venus(10,11);

int charPos = 0;
const int sentenceSize = 100;

char sentenceFlag = '$',
        readChar;

char tag[] = "GPGGA",
        sentence[sentenceSize];

void setup() {
    Serial.begin(9600);
    venus.begin(9600);

    Serial.println(F("starting"));

    memset(sentence, '\0', 100);

    clearPort();

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
    Serial.println(charPos);


    Serial.print(F("char pos "));    Serial.print(charPos - 6);    Serial.print(F(" "));     Serial.println(sentence[charPos - 6]);
    Serial.print(F("char pos "));    Serial.print(charPos - 5);    Serial.print(F(" "));     Serial.println(sentence[charPos - 5]);
    Serial.print(F("char pos "));    Serial.print(charPos - 4);    Serial.print(F(" "));     Serial.println(sentence[charPos - 4]);
    Serial.print(F("char pos "));    Serial.print(charPos - 3);    Serial.print(F(" "));     Serial.println(sentence[charPos - 3]);
    Serial.print(F("char pos "));    Serial.print(charPos - 2);    Serial.print(F(" "));     Serial.println(sentence[charPos - 2]);
    Serial.print(F("char pos Hex "));    Serial.print(charPos - 1);    Serial.print(F(" "));     Serial.println(sentence[charPos - 1], HEX);

    Serial.println(F("end of startup"));

    Serial.print(tag);
    Serial.print(F(" "));
    if(strstr(sentence, tag) != NULL) Serial.println(F("matched!!!"));
    else Serial.println(F("mis match..."));
}








void loop() {
  delay(5000);
}

void clearPort(){
    while( (venus.available() != 0) ) venus.read();
}
