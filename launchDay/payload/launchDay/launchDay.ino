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


//determines how printing is handled for launch debug and graphing contexts
enum configuration { launch, debug, plot };
const configuration mode = launch;
char delim;  //the seperator that will be printed to seperate all values.





#include <Wire.h>               //I2C class
#include <SPI.h>                //SPI class
#include <SD.h>                 //sd card file access class




// 9DOF
#include <Adafruit_Sensor.h>
#include "Mahony.h"
#include "Madgwick.h"
#include "Adafruit_FXAS21002C.h"
#include "Adafruit_FXOS8700.h"

Adafruit_FXAS21002C gyro = Adafruit_FXAS21002C(0x0021002C);
Adafruit_FXOS8700 accelmag = Adafruit_FXOS8700(0x8700A, 0x8700B);


float mag_offsets[3]            = { 0.93F, -7.47F, -35.23F };
float mag_softiron_matrix[3][3] = { {  0.943,  0.011,  0.020 },
                                    {  0.022,  0.918, -0.008 },
                                    {  0.020, -0.008,  1.156 } };
float mag_field_strength        = 50.23F;
float gyro_zero_offsets[3]      = { 0.0F, 0.0F, 0.0F };
Mahony filter;
// end 9DOF



// thermocouple
#include "Adafruit_MAX31865.h"
Adafruit_MAX31865 max = Adafruit_MAX31865(4, 5, 6, 7);
#define RREF 430.0
// end thermocouple




// uv sensor
// #include "Adafruit_SI1145.h"
// Adafruit_SI1145 uv = Adafruit_SI1145();
//end uv sensor



// #include "Adafruit_ADS1015.h"   //12 bit adc class

// start GPS
// #include "Mercury.h"             //include the library
//
// #ifdef _VARIANT_ARDUINO_DUE_X_
//     Mercury venus(&Serial1);         // declare an instance of the library, passing the software serial object to the Mercury constructor.
// #else
//     #include <SoftwareSerial.h>     //include SoftwareSerial
//     SoftwareSerial venusSerial(10,11);  // setup pin 10 and 11 as rx and tx for a software serial port.
//     Mercury venus(&venusSerial);         // declare an instance of the library, passing the software serial object to the Mercury constructor.
// #endif
// end GPS


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

    delay(100);
    Serial.println(("Starting up..."));
    Serial.print(F("mode = "));
    if(mode == launch)  Serial.println(F("launch"));
    else if(mode == debug)  Serial.println(F("\ndebug"));
    else if(mode == plot)   Serial.println(F("\nplot"));



    //set delimiter type
    if(mode == plot) delim = '\t';
    else delim = ',';



    //uSd Card
    pinMode(cardDetect, INPUT);
    if(mode == launch) initializeCard();
    //end uSd Card



    // uv sensor
    // if (! uv.begin()) {
    //   Serial.println("Didn't find uv sensor");
    //   while (1);
    // }
    // end uv sensor



    //humidity sensor
   dht.begin();



   // thermocouple
   max.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary




    //gps
//    venus.begin(9600);
//    venus.setRunMode(filtered);
//    venus.setGpsTag("GPGGA");






    // 9DOF
    if(!gyro.begin())
    {
      /* There was a problem detecting the gyro ... check your connections */
      Serial.println("Ooops, no gyro detected ... Check your wiring!");
      if(mode == debug) while(1);
    }
    if(!accelmag.begin(ACCEL_RANGE_4G))
    {
      Serial.println("Ooops, no FXOS8700 detected ... Check your wiring!");
      if(mode == debug) while(1);
    }
    // Filter expects 70 samples per second
    // Based on a Bluefruit M0 Feather ... rate should be adjuted for other MCUs
    filter.begin(10);
    // end 9DOF







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

    // header += "visible(adaUV),";
    // header += "IR(adaUV),";
    // header += "UVIndx(adaUV),";

    header += "tempF(thermocouple),";

    header += "accelXG(9DOF),";
    header += "accelYG(9DOF),";
    header += "accelZG(9DOF),";
    header += "gyroXdegPerSec(9DOF),";
    header += "gyroYdegPerSec(9DOF),";
    header += "gyroZdegPerSec(9DOF),";
    header += "magXuTesla(9DOF),";
    header += "magYuTesla(9DOF),";
    header += "magZuTesla(9DOF),";
    header += "roll(9DOF),";
    header += "pitch(9DOF),";
    header += "heading(9DOF),";

    // header += "latitude(gps),";
    // header += "longitude(gps),";
    // header += "altitudeMeters(gps),";
    // header += "fixQuality(gps),";
    // header += "posDilution(gps),";
    // header += "geoidHeightMeters(gps),";
    // header += "numSatsTracked(gps),";
    // header += "fixTime(gps),";


    header += "millis\n";

    Serial.println(("Startup finished..."));

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
//    if (mode == debug) Se/rial.println(F("loop start"));

    //SD card detection/reinitialization
    if(mode == launch){
        if (!digitalRead(cardDetect)){
            initializeCard();
        }
    }
    //end SD card detection/reinitialization


    String logLine = "";        //create a sentence string for logging












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
        logLine += humidity;
        logLine += delim;
        logLine += tempF;
        logLine += delim;
       logLine += dht.computeHeatIndex(tempF, humidity);
        logLine += delim;
    }
    //end dht22


    // uv sensor
    // logLine += uv.readVisible();
    // logLine += delim;
    // logLine += uv.readIR();
    // logLine += delim;
    // logLine += uv.readUV() / 100.0;
    // logLine += delim;
    // end uv sensor







    // thermocouple sensor
    float thermocoupleTemp = max.temperature(100, RREF);
    uint8_t fault = max.readFault();
    if(fault) {
        logLine += "fault 0x";
        logLine += fault;
        logLine += delim;
        max.clearFault();
    }
    else {
        logLine += ( (thermocoupleTemp * 9) / 5) + 32;
        logLine += delim;
    }
    // end thermocouple sensor






    // 9DOF
    sensors_event_t gyro_event;
    sensors_event_t accel_event;
    sensors_event_t mag_event;

    gyro.getEvent(&gyro_event);
    accelmag.getEvent(&accel_event, &mag_event);

    float x = mag_event.magnetic.x - mag_offsets[0];
    float y = mag_event.magnetic.y - mag_offsets[1];
    float z = mag_event.magnetic.z - mag_offsets[2];
    float mx = x * mag_softiron_matrix[0][0] + y * mag_softiron_matrix[0][1] + z * mag_softiron_matrix[0][2];
    float my = x * mag_softiron_matrix[1][0] + y * mag_softiron_matrix[1][1] + z * mag_softiron_matrix[1][2];
    float mz = x * mag_softiron_matrix[2][0] + y * mag_softiron_matrix[2][1] + z * mag_softiron_matrix[2][2];
    float gx = gyro_event.gyro.x + gyro_zero_offsets[0];
    float gy = gyro_event.gyro.y + gyro_zero_offsets[1];
    float gz = gyro_event.gyro.z + gyro_zero_offsets[2];
    gx *= 57.2958F;
    gy *= 57.2958F;
    gz *= 57.2958F;

    float ax = accel_event.acceleration.x;
    float ay = accel_event.acceleration.y;
    float az = accel_event.acceleration.z;

    filter.update(gx, gy, gz,
                  ax, ay, az,
                  mx, my, mz);

    logLine += ax;
    logLine += delim;
    logLine += ay;
    logLine += delim;
    logLine += az;
    logLine += delim;

    logLine += gx;
    logLine += delim;
    logLine += gy;
    logLine += delim;
    logLine += gz;
    logLine += delim;

    logLine += mx;
    logLine += delim;
    logLine += my;
    logLine += delim;
    logLine += mz;
    logLine += delim;

    logLine += filter.getRoll();
    logLine += delim;
    logLine += filter.getPitch();
    logLine += delim;
    logLine += filter.getYaw();
    logLine += delim;
    // end 9DOF






    //start gps
    // check the GPS field explaination
    // to see the field numbers
//     char gpsData[100] = "";
//
//     Serial.println();
//
//     venus.readLine();
//     venus.getLine(gpsData);
//
//     Serial.println(venus.geLineSize());
//     Serial.println(gpsData);
//
//
//
//     venus.getField(gpsData, 2);  // latitude(gps)
//     Serial.println(gpsData);
// //    logLine += gpsData;
// //    logLine += delim;
//
//     venus.getField(gpsData, 5);  // longitude eastwest
//     Serial.print(gpsData);
// //    if(gpsData[0] == 'W') logLine += '-';
//
//     venus.getField(gpsData, 4);  // longitude(gps)
//     Serial.println(gpsData);
// //    logLine += gpsData;
// //    logLine += delim;
//
//     venus.getField(gpsData, 9);  // altitudeMeters(gps)
//     Serial.println(gpsData);
// //    logLine += gpsData;
// //    logLine += delim;
//
//     venus.getField(gpsData, 6);  // fixQuality(gps)
//     Serial.println(gpsData);
// //    logLine += gpsData;
// //    logLine += delim;
//
//     venus.getField(gpsData, 8);  // posDilution(gps)
//     Serial.println(gpsData);
// //    logLine += gpsData;
// //    logLine += delim;
//
//     venus.getField(gpsData, 11);  // geoidHeightMeters(gps)
//     Serial.println(gpsData);
// //    logLine += gpsData;
// //    logLine += delim;
//
//     venus.getField(gpsData, 7);  // numSatsTracked(gps)
//     Serial.println(gpsData);
// //    logLine += gpsData;
// //    logLine += delim;
//
//     venus.getField(gpsData, 1);  // fixTime(gps)
//     Serial.println(gpsData);
//    logLine += gpsData;
//    logLine += delim;
    //end gps



    if(mode != plot){
        logLine += millis();    // timestamp
    }


    logLine += '\n';    // last character should be a newline


    if(mode == debug) Serial.print(logLine);
    else if(mode == plot) Serial.print(logLine);
    else if(mode == launch) lineLogger(logLine);

    delay(20);     //for sanity

//    Serial.pri/ntln();
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




/* removed until we get back to the ADC
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
// float getVoltageWGain(int channel, adsGain_t gain, bool single0_diff1){
//     int16_t adcVal = 0;
//
//     adc16Bit.setGain(gain);
//
//     //read from the desired channel in single ended or differential
//     //differential 0-1 is channel 0 and differential 2-3 is channel 1
//     if(!single0_diff1){
//         adcVal = adc16Bit.readADC_SingleEnded(channel);
//     }
//     else {
//         if(channel % 2 == 0)adcVal = adc16Bit.readADC_Differential_0_1();
//         else adcVal = adc16Bit.readADC_Differential_2_3();
//     }
//
//     //convert adc value to voltage using appropriate multiplier based on gain
//     switch(gain){
//         case GAIN_TWOTHIRDS:
//             return adcVal * 0.1875 / 1000;
//         break;
//         case GAIN_ONE:
//             return adcVal * 0.125 / 1000;
//         break;
//         case GAIN_TWO:
//             return adcVal * 0.0625 / 1000;
//         break;
//         case GAIN_FOUR:
//             return adcVal * 0.03125 / 1000;
//         break;
//         case GAIN_EIGHT:
//             return adcVal * 0.015625 / 1000;
//         break;
//         case GAIN_SIXTEEN:
//             return adcVal * 0.0078125 / 1000;
//         break;
//         default:
//             return 0.0;
//         break;
//     }
// }








////////////////////////////////////////////////////////////////////////////////
//                     End of function definitions
////////////////////////////////////////////////////////////////////////////////
