// end results: I'm missing a bunch of datapoints basically 3 seconds in between.
// Even worse, pretty much every other instance of detecting GGA ends up timing out
//So next step is gonna have to be experimenting with really reliable single sentence read that sorts into the GGA sentence.


/*
this was used as an experiment for trying to figure out why the system keeps
restarting like crazy. It seems to me like it could be due to the fact that I'm
parsing multiple lines (more data) and I'm running into the end of the buffer.

So basically the parsing is gonna be slightly different from the original version.
*/



#include <SoftwareSerial.h>
SoftwareSerial venus(10,11);

bool validAns;    //used for checking if the response from mobile shield is valid (it understood my commands)

void setup() {
    Serial.begin(9600);    //set hardware serial port speed to match the mobile board (arduino TX/RX)
    venus.begin(9600);
    Serial.println(F("Starting up..."));    //indicate to foolish humans
//    powerOn();    //run powerup sequence to startup the mobile shield
    delay(500);    //let the serial port rest a sec
    validAns = sendATcommand("", "$", 1000);    //send command to set mobile shield into GPS mode for reporting GPS
    if(validAns) Serial.println(F("GPS activated"));    //Indicate successful GPS configuration
    else Serial.println(F("Error initializing GPS"));    //report an error for human debugging
    Serial.println(F("Ready..."));    //indicate to humans the system is finished setting up
}



void loop() {
    validAns = sendATcommand("", "$GPGGA", 2000);    //send command requesting GPS location
    if(validAns){//if answer from mobile board is valid
        char gpsRaw[100];    //initialize 100 sized char array to store GPS string
        readGPS(gpsRaw);    //after the valid response was seen, a GPS string output is incoming, this reads that, and stores into the gpsRaw variable
        Serial.print(F("Sentence = "));    //the gps string is ",,,,,,,," when the location isn't fixed. otherwise the first char is anumber in the longitude
        Serial.println(gpsRaw);    //report position
//        Serial.println("");    //add a blank line to make the serial monitor more visually divided and easier to read.
    }
    else Serial.println(F("response is invalid"));    //indicate an invalid response from mobile board
    Serial.println('\n');
    delay(1000);
}


bool sendATcommand(char* command, char* expectedAns, unsigned int timeout){
    int charPosition = 0;    //position in the response string.
    bool validAns = false;    //default value
    unsigned long timeAtTransmit;    //used to store the current time in milliseconds when the arduino started waiting for a response from the mobile board
    char responseString[300];    //char arracy to store the mobile board response
    memset(responseString, '\0', 100);    //sets the last character to null making this a c-string
    delay(100);    //let serial port stablize before transmitting
    while(venus.available() > 0) venus.read();    //clear incoming serial port buffer, so the only thing in the buffer will be the shield response
    Serial.println("waiting");    //send command to the mobile board
    timeAtTransmit = millis();    //millis returns how many milliseconds have passed since the program started. Basically current time.
    do{
        if(venus.available() != 0){//only do something if there's serial data to read
            responseString[charPosition] = venus.read();    //serial read stored one byte (eight bits is one char), and store it into the current char position in the response string
            Serial.print(responseString[charPosition]);
            charPosition++;    //move to next char position
            if(strstr(responseString, expectedAns) != NULL){//this function returns null if expectedAns can't be found when searching through responseString
                validAns = true;    //if the expectedAns was found inside responseString, then the answer is valid.
            }
        }
    }//while answer is valid and while (current time - time when command was sent) are less than the timeout
    while((validAns == false) && ((millis() - timeAtTransmit) < timeout) && (charPosition < 300));
    return validAns;    //output whether or not the answer was valid.
}


void readGPS(char* gpsString){
    int gpsChar = 0;    //character position 0
    do{
        while(venus.available() == 0);    //wait for incoming gps data
        gpsString[gpsChar] = venus.read();    //store char into gps string
        gpsChar++;    //move to next position
    }
    while(gpsString[gpsChar - 1] != '\r');    //this board has a "cairraige return" and "line feed" and the end of each transmission. this "\r" is the cairraige return. This loops until that is seen and then the NULL character "\0" is set at the end to be a valid c-string that can be used with serial print.
    gpsString[gpsChar] = '\0';
}
















/*
results
arduino serial com

Starting up...
GPS activated
Ready...
Sentence = ,040357.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*60
Sentence = ,16,18,304,22,05,06,051,23,25,03,188,21,27,00,321,*76

Sentence = ,040400.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*65
Sentence = 04,22,05,06,051,23,25,03,188,21,27,01,321,*77

Sentence = ,040403.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*66
Sentence = 3,3,12,16,18,304,23,05,06,051,23,25,03,188,22,27,01,321,*75

Sentence = ,040406.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*63
Sentence = 8,304,22,05,06,051,23,25,03,188,21,27,01,321,*77

Sentence = ,040409.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*6C
Sentence = 16,18,304,22,05,06,051,22,25,03,188,21,27,01,321,*76

Sentence = ,040412.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*66
Sentence = 16,18,304,23,05,06,051,21,25,03,188,21,27,01,321,*74

Sentence = ,040415.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*61








results
cutecom serial



[21:03:57:125] $GPGGA,040356.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*61␍␊
[21:03:57:220] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:03:57:284] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,42,18,52,251,37*7A␍␊
[21:03:57:364] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,29*7A␍␊
[21:03:57:443] $GPGSV,3,3,12,16,18,304,22,05,06,051,24,25,03,188,21,27,00,321,*71␍␊
[21:03:57:521] $GPRMC,040356.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*70␍␊
[21:03:57:603] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊



[21:03:58:124] $GPGGA,040357.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*60␍␊
[21:03:58:213] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:03:58:284] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,42,18,52,251,37*7A␍␊
[21:03:58:362] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,29*7A␍␊
[21:03:58:441] $GPGSV,3,3,12,16,18,304,22,05,06,051,23,25,03,188,21,27,00,321,*76␍␊
[21:03:58:519] $GPRMC,040357.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*71␍␊
[21:03:58:601] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:03:59:123] $GPGGA,040358.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*6F␍␊
[21:03:59:215] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:03:59:281] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,43,18,52,251,37*7B␍␊
[21:03:59:361] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,28*7B␍␊
[21:03:59:441] $GPGSV,3,3,12,16,18,304,22,05,06,051,23,25,03,188,21,27,00,321,*76␍␊
[21:03:59:518] $GPRMC,040358.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*7E␍␊
[21:03:59:603] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:04:00:126] $GPGGA,040359.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*6E␍␊
[21:04:00:218] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:00:285] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,42,18,52,251,37*7A␍␊
[21:04:00:363] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,28*7B␍␊
[21:04:00:444] $GPGSV,3,3,12,16,18,304,22,05,06,051,23,25,03,188,21,27,01,321,*77␍␊
[21:04:00:521] $GPRMC,040359.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*7F␍␊
[21:04:00:604] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊



[21:04:01:133] $GPGGA,040400.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*65␍␊
[21:04:01:223] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:01:292] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,42,18,52,251,37*7A␍␊
[21:04:01:371] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,28*7B␍␊
[21:04:01:451] $GPGSV,3,3,12,16,18,304,22,05,06,051,23,25,03,188,21,27,01,321,*77␍␊
[21:04:01:529] $GPRMC,040400.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*74␍␊
[21:04:01:611] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:04:02:125] $GPGGA,040401.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*64␍␊
[21:04:02:215] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:02:282] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,42,18,52,251,37*7A␍␊
[21:04:02:363] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,29*7A␍␊
[21:04:02:443] $GPGSV,3,3,12,16,18,304,22,05,06,051,23,25,03,188,21,27,01,321,*77␍␊
[21:04:02:520] $GPRMC,040401.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*75␍␊
[21:04:02:602] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:04:03:126] $GPGGA,040402.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*67␍␊
[21:04:03:217] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:03:284] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,42,18,52,251,37*7A␍␊
[21:04:03:367] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,29*7A␍␊
[21:04:03:445] $GPGSV,3,3,12,16,18,304,22,05,06,051,23,25,03,188,21,27,01,321,*77␍␊
[21:04:03:522] $GPRMC,040402.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*76␍␊
[21:04:03:605] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊



[21:04:04:125] $GPGGA,040403.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*66␍␊
[21:04:04:215] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:04:282] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,43,18,52,251,37*7B␍␊
[21:04:04:365] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,29*7A␍␊
[21:04:04:442] $GPGSV,3,3,12,16,18,304,23,05,06,051,23,25,03,188,21,27,01,321,*76␍␊
[21:04:04:520] $GPRMC,040403.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*77␍␊
[21:04:04:604] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:04:05:131] $GPGGA,040404.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*61␍␊
[21:04:05:224] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:05:288] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,43,18,52,251,37*7B␍␊
[21:04:05:369] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,29*7A␍␊
[21:04:05:450] $GPGSV,3,3,12,16,18,304,23,05,06,051,23,25,03,188,22,27,01,321,*75␍␊
[21:04:05:527] $GPRMC,040404.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*70␍␊
[21:04:05:609] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:04:06:133] $GPGGA,040405.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*60␍␊
[21:04:06:222] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:06:290] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,43,18,52,251,38*74␍␊
[21:04:06:369] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,29*7A␍␊
[21:04:06:450] $GPGSV,3,3,12,16,18,304,22,05,06,051,23,25,03,188,22,27,01,321,*74␍␊
[21:04:06:528] $GPRMC,040405.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*71␍␊
[21:04:06:610] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊



[21:04:07:132] $GPGGA,040406.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*63␍␊
[21:04:07:225] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:07:289] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,43,18,52,251,38*74␍␊
[21:04:07:369] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,32,10,23,240,29*7B␍␊
[21:04:07:450] $GPGSV,3,3,12,16,18,304,22,05,06,051,23,25,03,188,22,27,01,321,*74␍␊
[21:04:07:527] $GPRMC,040406.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*72␍␊
[21:04:07:610] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:04:08:126] $GPGGA,040407.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*62␍␊
[21:04:08:218] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:08:285] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,43,18,52,251,38*74␍␊
[21:04:08:364] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,30*72␍␊
[21:04:08:447] $GPGSV,3,3,12,16,18,304,22,05,06,051,23,25,03,188,21,27,01,321,*77␍␊
[21:04:08:523] $GPRMC,040407.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*73␍␊
[21:04:08:607] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:04:09:128] $GPGGA,040408.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*6D␍␊
[21:04:09:218] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:09:286] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,43,18,52,251,38*74␍␊
[21:04:09:368] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,32,10,23,240,29*7B␍␊
[21:04:09:448] $GPGSV,3,3,12,16,18,304,22,05,06,051,23,25,03,188,21,27,01,321,*77␍␊
[21:04:09:524] $GPRMC,040408.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*7C␍␊
[21:04:09:607] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊



[21:04:10:126] $GPGGA,040409.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*6C␍␊
[21:04:10:216] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:10:284] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,43,18,52,251,38*74␍␊
[21:04:10:366] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,29*7A␍␊
[21:04:10:444] $GPGSV,3,3,12,16,18,304,22,05,06,051,22,25,03,188,21,27,01,321,*76␍␊
[21:04:10:521] $GPRMC,040409.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*7D␍␊
[21:04:10:604] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:04:11:125] $GPGGA,040410.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*64␍␊
[21:04:11:218] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:11:282] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,43,18,52,251,38*74␍␊
[21:04:11:362] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,33,10,23,240,30*72␍␊
[21:04:11:443] $GPGSV,3,3,12,16,18,304,22,05,06,051,22,25,03,188,21,27,01,321,*76␍␊
[21:04:11:521] $GPRMC,040410.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*75␍␊
[21:04:11:603] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:04:12:132] $GPGGA,040411.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*65␍␊
[21:04:12:221] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:12:288] $GPGSV,3,1,12,29,59,151,44,21,56,326,34,15,53,108,43,18,52,251,38*75␍␊
[21:04:12:369] $GPGSV,3,2,12,20,51,040,27,13,31,063,38,26,23,275,32,10,23,240,30*73␍␊
[21:04:12:449] $GPGSV,3,3,12,16,18,304,22,05,06,051,22,25,03,188,21,27,01,321,*76␍␊
[21:04:12:527] $GPRMC,040411.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*74␍␊
[21:04:12:609] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊



[21:04:13:133] $GPGGA,040412.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*66␍␊
[21:04:13:222] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:13:290] $GPGSV,3,1,12,29,59,151,44,21,56,326,34,15,53,108,43,18,52,251,38*75␍␊
[21:04:13:370] $GPGSV,3,2,12,20,51,040,27,13,31,063,37,26,23,275,33,10,23,240,29*75␍␊
[21:04:13:450] $GPGSV,3,3,12,16,18,304,22,05,06,051,22,25,03,188,21,27,01,321,*76␍␊
[21:04:13:528] $GPRMC,040412.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*77␍␊
[21:04:13:610] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:04:14:131] $GPGGA,040413.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*67␍␊
[21:04:14:220] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:14:288] $GPGSV,3,1,12,29,59,151,44,21,56,326,35,15,53,108,43,18,52,251,37*7B␍␊
[21:04:14:367] $GPGSV,3,2,12,20,51,040,25,13,31,063,38,26,23,275,32,10,23,240,30*71␍␊
[21:04:14:448] $GPGSV,3,3,12,16,18,304,23,05,06,051,21,25,03,188,21,27,01,321,*74␍␊
[21:04:14:526] $GPRMC,040413.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*76␍␊
[21:04:14:608] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊

[21:04:15:133] $GPGGA,040414.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*60␍␊
[21:04:15:223] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:15:290] $GPGSV,3,1,12,29,59,151,44,21,56,326,34,15,53,108,43,18,52,251,38*75␍␊
[21:04:15:371] $GPGSV,3,2,12,20,51,040,25,13,31,063,38,26,23,275,32,10,23,240,29*79␍␊
[21:04:15:450] $GPGSV,3,3,12,16,18,304,23,05,06,051,21,25,03,188,20,27,01,321,*75␍␊
[21:04:15:531] $GPRMC,040414.917,A,3326.5260,N,11224.3103,W,000.0,000.0,070617,,,A*71␍␊
[21:04:15:611] $GPVTG,000.0,T,,M,000.0,N,000.0,K,A*0D␍␊



[21:04:16:133] $GPGGA,040415.917,3326.5260,N,11224.3103,W,1,10,1.0,306.5,M,-28.3,M,,0000*61␍␊
[21:04:16:222] $GPGSA,A,3,15,18,21,26,29,20,13,10,16,05,,,1.7,1.0,1.4*32␍␊
[21:04:16:289] $GPGSV,3,1,12,29,59,151,44,21,56,326,34,15,53,108,43,18,52,251,38*75␍␊
[21:04:16:370] $GPGSV,3,2,12,20,51,040,25,13,31,063,37,26,23,275,32,10,23,240,30*7E␍␊
[21:04:16:450] $GPGSV,3,3,12,16,18,304,24,05,06,051,21,25,03,188,20,27,01,321,*72␍␊

*/
