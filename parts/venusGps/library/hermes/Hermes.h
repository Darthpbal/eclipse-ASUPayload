#ifndef Hermes_h
#define Hermes_h

#include "Arduino.h"
#include <SoftwareSerial.h>


//typedef enum runMode = {r/aw, filtered};
typedef enum { raw, filtered } runMode;

class Hermes {
private:
    const static int sentenceSize = 100,
                        gpsTagSize = 10,
                        payloadSize = 20,
                        responseSize = 20;

    //Here's some default settings a user may want to change.
    runMode mode;     //determines if all data will be printed, or only
                            //lines matching the desired gps tag
    char gpsTag[gpsTagSize];     //desired gps tag
    unsigned int timeout;    //controls how long the board waits for a gps tag before a timing out
    bool saveMode;      //controls whether to save to venus board's flash
                                //or not. This will make whatever settings change
                                //you do, stay that way after removing power.
                                //My preference was to never actually permanently
                                //overwrite any settings to avoid having settings
                                //that functionally conflict. For example, the
                                //command set, says that the 10Hz update rate must
                                //be at 38400 baud or higher. And in my experience, if you
                                //switch to that update rate without changing the
                                //baud rate first, communication with the board
                                //breaks down and the data is no longer readable
                                //at any baud rate. Keeping all changes temporary
                                //and simply configuring the gps in your setup
                                //avoids that mess. :D
    //End of default settings


    char sentence[sentenceSize];
    byte payload[payloadSize];
    byte response[responseSize];
    unsigned int payloadLength;
    void mapMsgToPayloadLength(int msgId);               //for setting the payload length to the appropriate value for all messages I'll implement
    unsigned int calcChecksum();                //for calculating the checksum
    bool readResponse();                        //reads the ACk response of the GPS
    bool detectFlag(char *flag);                //for listening on a serial port and returning true when a sequence is seen
    SoftwareSerial *serialPort;
    byte msgStartFlag[2] = {0xA0, 0xA1},
        msgEndFlag[2] = {0x0D, 0x0A};



public:
    Hermes (SoftwareSerial *serial);            //ctor, sets the member software serial pointer to the ctor argument

    bool readSentence();
    void clearSentence();                       //read a normal gps sentence
    void begin(int baud);                       //set software serial port baud rate
    void setGpsTag(char* tag);                  //seeter for the gps tag
    void getGpsTag(char* buffer);               //getter for the gps tag
    void setTimeout(unsigned int newTimeout);                          //seeter and getter for timout
    unsigned int getTimeout();                          //
    void getResponse(char *buffer);             //return
    void setRunMode(runMode newMode);              //set whether or not to filter incoming data
    void getSentence(char* buffer);             //fills the buffer with the last sentence seen
    void getField(char* buffer, int index);     //fills the buffer with whatever desired index
    bool getSoftwareVersion(char* buffer);      //fills the buffer with the software version
    bool setDefault();                          //sets the device the command to reset to factory defaults
    bool setSerialBaud(int speed);              //sends the gps the commands to configure it's baud rate
    bool setNmeaContent(int gga, int gsa, int gsv, int gll, int rmc, int vtg, int zda);     //configures what type of NMEA sentences are reported
    bool setMsgType(int type);                  //turns ff the nmea output or sets it to normal and binary mode.
    bool setPowerMode(int mode);                //sets the power mode on the gps
    bool setPositionUpdateRate(int rate);       //sets the position update rate and sets the baud rate to appropriate baud for safety
    bool setWaas(bool enable);                  //I don't really know what WAAS is, but it's easy to configure and sounded like it could be a useful setting
    bool getWaas(char* buffer);                 //fills the buffer with the WAAS status
    bool setSaveMode(bool permanent = false);   //sets whether or not configuration of the gps is permanent. I recommend against unless you know you won't break the board.
    bool getSaveMode();                         //returns the saveMode member variable, in case you need it
    bool setPositionPinning(bool enable);       //configures position pinning
    bool getPositionPinning(char* buffer);      //fills the buffer with the pinning state
    bool configPositionPinning(unsigned int pinSpeed, unsigned int pinCnt, unsigned int unPinSpeed, unsigned int unPinCnt, unsigned int unPinDistance);     //changes position pinning settings on the gps board
    bool setNavMode(bool mode);                 //sets the mode to either false for car, or true for pedestrian
    bool getNavMode(char* buffer);              //fills the buffer with the navigation mode
    bool setPps(int mode);                      //sets the pps mode from 0 off, 1 on when 3D fix, and 2 on when 1 SV
    bool getPps(int* mode);                     //sets the mode argument to whatever the GPS mode is
};
#endif
