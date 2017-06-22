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
//                Declare globals and include headers
////////////////////////////////////////////////////////////////////////////////




#include <Wire.h>               //I2C class
#include <SPI.h>                //SPI class
#include <SD.h>                 //sd card file access class



// #include "Adafruit_ADS1015.h"   //12 bit adc class

//altimeter
#include "Adafruit_MPL3115A2.h"    // altimeter
Adafruit_MPL3115A2 altimeter = Adafruit_MPL3115A2();
//end altimeter


//DHT22
#include "DHT.h"
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
//end DHT22





//determines how printing is handled for launch debug and graphing contexts
enum configuration { launch, debug, plot };
const configuration mode = launch;
char delim;  //the seperator that will be printed to seperate all values.



//uSD globals
File logFile;
char fileName[] = "logFile.txt"; // SD library only supports up to 8.3 names
const uint8_t chipSelect = 8;
const uint8_t cardDetect = 9;
bool alreadyBegan = false;  // SD.begin() misbehaves if not first call
//end uSD globals


//adc
// Adafruit_ADS1115 adc16Bit;
// int channelName = 0;  //uncomment this when adding analog channels to read.
//end adc





////////////////////////////////////////////////////////////////////////////////
//             End of global variables and included headers
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

    //humidity sensor
    dht.begin();



    /*
    header definitions:
    pascals(absPres)          = pressure in pascals read by the MPL3115A2 altimeter
    atmospheres(absPres)      = pressure in atmospheres read by the MPL3115A2 altimeter
    altitude(absPres)         = altitude in meters read by the MPL3115A2 altimeter
    tempC(absPres)            = temperature in C read by the MPL3115A2 altimeter
    millis:                   = a millis timestamp showing how many milliseconds the current program has been running
    */
    String header = ""; //create header variable

    header += "pascals(absPres),";
    header += "atmospheres(absPres),";
    header += "altitudeMeters(absPres),";
    header += "tempC(absPres),";

    header += "humidity%(dht22),";
    header += "tempF(dht22),";
    header += "heatIndxF(dht22),";


    header += "millis\n";
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



    //altimeter
    float pascals = altimeter.getPressure();
    logLine += pascals;
    logLine += delim;

    logLine += pascals / 101325;
    logLine += delim;

    logLine = altimeter.getAltitude();
    logLine += delim;

    // logLine = altimeter.getTemperature(); // in celcius
    logLine = ( ( 9 * altimeter.getTemperature() ) / 5) + 32; // in fahrenheight
    logLine += delim;
    //end altimeter






    //start dht22
    float humidity = dht.readHumidity(); //get humitidy percentage
    float tempF = dht.readTemperature(true); //read temp in fahrenheight

    // Check if any reads failed and exit early (to try again).
    if ( isnan(humidity) || isnan(tempF) ) {
      logLine += "-1";
      logLine += delim;
      logLine += "-1";
      logLine += delim;
      logLine += "-1";
      logLine += delim;
    }
    else {
        logLine += humidity
        logLine += delim;
        logLine += tempF;
        logLine += delim;
        logLine += dht.computeHeatIndex(tempF, humidity);
        logLine += delim;
    }

    //end dht22









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




/*
Arguments:
channel: choose the channel number, 0-3 in single ended, and 0-1 in differential
gain: determines how to interpret the channel argument and chooose between using
    single ended or differential class funtions and which appropriate one to use.
    Also chooses the appropriate multiplier for conversion from an ADC value to
    voltage.
single0_diff1: Boolean value for choosing between measuring a single ended signal
    and a differential signal. Choose 0 for single ended and 1 for differential.
                Single ended: channels directly correspond to actual ADC channels
                differential: channel 0 is differential between 0-1, and 1 is between 2-3

                                              ADS1015  ADS1115
                                              -------  -------
GAIN_TWOTHIRDS  2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
GAIN_ONE        1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
GAIN_TWO        2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
GAIN_FOUR       4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
GAIN_EIGHT      8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
GAIN_SIXTEEN    16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
*/
float getVoltageWGain(int channel, adsGain_t gain, bool single0_diff1){
    int16_t adcVal = 0;

    adc16Bit.setGain(gain);

    //read from the desired channel in single ended or differential
    //differential 0-1 is channel 0 and differential 2-3 is channel 1
    if(!single0_diff1){
        adcVal = adc16Bit.readADC_SingleEnded(channel);
    }
    else {
        if(channel % 2 == 0)adcVal = adc16Bit.readADC_Differential_0_1();
        else adcVal = adc16Bit.readADC_Differential_2_3();
    }

    //convert adc value to voltage using appropriate multiplier based on gain
    switch(gain){
        case GAIN_TWOTHIRDS:
            return adcVal * 0.1875 / 1000;
        break;
        case GAIN_ONE:
            return adcVal * 0.125 / 1000;
        break;
        case GAIN_TWO:
            return adcVal * 0.0625 / 1000;
        break;
        case GAIN_FOUR:
            return adcVal * 0.03125 / 1000;
        break;
        case GAIN_EIGHT:
            return adcVal * 0.015625 / 1000;
        break;
        case GAIN_SIXTEEN:
            return adcVal * 0.0078125 / 1000;
        break;
        default:
            return 0.0;
        break;
    }
}


////////////////////////////////////////////////////////////////////////////////
//                     End of function definitions
////////////////////////////////////////////////////////////////////////////////
