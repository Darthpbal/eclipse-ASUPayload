#include "Hermes.h"
#include <SoftwareSerial.h>







Hermes::Hermes (SoftwareSerial *serial){
    serialPort = serial;

    //library settings
    sentenceFlag = '$';
    setRunMode(raw);
    setGpsTag("GPGGA");
    setTimeout(5000);
    setSaveMode(false);
    clearLine();
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



void Hermes::readRawLine(){
    charPos = 0;
    memset(sentence, '\0', 100);
    while( readChar != '$' ) readChar = serialPort->read();
    sentence[charPos] = readChar;
    charPos++;

    do{
        while(serialPort->available() == 0);
        readChar = serialPort->read();
        sentence[charPos] = readChar;
        charPos++;
    }
    while( (readChar != '\r') && (charPos < sentenceSize) );

    sentence[charPos - 1] = '\0';
}



void Hermes::readFilteredLine(){
    tagDetected = false;
    do{
        readRawLine();

        if((strstr(sentence, gpsTag) != NULL)) tagDetected = true;
        else tagDetected = false;
    }
    while(tagDetected == false);
}



bool Hermes::readLine(){
    if(mode == raw) readRawLine();
    else if(mode == filtered) readFilteredLine();
    else Serial.print(F("mode error"));
}



void Hermes::clearLine(){
    memset(sentence, '\0', sentenceSize);
}



void Hermes::getLine(char* buffer){
    for (int i = 0; i < sentenceSize; i++) {
        buffer[i] = sentence[i];
    }
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



bool Hermes::setGPSSerialBaud(int speed){
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
    unsigned int checksum = 0;
    for (int i = 0; i < payloadLength; i++) {
        checksum ^= payload[i];
    }
    return checksum;
}



bool Hermes::readResponse(){
}



void Hermes::getResponse(char *buffer){
    for (int i = 0; i < responseSize; i++) {
        buffer[i] = response[i];
    }
}
