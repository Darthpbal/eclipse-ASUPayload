#ifndef Hermes_h
#define Hermes_h

#include "Arduino.h"


//typedef enum runMode = {r/aw, filtered};
typedef enum { raw, filtered } runMode;

class Hermes {
private:
    char sentence[100];
    char gpsTag[10];
    byte payload[20];
    byte response[20];
    unsigned int payloadLength;
    bool saveMode = false;
    runMode mode = raw;
    void mapMsgToPayloadLength();
    unsigned int calcChecksum();
    bool readResponse();
    Stream *serialPort;
    byte msgStartFlag[2] = {0xA0, 0xA1},
        msgEndFlag[2] = {0x0D, 0x0A};



public:
    Hermes (Stream *serialPort);

    void getResponse(char *buffer);
    void setRunMode(runMode mode);
    void getSentence();
    void getField(char* buffer, int index);
    bool getSoftwareVersion();
    bool setDefault();
    bool setSerialBaud(int speed);
    bool setNmeaContent(int gga, int gsa, int gsv, int gll, int rmc, int vtg, int zda);
    bool setMsgType(int type);
    bool setPowerMode(int mode);
    bool setPositionUpdateRate(int rate);
    bool setWaas(bool enable);
    bool getWaas();
    bool setSaveMode(bool permanent = 0);
    bool setPositionPinning(bool enable);
    bool getPositionPinning();
    bool configPositionPinning(unsigned int pinSpeed, unsigned int pinCnt, unsigned int unPinSpeed, unsigned int unPinCnt, unsigned int unPinDistance);
    bool setNavMode(bool mode);
    bool getNavMode();
    bool setPps();
    bool getPps();
};
#endif
