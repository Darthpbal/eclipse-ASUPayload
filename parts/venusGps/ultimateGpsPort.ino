/*
This is meant to be an experiment. Theorhetically, the ultimate GPS and the venus
GPS appear to work exactly the same aside from the venus GPS not having a patch antenna.
Though, once a position is fixed, the devices appear to send pretty much the exact same data.
Bunches of assorted data with GPGGA and GPRMC as options with a $ start character.

So I wanna see if my program works for both sensors once the software serial port is made.
*/

#include <SoftwareSerial.h>
SoftwareSerial venusGps(10,11); //rx, tx


void setup() {
    Serial.begin(115200);
    venusGps.begin(9600)
    Serial.println("Finished startup");
}


void loop() {
    char tag[] = "$GPGGA,";
    char gpsRawString[100];
    bool validAns = detectGPSTag(tag, 5000);
    if(validAns){
        readGPS(gpsRawString);
        Serial.println(gpsRawString);
    }
    else {
        Serial.println("invalid answer");
    }
    delay(250);//error extra bits of lines show up without this.
}








void readGPS(char* gpsString){
    int gpsChar = 0;    //character position 0
    do{
        while(venusGps.available() == 0);    //wait for incoming gps data
        gpsString[gpsChar] = venusGps.read();    //store char into gps string
        gpsChar++;    //move to next position
    }
    while(gpsString[gpsChar - 1] != '\r');    //this board has a "cairraige return" and "line feed" and the end of each transmission. this "\r" is the cairraige return. This loops until that is seen and then the NULL character "\0" is set at the end to be a valid c-string that can be used with serial print.
    gpsString[gpsChar - 1] = '\0';
    gpsString[gpsChar] = '\0';
}








bool detectGPSTag(char* expectedAns, unsigned int timeout){
    int charPosition = 0;    //position in the response string.
    bool validAns = false;    //default value
    unsigned long timeAtTransmit;    //used to store the current time in milliseconds when the arduino started waiting for a response from the mobile board
    char responseString[100];    //char arracy to store the mobile board response
    memset(responseString, '\0', 100);    //sets the last character to null making this a c-string
    delay(100);    //let serial port stablize before transmitting
    while(venusGps.available() > 0) venusGps.read();    //clear incoming serial port buffer, so the only thing in the buffer will be the shield response
    timeAtTransmit = millis();    //millis returns how many milliseconds have passed since the program started. Basically current time.
    do{
        if(venusGps.available() != 0){//only do something if there's serial data to read
            responseString[charPosition] = venusGps.read();    //serial read stored one byte (eight bits is one char), and store it into the current char position in the response string
            charPosition++;    //move to next char position
            if(strstr(responseString, expectedAns) != NULL){//this function returns null if expectedAns can't be found when searching through responseString
                validAns = true;    //if the expectedAns was found inside responseString, then the answer is valid.
            }
        }
    }//while answer is valid and while (current time - time when command was sent) are less than the timeout
    while((validAns == false) && ((millis() - timeAtTransmit) < timeout));
    return validAns;    //output whether or not the answer was valid.
}









/*


*/
