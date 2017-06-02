#include "Hermes.h"
#include <SoftwareSerial.h>







Hermes::Hermes (Stream *serialPort){
}



void Hermes::mapMsgToPayloadLength(){
}



unsigned int Hermes::calcChecksum(){
}



bool Hermes::readResponse(){
}



void Hermes::getResponse(char *buffer){
}



void Hermes::setRunMode(runMode mode){
}



void Hermes::getSentence(){
}



void Hermes::getField(char* buffer, int index){
}



bool Hermes::getSoftwareVersion(){
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



bool Hermes::getWaas(){
}



bool Hermes::setSaveMode(bool permanent = 0){
}



bool Hermes::setPositionPinning(bool enable){
}



bool Hermes::getPositionPinning(){
}



bool Hermes::configPositionPinning(unsigned int pinSpeed, unsigned int pinCnt, unsigned int unPinSpeed, unsigned int unPinCnt, unsigned int unPinDistance){
}



bool Hermes::setNavMode(bool mode){
}



bool Hermes::getNavMode(){
}



bool Hermes::setPps(){
}



bool Hermes::getPps(){
}
