#include <SoftwareSerial.h>

SoftwareSerial venus(2,3);



bool tagDetected = false;

int charPos;
const int sentenceSize = 50;

byte prevChar,
        readChar;

byte sentence[sentenceSize];


const static int msgSize = 9;
byte cnfgMsg[msgSize] =
     {0xA0, 0xA1, 0x0, 0x2, 0x2, 0x1, 0x3, 0xD, 0xA};  // size 9 get software version
//    {0xA0, 0xA1, 0x0, 0x9, 0x8, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x9, 0xD, 0xA};    // size 16 set nmea output to gga only


void setup() {
    Serial.begin(9600);
    venus.begin(9600);
    delay(10);
    Serial.println(F("\nFinished startup"));
}



void loop() {

    Serial.println(F("Sending message"));
    venus.write(cnfgMsg, msgSize);




    Serial.println(F("\nreading response\n"));


    memset(sentence, 0x0, sentenceSize);

    tagDetected = false;

    do{
      while(venus.available() == 0);
      readChar = venus.read();
      if(prevChar == 0xA0 && readChar == 0xA1) break;
      else prevChar = readChar;
    }
    while(tagDetected == false);

    sentence[0] = prevChar;
    sentence[1] = readChar;


    tagDetected = false;
    charPos = 2;

    do{
        do{
            while(venus.available() == 0);
            readChar = venus.read();
            sentence[charPos] = readChar;
            charPos++;
            if(prevChar == 0x0D && readChar == 0x0A) break;
            else {
              prevChar = readChar;
            }
        }
        while( (charPos < sentenceSize) );
        tagDetected = true;
    }
    while(tagDetected == false);


    Serial.println(charPos);


    Serial.print("response = ");
    for(int i = 0; i < charPos; i++){
      Serial.print(sentence[i], HEX);
      Serial.print(' ');
    }
    Serial.println();

    if(charPos > 20){
      Serial.print("Kernel version: ");
      Serial.print(sentence[7], DEC);
      Serial.print(".");
      Serial.print(sentence[8], DEC);
      Serial.print(".");
      Serial.println(sentence[9], DEC);

      Serial.print("ODM version: ");
      Serial.print(sentence[11], DEC);
      Serial.print(".");
      Serial.print(sentence[12], DEC);
      Serial.print(".");
      Serial.println(sentence[13], DEC);

      Serial.print("revision: 20");
      Serial.print(sentence[15], DEC);
      Serial.print(".");
      Serial.print(sentence[16], DEC);
      Serial.print(".");
      Serial.println(sentence[17], DEC);
    }


    delay(10000);
}
