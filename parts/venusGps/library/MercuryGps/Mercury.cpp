#include "Mercury.h"
#include <SoftwareSerial.h>







Mercury::Mercury (SoftwareSerial *serial){
    serialPort = serial;

    //library settings
    sentenceFlag = '$';
    setRunMode(raw);
    setGpsTag("GPGGA");
    setTimeout(5000);
    setSaveMode(false);
    clearLine();
}



void Mercury::setTimeout(unsigned int newTimeout){
    timeout = newTimeout;
}



unsigned int Mercury::getTimeout(){
    return timeout;
}



void Mercury::begin(int baud){
    serialPort->begin(baud);
}



void Mercury::readRawLine(){
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



int Mercury::geLineSize(){
    return charPos;
}



void Mercury::readFilteredLine(){
    tagDetected = false;
    do{
        readRawLine();

        if((strstr(sentence, gpsTag) != NULL)) tagDetected = true;
        else tagDetected = false;
    }
    while(tagDetected == false);
}



bool Mercury::readLine(){
    if(mode == raw) readRawLine();
    else if(mode == filtered) readFilteredLine();
    else Serial.print(F("mode error"));
}



void Mercury::clearLine(){
    memset(sentence, '\0', sentenceSize);
}



void Mercury::getLine(char* buffer){
    for (int i = 0; i < sentenceSize; i++) {
        buffer[i] = sentence[i];
    }
}



void Mercury::setGpsTag(char* tag){
    for (int i = 0; i < gpsTagSize; i++) {
        gpsTag[i] = tag[i];
    }
}



void Mercury::getGpsTag(char* buffer){
    for (int i = 0; i < gpsTagSize; i++) {
        buffer[i] = gpsTag[i];
    }
}



void Mercury::setRunMode(runMode newMode){
    mode = newMode;
}



void Mercury::getField(char* buffer, int index){
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



bool Mercury::getSoftwareVersion(char* buffer){
}



bool Mercury::setDefault(){
}



bool Mercury::setGPSSerialBaud(int speed){
}



bool Mercury::setNmeaContent(int gga, int gsa, int gsv, int gll, int rmc, int vtg, int zda){
}



bool Mercury::setMsgType(int type){
}



bool Mercury::setPowerMode(int mode){
}



bool Mercury::setPositionUpdateRate(int rate){
}



bool Mercury::setWaas(bool enable){
}



bool Mercury::getWaas(char* buffer){
}



bool Mercury::setSaveMode(bool permanent = false){
    saveMode = permanent;
}



bool Mercury::setPositionPinning(bool enable){
}



bool Mercury::getPositionPinning(char* buffer){
}



bool Mercury::configPositionPinning(unsigned int pinSpeed, unsigned int pinCnt, unsigned int unPinSpeed, unsigned int unPinCnt, unsigned int unPinDistance){
}



bool Mercury::setNavMode(bool mode){
}



bool Mercury::getNavMode(char* buffer){
}



bool Mercury::setPps(int mode){
}



bool Mercury::getPps(int* mode){
}



void Mercury::mapMsgToPayloadLength(int msgId){
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



unsigned int Mercury::calcChecksum(){
    unsigned int checksum = 0;
    for (int i = 0; i < payloadLength; i++) {
        checksum ^= payload[i];
    }
    return checksum;
}



bool Mercury::readResponse(){
}



void Mercury::getResponse(char *buffer){
    for (int i = 0; i < responseSize; i++) {
        buffer[i] = response[i];
    }
}
