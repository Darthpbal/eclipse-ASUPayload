/*
ASU Eclipse Ballooning Project Launch Code for August 2017 Total Solar Eclipse



Last known compilation stats : (date) *last additions
Sketch uses 0 bytes (0%) of program storage space. Maximum is X bytes.
Global variables use 0 bytes (0%) of dynamic memory, leaving X bytes for local variables. Maximum is X bytes.




**visually friendly GPS header difinition:
(GPS data is logged by cropping the GPGGA tag off of the GPS sentence and logging)
*credit - http://www.gpsinformation.org/dale/nmea.htm#GGA
GGA - essential fix data which provide 3D location and accuracy data.
$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
Where:
     GGA          Global Positioning System Fix Data
     123519       Fix taken at 12:35:19 UTC
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     1            Fix quality: 0 = invalid
                               1 = GPS fix (SPS)
                               2 = DGPS fix
                               3 = PPS fix
            			       4 = Real Time Kinematic
            			       5 = Float RTK
                               6 = estimated (dead reckoning) (2.3 feature)
            			       7 = Manual input mode
            			       8 = Simulation mode
     08           Number of satellites being tracked
     0.9          Horizontal dilution of position
     545.4,M      Altitude, Meters, above mean sea level
     46.9,M       Height of geoid (mean sea level) above WGS84
                      ellipsoid
     (empty field) time in seconds since last DGPS update
     (empty field) DGPS station ID number
     *47          the checksum data, always begins with *


*/


////////////////////////////////////////////////////////////////////////////////
//               Start of including headers
////////////////////////////////////////////////////////////////////////////////


#include <Wire.h>               //I2C class
#include <SPI.h>                //SPI class
#include <SD.h>                 //sd card file access class


////////////////////////////////////////////////////////////////////////////////
//                End of including headers
////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////
//                Declare globals
////////////////////////////////////////////////////////////////////////////////

char delim;  //the seperator that will be printed to seperate all values.


//determines how printing is handled for launch debug and graphing contexts
enum configuration { launch, debug, plot };
const configuration mode = launch;



//uSD globals
File logFile;
char fileName[] = "logFile.txt"; // SD library only supports up to 8.3 names
const uint8_t chipSelect = 8;
const uint8_t cardDetect = 9;
bool alreadyBegan = false;  // SD.begin() misbehaves if not first call
//end uSD globals



////////////////////////////////////////////////////////////////////////////////
//             End of global variables
////////////////////////////////////////////////////////////////////////////////








////////////////////////////////////////////////////////////////////////////////
//                          Start of setup
////////////////////////////////////////////////////////////////////////////////
void setup() {
    Serial.begin(9600);
    while (!Serial);  // Wait for serial port to connect (ATmega32U4 type PCBAs)


    Serial.print(F("mode = "));
    if(mode == launch)  Serial.println(F("launch"));
    else if(mode == debug)  Serial.println(F("debug"));
    else if(mode == plot)   Serial.println(F("plot"));

    //set delimiter type
    if(mode == plot) delim = '\t';
    else delim = ',';

    //uSd Card
    pinMode(cardDetect, INPUT);
    if(mode == launch) initializeCard();
    //end uSd Card



    /*
    header definitions:
    millis:        = a millis timestamp showing how many milliseconds the current program has been running
    */
    String header = "millis\n";
    if(mode  == debug) Serial.print(header);
    else if(mode == launch) lineLogger(header);
}//end of setup

////////////////////////////////////////////////////////////////////////////////
//                          End of setup
////////////////////////////////////////////////////////////////////////////////














////////////////////////////////////////////////////////////////////////////////
//                          Start of loop
////////////////////////////////////////////////////////////////////////////////
void loop() {
    //SD card detection/reinitialization
    if(mode == launch){
        if (!digitalRead(cardDetect)){
            initializeCard();
        }
    }
    //end SD card detection/reinitialization


    String logLine = "";        //create a sentence string for logging










    //put sensors here.











    if(mode != plot){
        logLine += millis();    // timestamp
    }


    logLine += '\n';    // last character should be a newline


    if(mode == debug) Serial.print(logLine);
    else if(mode == plot) Serial.print(logLine);
    else if(mode == launch) lineLogger(logLine);

    delay(150);     //for sanity
}// end of loop

////////////////////////////////////////////////////////////////////////////////
//                     End of loop
////////////////////////////////////////////////////////////////////////////////










////////////////////////////////////////////////////////////////////////////////
//                     Start of function definitions
////////////////////////////////////////////////////////////////////////////////






//Do everything from detecting card through opening the demo file
void initializeCard(void){
    Serial.print(F("Initializing SD card..."));

    // Is there even a card?
    if (!digitalRead(cardDetect)){
        Serial.println(F("No card detected. Waiting for card."));
        while (!digitalRead(cardDetect));
        delay(250); // 'Debounce insertion'
    }

    // Card seems to exist.  begin() returns failure
    // even if it worked if it's not the first call.
    if (!SD.begin(chipSelect) && !alreadyBegan){  // begin uses half-speed...
        Serial.println(F("Initialization failed!"));
        initializeCard(); // Possible infinite retry loop is as valid as anything
    }
    else{
        alreadyBegan = true;
    }
    Serial.println(F("Initialization done."));

    Serial.print(fileName);
    if (SD.exists(fileName)){
        Serial.println(F(" exists."));
    }
    else{
        Serial.println(F(" doesn't exist. Creating."));
    }

    Serial.print(F("Opening file: "));
    Serial.println(fileName);
}






// Write the buffer to the log file. If we are possibly in the EOF state, verify
// that to make sure the command isn't written to the file.
void lineLogger(String line){
    logFile = SD.open(fileName, FILE_WRITE);
    if (logFile){
        logFile.write(line.c_str(), line.length());
        logFile.flush();
        logFile.close();
    }
}





////////////////////////////////////////////////////////////////////////////////
//                     End of function definitions
////////////////////////////////////////////////////////////////////////////////
