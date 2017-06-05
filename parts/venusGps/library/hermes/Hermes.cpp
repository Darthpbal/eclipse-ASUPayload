#include "Hermes.h"
#include <SoftwareSerial.h>







Hermes::Hermes (SoftwareSerial *serial){
    serialPort = serial;

    //library settings
    setRunMode(raw);
    setGpsTag("$GPGGA");
    setTimeout(5000);
    setSaveMode(false);
}



void Hermes::setTimeout(unsigned int newTimeout){
    timeout = newTimeout;
}

unsigned int Hermes::getTimeout(){
    return timeout;
}



void Hermes::begin(int baud){
    serialPort->begin(baud);
}



bool Hermes::readSentence(){
    //filtered
    bool validAns = detectFlag("$");
    if(validAns){
        sentence[0] = '$';
        int currentChar = 1;    //character position 0
        do{
            while(serialPort->available() == 0);    //wait for incoming gps data
            sentence[currentChar] = serialPort->read();    //store char into gps string
            currentChar++;    //move to next position
        }
        while(sentence[currentChar - 1] != '\r');    //this board has a "carraige return" and "line feed" and the end of each transmission. this "\r" is the cairraige return. This loops until that is seen and then the NULL character "\0" is set at the end to be a valid c-string that can be used with serial print.
        sentence[currentChar - 1] = '\0';
        sentence[currentChar] = '\0';

        Serial.println(sentence);
    }
    else {
        Serial.println("invalid answer");
    }

    ////////nother version for reading a sentence//////////////////////
}



void Hermes::getSentence(char* buffer){
    for (int i = 0; i < sentenceSize; i++) {
        buffer[i] = sentence[i];
    }
}



void Hermes::mapMsgToPayloadLength(int msgId){
    switch (msgId) {
        case 0x2:
        break;
        case 0x4:
        break;
        case 0x5:
        break;
        case 0x8:
        break;
        case 0x9:
        break;
        case 0xC:
        break;
        case 0xE:
        break;
        case 0x10:
        break;
        case 0x37:
        break;
        case 0x38:
        break;
        case 0x39:
        break;
        case 0x3A:
        break;
        case 0x3B:
        break;
        case 0x3C:
        break;
        case 0x3D:
        break;
        case 0x3E:
        break;
        case 0x3F:
        break;
        case 0x80:
        break;
        case 0x81:
        break;
        case 0x82:
        break;
        case 0x83:
        break;
        case 0x84:
        break;
        case 0x86:
        break;
        case 0xB3:
        break;
        case 0xB4:
        break;
        case 0xB5:
        break;
        case 0xB6:
        break;
        default:
            payloadLength = 0;
        break;
    }
}



unsigned int Hermes::calcChecksum(){
    for (int i = 0; i < payloadLength; i++) {
        /* code */
    }
}



bool Hermes::readResponse(){

}



void Hermes::getResponse(char *buffer){
    for (int i = 0; i < responseSize; i++) {
        buffer[i] = response[i];
    }
}



void Hermes::setRunMode(runMode newMode){
    mode = newMode;
}



void Hermes::getField(char* buffer, int index){
    int sentencePos = 0;
    int fieldPos = 0;
    int commaCount = 0;
    while (sentencePos < sentenceSize){
        if (sentence[sentencePos] == ','){
            commaCount ++;
            sentencePos ++;
        }
        if (commaCount == index){
            buffer[fieldPos] = sentence[sentencePos];
            fieldPos ++;
        }
        sentencePos ++;
    }
    buffer[fieldPos] = '\0';
}



bool Hermes::getSoftwareVersion(char* buffer){
}



bool Hermes::setDefault(){
}



bool Hermes::setSerialBaud(int speed){
}



bool Hermes::setNmeaContent(int gga, int gsa, int gsv, int gll, int rmc, int vtg, int zda){
}



bool Hermes::setMsgType(int type){
}



bool Hermes::setPowerMode(int mode){
}



bool Hermes::setPositionUpdateRate(int rate){
}



bool Hermes::setWaas(bool enable){
}



bool Hermes::getWaas(char* buffer){
}



bool Hermes::setSaveMode(bool permanent = false){
    saveMode = permanent;
}



bool Hermes::setPositionPinning(bool enable){
}



bool Hermes::getPositionPinning(char* buffer){
}



bool Hermes::configPositionPinning(unsigned int pinSpeed, unsigned int pinCnt, unsigned int unPinSpeed, unsigned int unPinCnt, unsigned int unPinDistance){
}



bool Hermes::setNavMode(bool mode){
}



bool Hermes::getNavMode(char* buffer){
}



bool Hermes::setPps(int mode){
}



bool Hermes::getPps(int* mode){
}




void Hermes::setGpsTag(char* tag){
    for (int i = 0; i < gpsTagSize; i++) {
        gpsTag[i] = tag[i];
    }
}




void Hermes::getGpsTag(char* buffer){
    for (int i = 0; i < gpsTagSize; i++) {
        buffer[i] = gpsTag[i];
    }
}





bool Hermes::detectFlag(char *flag){
    int charPosition = 0;    //position in the response string.
    bool validAns = false;    //default value
    memset(sentence, '\0', 100);    //sets the character array to null making this a c-string
    // delay(100);    //let serial port stablize before transmitting
    while(serialPort->available() > 0) serialPort->read();    //clear incoming serial port buffer, so the only thing in the buffer will be the shield response
    unsigned long timeAtTransmit = millis();    //millis returns how many milliseconds have passed since the program started. Basically current time.
    do{
        if(serialPort->available() != 0){//only do something if there's serial data to read
            sentence[charPosition] = serialPort->read();    //serial read stored one byte (eight bits is one char), and store it into the current char position in the response string
            charPosition++;    //move to next char position
            if(strstr(sentence, flag) != NULL){//this function returns null if expectedAns can't be found when searching through responseString
                validAns = true;    //if the expectedAns was found inside responseString, then the answer is valid.
            }
        }
    }//while answer is valid and while (current time - time when command was sent) are less than the timeout
    while((validAns == false) && ((millis() - timeAtTransmit) < timeout));
    return validAns;    //output whether or not the answer was valid.
}
