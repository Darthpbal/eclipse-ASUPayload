/*
This file was for testing as many sensors as I can together
that I currently had in hand before our second major purchase.

Sensors tested:
    uv sensor
    luminosity
    light 2 frequency
    12 bit ADC
    uSd logger

Sensors passed: - ready for Integration
    12 bit ADC
    uSd logger

Sensors not passed & why:
    uv sensor:
        unable to verify sensors output values are accurate.
        I get a noisy constant high-ish voltage from the sensor
        with a surprisingly small difference between direct sun
        brightness and a dark hallway darkness. Upon investigation,
        it appears UV sensors may have a uniquely short shelf life
        after initial exposire to moisture in the air.
    luminosity:
        This sensor appeared to work fine, however it's values need
        to be verified as well as the saturation variable returned
        true no matter what, which shouldn't happen. I've read that
        it could trip early since the IR range will hit a limit before
        the visible range, but I'll need more solo testing.
    light 2 frequency:
        A lag was discovered during this test and after further investigation
        it was due to this sensor alone. Theorhetically a faster controller
        but we need the controller in the mail to tell.
*/


/*
Todo:
    1. Make a scaling function to scale the values of the sensors to a common
        scale so that plot is actually useful instead of only seeing the largest
        value only.
    2.Make the file logging algorithm create a new file upon removal and
        reinssertion. This is preferable since many events could have happened
        between the points in time the sd card is inserted. So the timestamps
        aren't continuous.
*/

#include <SparkFunTSL2561.h>
#include <Wire.h>

#include <SPI.h>
#include <SD.h>

#include <Adafruit_ADS1015.h>


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


//LUM globals
SFE_TSL2561 light;
boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
//end LUM globals


//light to freq
int light2FreqPin = 3;
unsigned long hz; //stores the
volatile unsigned long livePulseCount = 0;
unsigned long oldPulseCount = 0;
unsigned long tempPulseCount = 0;
unsigned long lastSampleTime;
//end light to freq


//adc
Adafruit_ADS1015 adc11Bit;
int chanPotLeft = 0,
    chanPotRight = 1,
    chanDiffLeft2Right = 1;
//end adc


void setup() {
    Serial.begin(9600);
    Serial.print("mode = ");
    if(mode == launch) Serial.println("launch");
    else if(mode == debug) Serial.println("debug");
    else if(mode == plot) Serial.println("plot");

    //light to freq
    pinMode(light2FreqPin, INPUT);
    digitalWrite(light2FreqPin, HIGH);
    attachInterrupt(digitalPinToInterrupt(light2FreqPin), frequencyPulseCount, RISING);
    //end light to freq


    //uSd Card
    while (!Serial);  // Wait for serial port to connect (ATmega32U4 type PCBAs)
    pinMode(cardDetect, INPUT);
    if(mode == launch) initializeCard();
    //end uSd Card


    //LUM
    light.begin();
    gain = 0;
    unsigned char time = 2;
    light.setTiming(gain, time, ms);
    light.setPowerUp();
    //end LUM


    /*
    header definitions:
    lumRaw0:        = luminosity sensor channel 0, visible light spectrum
    lumRaw1:        = luminosity sensor channel 1, infrared light spectrum
    lumLvl:         = luminosity level in lux
    lumSat:         = boolean indicator for whether or not the photodiodes are saturated
    freq:           = frequency from the light to frequency converter
    mW/m2:          = irradiance from light to frequency converter in milliwatts per square meter
    adc0PotLeft:    = analog to digital converter board left potentiometer voltage
    adc1PotRight:   = analog to digital converter board right potentiometer voltage
    diffLeft2Right: = analog to digital converter board differential between left and right potentiometer voltages.
    */
    String header = "lumVisRaw,lumIrRaw,lumLvl,lumSat,lightfreq,lightFreqmW/m2,adc0PotLeft,adc1PotRight,diffLeft2Right,millis\n";
    if(mode  == debug) Serial.print(header);
    else if(mode == launch) lineLogger(header);
}



void loop() {
    //SD card detection/reinitialization
    if(mode == launch){
        if (!digitalRead(cardDetect)){
            initializeCard();
        }
    }
    //end SD card detection/reinitialization


    String logLine = "";        //create a sentence string for logging


    //LUM
    unsigned int chanVisRaw, chanIrRaw;
    if(light.getData(chanVisRaw, chanIrRaw)){
        logLine += chanVisRaw;
        logLine += (mode == launch)? ',': '\t';
        logLine += chanIrRaw;
        logLine += (mode == launch)? ',': '\t';
        double lux;
        boolean goodLuxVal = light.getLux(gain, ms, chanVisRaw, chanIrRaw, lux);
        logLine += lux;
        logLine += (mode == launch)? ',': '\t';
        logLine += goodLuxVal;
        logLine += (mode == launch)? ',': '\t';
    }
    else {
        logLine += "lumI2C error";
        for(int i = 0; i < 4; i++){
            logLine += (mode == launch)? ',': ' ';
        }
    }
    //end LUM


    // this algorithm has been changed from it's original form, but the values appear to be pretty similar. I'd like to test to make sure these values are accurate.
    //light to freq
    unsigned long deltaTime = millis() - lastSampleTime;
    tempPulseCount = livePulseCount;
    hz = tempPulseCount - oldPulseCount; // peak count between time stamps
    hz *= (1000 / deltaTime); //convert from milliseconds to seconds and divide by delta time.
    logLine += hz;
    logLine += (mode == launch)? ',': '\t';
    logLine += (hz+50) / 100;  // +50 == rounding last digit
    logLine += (mode == launch)? ',': '\t';
    oldPulseCount = tempPulseCount;
    lastSampleTime = millis();
    //end light to freq


    //ADC
    logLine += getVoltageWGain(chanPotLeft, GAIN_ONE, 0);
    logLine += (mode == launch)? ',': '\t';
    logLine += getVoltageWGain(chanPotRight, GAIN_ONE, 0);
    logLine += (mode == launch)? ',': '\t';
    logLine += getVoltageWGain(chanDiffLeft2Right, GAIN_ONE, 1);
    logLine += (mode == launch)? ',': '\t';
    //end ADC


    if(mode != plot){
        logLine += millis();    // timestamp
    }


    logLine += '\n';    // last character should be a newline


    if(mode == debug) Serial.print(logLine);
    else if(mode == plot) Serial.print(logLine);
    else if(mode == launch) lineLogger(logLine);

    delay(150);     //for sanity

}



// counts the number of pulses seen from the light to frequency converter.
void frequencyPulseCount(){
    livePulseCount++;
}



////////////////////////////////////////////////////////////////////////////////
// Do everything from detecting card through opening the demo file
////////////////////////////////////////////////////////////////////////////////
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

    Serial.print("Opening file: ");
    Serial.println(fileName);
}



////////////////////////////////////////////////////////////////////////////////
// Write the buffer to the log file. If we are possibly in the EOF state, verify
// that to make sure the command isn't written to the file.
////////////////////////////////////////////////////////////////////////////////
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

GAIN_TWOTHIRDS   2/3x gain +/- 6.144V  1 bit = 3mV    (default)
GAIN_ONE         1x gain   +/- 4.096V  1 bit = 2mV
GAIN_TWO         2x gain   +/- 2.048V  1 bit = 1mV
GAIN_FOUR        4x gain   +/- 1.024V  1 bit = 0.5mV
GAIN_EIGHT       8x gain   +/- 0.512V  1 bit = 0.25mV
GAIN_SIXTEEN     16x gain  +/- 0.256V  1 bit = 0.125mV
*/
float getVoltageWGain(int channel, adsGain_t gain, bool single0_diff1){
    int16_t adcVal = 0;

    adc11Bit.setGain(gain);

    //read from the desired channel in single ended or differential
    //differential 0-1 is channel 0 and differential 2-3 is channel 1
    if(!single0_diff1){
        adcVal = adc11Bit.readADC_SingleEnded(channel);
    }
    else {
        if(channel % 2 == 0)adcVal = adc11Bit.readADC_Differential_0_1();
        else adcVal = adc11Bit.readADC_Differential_2_3();
    }

    //convert adc value to voltage using appropriate multiplier based on gain
    switch(gain){
        case GAIN_TWOTHIRDS:
            return adcVal * 3.0 / 1000;
        break;
        case GAIN_ONE:
            return adcVal * 2.0 / 1000;
        break;
        case GAIN_TWO:
            return adcVal * 1.0 / 1000;
        break;
        case GAIN_FOUR:
            return adcVal * 0.5 / 1000;
        break;
        case GAIN_EIGHT:
            return adcVal * 0.25 / 1000;
        break;
        case GAIN_SIXTEEN:
            return adcVal * 0.125 / 1000;
        break;
        default:
            return 0.0;
        break;
    }
}








/*
***  RESULTS  ***
text captured from the serial monitor:

Initializing SD card...No card detected. Waiting for card.
Initialization done.
logFile.txt doesn't exist. Creating.
Opening file: logFile.txt
Initializing SD card...No card detected. Waiting for card.
Initialization done.
logFile.txt exists.
Opening file: logFile.txt
Initializing SD card...No card detected. Waiting for card.







contents of the resulting logfile:

lumVisRaw,lumIrRaw,lumLvl,lumSat,freq,mW/m2,adc0PotLeft,adc1PotRight,diffLeft2Right
127,83,5.69,1,0,0mW/m2,1.47,1.66,-0.19,8792
107,72,4.29,1,6515,65mW/m2,1.52,1.71,-0.19,8967
107,72,4.29,1,9945,99mW/m2,1.52,1.71,-0.19,9141
132,90,5.00,1,10340,103mW/m2,1.53,1.71,-0.19,9317
132,90,5.00,1,10335,103mW/m2,1.53,1.71,-0.19,9492
132,90,5.00,1,10340,103mW/m2,1.53,1.71,-0.19,9667
160,108,6.33,1,10330,103mW/m2,1.53,1.71,-0.19,9842
160,108,6.33,1,10325,103mW/m2,1.53,1.72,-0.19,10017
160,108,6.33,1,10330,103mW/m2,1.53,1.71,-0.19,10192
160,108,6.33,1,10242,102mW/m2,1.53,1.71,-0.19,10482
160,108,6.33,1,10335,103mW/m2,1.53,1.71,-0.18,10657
160,108,6.33,1,10330,103mW/m2,1.53,1.71,-0.18,10832
160,108,6.33,1,10335,103mW/m2,1.53,1.71,-0.18,11008
160,108,6.33,1,10335,103mW/m2,1.53,1.71,-0.19,11184
160,108,6.33,1,10320,103mW/m2,1.53,1.71,-0.19,11359
160,108,6.33,1,10315,103mW/m2,1.53,1.71,-0.19,11534
160,108,6.33,1,10320,103mW/m2,1.53,1.71,-0.19,11709
160,108,6.33,1,10320,103mW/m2,1.53,1.71,-0.19,11884
160,108,6.33,1,10330,103mW/m2,1.53,1.71,-0.19,12060
160,108,6.33,1,10284,103mW/m2,1.53,1.71,-0.19,12351
160,108,6.33,1,10315,103mW/m2,1.53,1.71,-0.18,12526
160,108,6.33,1,10315,103mW/m2,1.53,1.71,-0.18,12701
160,108,6.33,1,10310,103mW/m2,1.53,1.72,-0.18,12876
160,108,6.33,1,10060,101mW/m2,1.53,1.72,-0.18,13089
160,108,6.33,1,10305,103mW/m2,1.53,1.72,-0.18,13265
160,108,6.33,1,10305,103mW/m2,1.53,1.72,-0.18,13441
160,108,6.33,1,10305,103mW/m2,1.53,1.72,-0.18,13616
160,108,6.33,1,10295,103mW/m2,1.53,1.72,-0.18,13791
160,108,6.33,1,10300,103mW/m2,1.53,1.72,-0.18,13966
160,108,6.33,1,10215,102mW/m2,1.53,1.72,-0.18,14255
160,108,6.33,1,10275,103mW/m2,1.53,1.72,-0.18,14430
160,108,6.33,1,10270,103mW/m2,1.53,1.72,-0.18,14605
160,108,6.33,1,10275,103mW/m2,1.53,1.72,-0.18,14779
160,108,6.33,1,10280,103mW/m2,1.53,1.72,-0.18,14954
160,108,6.33,1,10290,103mW/m2,1.53,1.72,-0.18,15129
160,108,6.33,1,10285,103mW/m2,1.53,1.72,-0.18,15303
160,108,6.33,1,10935,109mW/m2,1.53,1.72,-0.18,15490
160,108,6.33,1,10280,103mW/m2,1.53,1.72,-0.18,15664
160,108,6.33,1,10285,103mW/m2,1.53,1.72,-0.18,15839
160,108,6.33,1,10605,106mW/m2,1.53,1.72,-0.18,16019
160,108,6.33,1,10300,103mW/m2,1.53,1.72,-0.18,16194
160,108,6.33,1,10295,103mW/m2,1.53,1.72,-0.18,16369
160,108,6.33,1,10300,103mW/m2,1.53,1.72,-0.18,16543
160,108,6.33,1,10300,103mW/m2,1.53,1.72,-0.18,16718
160,108,6.33,1,10300,103mW/m2,1.53,1.72,-0.18,16893
160,108,6.33,1,10305,103mW/m2,1.53,1.72,-0.18,17068
160,108,6.33,1,10305,103mW/m2,1.53,1.72,-0.18,17243
160,108,6.33,1,10295,103mW/m2,1.53,1.72,-0.18,17418
160,108,6.33,1,10290,103mW/m2,1.53,1.72,-0.18,17593
160,108,6.33,1,10268,103mW/m2,1.54,1.72,-0.18,17811
160,108,6.33,1,10305,103mW/m2,1.53,1.72,-0.18,17986
160,108,6.33,1,10350,104mW/m2,1.54,1.72,-0.18,18161
160,108,6.33,1,10350,104mW/m2,1.53,1.72,-0.18,18335
161,108,6.53,1,10335,103mW/m2,1.53,1.72,-0.18,18510
161,108,6.53,1,10315,103mW/m2,1.54,1.72,-0.18,18685
160,108,6.33,1,10285,103mW/m2,1.53,1.72,-0.18,18861
160,108,6.33,1,10255,103mW/m2,1.53,1.72,-0.18,19036
159,107,6.37,1,10245,102mW/m2,1.53,1.72,-0.18,19211
159,107,6.37,1,10550,106mW/m2,1.53,1.72,-0.18,19391
159,107,6.37,1,10225,102mW/m2,1.53,1.72,-0.18,19565
159,107,6.37,1,10230,102mW/m2,1.53,1.72,-0.18,19740
159,107,6.37,1,10230,102mW/m2,1.53,1.72,-0.18,19915
159,107,6.37,1,10235,102mW/m2,1.53,1.72,-0.18,20089
159,107,6.37,1,10240,102mW/m2,1.53,1.72,-0.18,20264
159,107,6.37,1,10250,103mW/m2,1.53,1.72,-0.18,20440
159,107,6.37,1,10285,103mW/m2,1.53,1.72,-0.18,20614
160,108,6.33,1,10310,103mW/m2,1.53,1.72,-0.18,20789
160,108,6.33,1,10315,103mW/m2,1.53,1.72,-0.18,20964
161,108,6.53,1,10630,106mW/m2,1.53,1.72,-0.18,21144
161,108,6.53,1,10310,103mW/m2,1.53,1.72,-0.18,21318
161,108,6.53,1,10320,103mW/m2,1.53,1.72,-0.18,21493
161,108,6.53,1,10325,103mW/m2,1.53,1.72,-0.18,21668
161,108,6.53,1,10325,103mW/m2,1.53,1.72,-0.18,21842
161,109,6.29,1,10980,110mW/m2,1.53,1.72,-0.18,22029
161,109,6.29,1,10084,101mW/m2,1.53,1.72,-0.18,22242
161,109,6.29,1,10340,103mW/m2,1.53,1.72,-0.18,22416
161,109,6.29,1,10340,103mW/m2,1.53,1.72,-0.18,22591
161,109,6.29,1,10325,103mW/m2,1.53,1.72,-0.18,22766
161,109,6.29,1,11375,114mW/m2,1.53,1.72,-0.18,22959
161,109,6.29,1,10500,105mW/m2,1.53,1.72,-0.18,23136
161,109,6.29,1,10490,105mW/m2,1.53,1.72,-0.18,23313
161,109,6.29,1,10495,105mW/m2,1.53,1.72,-0.18,23491
161,109,6.29,1,10495,105mW/m2,1.53,1.72,-0.18,23668
161,109,6.29,1,10480,105mW/m2,1.53,1.72,-0.18,23846
161,109,6.29,1,10480,105mW/m2,1.53,1.72,-0.18,24024
161,109,6.29,1,10500,105mW/m2,1.53,1.72,-0.18,24201
161,109,6.29,1,10475,105mW/m2,1.53,1.72,-0.18,24379
161,109,6.29,1,10485,105mW/m2,1.53,1.72,-0.18,24556
161,109,6.29,1,10795,108mW/m2,1.53,1.72,-0.18,24739
161,109,6.29,1,10515,105mW/m2,1.53,1.72,-0.18,24916
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,25095
161,109,6.29,1,10500,105mW/m2,1.53,1.72,-0.18,25273
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,25450
161,109,6.29,1,10510,105mW/m2,1.53,1.72,-0.18,25628
161,109,6.29,1,10500,105mW/m2,1.53,1.72,-0.18,25805
161,109,6.29,1,10510,105mW/m2,1.53,1.72,-0.18,25984
161,109,6.29,1,10515,105mW/m2,1.53,1.72,-0.18,26162
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,26339
161,109,6.29,1,10825,108mW/m2,1.53,1.72,-0.18,26522
161,109,6.29,1,10500,105mW/m2,1.53,1.72,-0.18,26699
161,109,6.29,1,10208,102mW/m2,1.53,1.72,-0.18,26915
161,109,6.29,1,10515,105mW/m2,1.53,1.72,-0.18,27092
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,27271
161,109,6.29,1,10500,105mW/m2,1.53,1.72,-0.18,27448
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,27625
161,109,6.29,1,10515,105mW/m2,1.53,1.72,-0.18,27803
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,27980
161,109,6.29,1,10500,105mW/m2,1.53,1.72,-0.18,28157
161,109,6.29,1,10810,108mW/m2,1.53,1.72,-0.18,28341
161,109,6.29,1,10515,105mW/m2,1.53,1.72,-0.18,28518
161,109,6.29,1,11160,112mW/m2,1.53,1.72,-0.18,28707
161,109,6.29,1,10515,105mW/m2,1.53,1.72,-0.18,28884
161,109,6.29,1,10530,105mW/m2,1.53,1.72,-0.18,29062
161,109,6.29,1,10520,105mW/m2,1.53,1.72,-0.18,29240
161,109,6.29,1,10515,105mW/m2,1.53,1.72,-0.19,29417
161,109,6.29,1,10525,105mW/m2,1.53,1.72,-0.18,29595
161,109,6.29,1,10510,105mW/m2,1.53,1.72,-0.18,29772
161,109,6.29,1,10840,108mW/m2,1.53,1.72,-0.18,29956
161,109,6.29,1,10515,105mW/m2,1.53,1.72,-0.18,30133
162,109,6.49,1,10525,105mW/m2,1.53,1.72,-0.18,30310
162,109,6.49,1,10525,105mW/m2,1.53,1.72,-0.18,30488
161,109,6.29,1,10510,105mW/m2,1.53,1.72,-0.18,30665
161,109,6.29,1,10520,105mW/m2,1.53,1.72,-0.18,30843
161,109,6.29,1,10525,105mW/m2,1.53,1.72,-0.18,31021
161,109,6.29,1,10520,105mW/m2,1.53,1.72,-0.18,31198
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,31376
161,109,6.29,1,10200,102mW/m2,1.53,1.72,-0.18,31592
161,109,6.29,1,10790,108mW/m2,1.53,1.72,-0.18,31774
161,108,6.53,1,10490,105mW/m2,1.53,1.72,-0.18,31952
161,108,6.53,1,10485,105mW/m2,1.53,1.72,-0.18,32130
160,108,6.33,1,10465,105mW/m2,1.53,1.72,-0.18,32308
160,108,6.33,1,10460,105mW/m2,1.53,1.72,-0.18,32485
160,108,6.33,1,10475,105mW/m2,1.53,1.72,-0.18,32663
160,108,6.33,1,10470,105mW/m2,1.53,1.72,-0.18,32841
160,108,6.33,1,10470,105mW/m2,1.53,1.72,-0.18,33018
160,108,6.33,1,10495,105mW/m2,1.53,1.72,-0.18,33197
160,108,6.33,1,10480,105mW/m2,1.53,1.72,-0.18,33374
160,108,6.33,1,10790,108mW/m2,1.53,1.72,-0.19,33557
160,108,6.33,1,10465,105mW/m2,1.53,1.72,-0.18,33734
160,108,6.33,1,10480,105mW/m2,1.53,1.72,-0.18,33912
160,108,6.33,1,10485,105mW/m2,1.53,1.72,-0.19,34089
160,108,6.33,1,10480,105mW/m2,1.53,1.72,-0.18,34268
160,108,6.33,1,10470,105mW/m2,1.53,1.72,-0.18,34445
160,108,6.33,1,10480,105mW/m2,1.53,1.72,-0.18,34622
160,108,6.33,1,10470,105mW/m2,1.53,1.72,-0.18,34800
160,108,6.33,1,10465,105mW/m2,1.53,1.72,-0.18,34977
160,108,6.33,1,10490,105mW/m2,1.53,1.72,-0.18,35155
160,108,6.33,1,11440,114mW/m2,1.53,1.72,-0.18,35349
160,108,6.33,1,10475,105mW/m2,1.53,1.72,-0.18,35526
160,108,6.33,1,10480,105mW/m2,1.53,1.72,-0.18,35703
160,108,6.33,1,10480,105mW/m2,1.53,1.72,-0.18,35881
160,108,6.33,1,10184,102mW/m2,1.53,1.72,-0.18,36098
161,108,6.53,1,10485,105mW/m2,1.53,1.72,-0.18,36275
161,108,6.53,1,10460,105mW/m2,1.53,1.72,-0.18,36452
160,108,6.33,1,10465,105mW/m2,1.53,1.72,-0.19,36630
160,108,6.33,1,10480,105mW/m2,1.53,1.72,-0.18,36807
160,108,6.33,1,10475,105mW/m2,1.53,1.72,-0.18,36984
160,108,6.33,1,11525,115mW/m2,1.53,1.72,-0.18,37180
161,108,6.53,1,10495,105mW/m2,1.53,1.72,-0.18,37358
161,108,6.53,1,10485,105mW/m2,1.53,1.72,-0.18,37535
160,108,6.33,1,10480,105mW/m2,1.53,1.72,-0.19,37712
160,108,6.33,1,10485,105mW/m2,1.53,1.72,-0.18,37891
160,108,6.33,1,10485,105mW/m2,1.53,1.72,-0.18,38068
161,108,6.53,1,10480,105mW/m2,1.53,1.72,-0.18,38246
161,108,6.53,1,10480,105mW/m2,1.53,1.72,-0.18,38423
161,108,6.53,1,10490,105mW/m2,1.53,1.72,-0.18,38600
161,108,6.53,1,10490,105mW/m2,1.53,1.72,-0.18,38778
161,108,6.53,1,10785,108mW/m2,1.53,1.72,-0.18,38961
161,108,6.53,1,10500,105mW/m2,1.53,1.72,-0.18,39139
161,108,6.53,1,10520,105mW/m2,1.53,1.72,-0.18,39316
161,108,6.53,1,10500,105mW/m2,1.53,1.72,-0.18,39494
161,108,6.53,1,10500,105mW/m2,1.53,1.72,-0.18,39672
161,109,6.29,1,10515,105mW/m2,1.53,1.72,-0.18,39849
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,40028
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,40205
161,109,6.29,1,10510,105mW/m2,1.53,1.72,-0.18,40383
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,40561
161,109,6.29,1,10444,104mW/m2,1.53,1.72,-0.18,40782
161,109,6.29,1,10495,105mW/m2,1.53,1.72,-0.18,40960
161,109,6.29,1,10495,105mW/m2,1.53,1.72,-0.18,41137
161,109,6.29,1,10495,105mW/m2,1.53,1.72,-0.18,41315
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,41492
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,41670
161,109,6.29,1,10490,105mW/m2,1.53,1.72,-0.18,41847
161,109,6.29,1,11145,111mW/m2,1.53,1.72,-0.18,42036
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,42214
161,109,6.29,1,10820,108mW/m2,1.53,1.72,-0.18,42396
162,109,6.49,1,10505,105mW/m2,1.53,1.72,-0.18,42573
162,109,6.49,1,10515,105mW/m2,1.53,1.72,-0.18,42752
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,42929
161,109,6.29,1,10500,105mW/m2,1.53,1.72,-0.18,43106
162,109,6.49,1,10515,105mW/m2,1.53,1.72,-0.18,43284
162,109,6.49,1,10505,105mW/m2,1.53,1.72,-0.18,43461
161,109,6.29,1,10490,105mW/m2,1.53,1.72,-0.18,43639
161,109,6.29,1,10495,105mW/m2,1.53,1.72,-0.18,43816
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,43994
161,109,6.29,1,10825,108mW/m2,1.53,1.72,-0.18,44177
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,44355
161,109,6.29,1,10485,105mW/m2,1.53,1.72,-0.18,44532
161,109,6.29,1,10500,105mW/m2,1.53,1.72,-0.18,44709
161,109,6.29,1,10500,105mW/m2,1.53,1.72,-0.18,44888
161,109,6.29,1,10490,105mW/m2,1.53,1.72,-0.18,45065
161,109,6.29,1,10505,105mW/m2,1.53,1.72,-0.18,45243
161,109,6.29,1,10208,102mW/m2,1.53,1.72,-0.18,45458
162,109,6.49,1,10520,105mW/m2,1.53,1.72,-0.18,45636
162,109,6.49,1,10510,105mW/m2,1.53,1.72,-0.18,45813
162,109,6.49,1,10820,108mW/m2,1.53,1.72,-0.18,45997
162,109,6.49,1,10535,105mW/m2,1.53,1.72,-0.18,46174
162,109,6.49,1,10535,105mW/m2,1.53,1.72,-0.18,46352
162,109,6.49,1,10525,105mW/m2,1.53,1.72,-0.18,46529
162,109,6.49,1,10520,105mW/m2,1.53,1.72,-0.18,46707
162,109,6.49,1,10525,105mW/m2,1.53,1.72,-0.18,46885
162,109,6.49,1,10530,105mW/m2,1.53,1.72,-0.18,47063
162,109,6.49,1,10530,105mW/m2,1.53,1.72,-0.18,47241
162,109,6.49,1,10535,105mW/m2,1.53,1.72,-0.18,47418
162,109,6.49,1,10560,106mW/m2,1.53,1.72,-0.18,47596
162,109,6.49,1,10845,108mW/m2,1.53,1.72,-0.18,47779
162,109,6.49,1,10400,104mW/m2,1.53,1.72,-0.18,47956
161,109,6.29,1,10260,103mW/m2,1.53,1.72,-0.18,48134
161,109,6.29,1,10350,104mW/m2,1.53,1.72,-0.18,48312
160,108,6.33,1,10395,104mW/m2,1.52,1.70,-0.18,48489
160,108,6.33,1,10680,107mW/m2,1.48,1.69,-0.18,48667
165,111,6.62,1,13525,135mW/m2,1.39,1.61,-0.18,48856
165,111,6.62,1,21120,211mW/m2,1.54,1.71,-0.18,49034
212,133,10.86,1,24445,244mW/m2,1.50,1.69,-0.19,49213
212,133,10.86,1,24900,249mW/m2,1.46,1.55,-0.18,49392
235,143,13.30,1,26430,264mW/m2,1.52,1.71,-0.19,49577
235,143,13.30,1,25750,258mW/m2,1.51,1.70,-0.19,49757
241,145,14.45,1,25068,251mW/m2,1.41,1.60,-0.18,49975
241,145,14.45,1,26010,260mW/m2,1.48,1.64,-0.18,50154
243,144,15.67,1,26330,263mW/m2,1.53,1.71,-0.18,50333
243,144,15.67,1,26240,262mW/m2,1.51,1.69,-0.17,50513
243,144,15.67,1,26195,262mW/m2,1.41,1.56,-0.16,50693
242,141,16.80,1,26120,261mW/m2,1.36,1.56,-0.17,50872
242,141,16.80,1,25355,254mW/m2,1.50,1.69,-0.18,51052
238,139,16.36,1,24660,247mW/m2,1.52,1.71,-0.19,51250
238,139,16.36,1,15695,157mW/m2,1.49,1.69,-0.18,51428
206,126,11.34,1,11040,110mW/m2,1.52,1.71,-0.19,51606
206,126,11.34,1,9220,92mW/m2,1.53,1.71,-0.19,51783
179,113,9.00,1,9200,92mW/m2,1.53,1.72,-0.19,51960
179,113,9.00,1,9160,92mW/m2,1.53,1.72,-0.19,52137
175,111,8.67,1,9280,93mW/m2,1.53,1.72,-0.19,52316
175,111,8.67,1,9280,93mW/m2,1.53,1.72,-0.19,52493
175,111,8.67,1,9305,93mW/m2,1.53,1.72,-0.19,52670
176,112,8.63,1,9330,93mW/m2,1.53,1.72,-0.19,52847
176,112,8.63,1,9615,96mW/m2,1.53,1.72,-0.19,53030
176,112,8.63,1,9365,94mW/m2,1.53,1.72,-0.19,53208
176,112,8.63,1,9380,94mW/m2,1.53,1.72,-0.19,53385
177,113,8.59,1,9405,94mW/m2,1.53,1.72,-0.19,53563
177,113,8.59,1,9415,94mW/m2,1.53,1.72,-0.19,53740
177,113,8.59,1,9405,94mW/m2,1.53,1.72,-0.19,53918
177,113,8.59,1,9390,94mW/m2,1.53,1.71,-0.19,54095
177,113,8.59,1,9380,94mW/m2,1.53,1.72,-0.19,54274
177,112,8.83,1,9355,94mW/m2,1.53,1.72,-0.19,54451
177,112,8.83,1,9096,91mW/m2,1.53,1.72,-0.19,54667
176,112,8.63,1,9345,93mW/m2,1.53,1.72,-0.19,54844
176,112,8.63,1,9625,96mW/m2,1.53,1.72,-0.19,55026
176,112,8.63,1,9355,94mW/m2,1.53,1.72,-0.19,55204
176,112,8.63,1,9920,99mW/m2,1.53,1.72,-0.19,55393
176,112,8.63,1,9190,92mW/m2,1.53,1.72,-0.19,55570
176,112,8.63,1,4880,49mW/m2,1.53,1.72,-0.19,55747
96,61,4.73,1,2370,24mW/m2,1.53,1.72,-0.19,55923
96,61,4.73,1,2370,24mW/m2,1.53,1.71,-0.19,56100
44,19,7.93,1,2305,23mW/m2,1.52,1.71,-0.19,56278
44,19,7.93,1,2440,24mW/m2,1.53,1.71,-0.19,56455
44,19,7.93,1,2495,25mW/m2,1.53,1.72,-0.19,56631
47,14,14.31,1,2615,26mW/m2,1.53,1.72,-0.19,56813
47,14,14.31,1,2585,26mW/m2,1.53,1.72,-0.19,56990
48,13,15.70,1,2635,26mW/m2,1.53,1.71,-0.19,57167
48,13,15.70,1,2650,27mW/m2,1.52,1.71,-0.19,57345
47,13,15.15,1,2445,24mW/m2,1.53,1.71,-0.19,57521
47,13,15.15,1,8055,81mW/m2,1.53,1.72,-0.19,57698
126,79,6.47,1,9335,93mW/m2,1.53,1.72,-0.19,57876
126,79,6.47,1,9325,93mW/m2,1.53,1.72,-0.19,58053
176,112,8.63,1,9330,93mW/m2,1.53,1.72,-0.19,58230
176,112,8.63,1,9330,93mW/m2,1.53,1.72,-0.19,58407
176,112,8.63,1,9315,93mW/m2,1.53,1.72,-0.19,58585
176,112,8.63,1,9625,96mW/m2,1.53,1.72,-0.19,58768
176,112,8.63,1,9340,93mW/m2,1.53,1.72,-0.19,58945
176,112,8.63,1,9370,94mW/m2,1.53,1.72,-0.19,59122
176,112,8.63,1,9084,91mW/m2,1.53,1.72,-0.19,59338
175,112,8.42,1,9320,93mW/m2,1.53,1.72,-0.19,59515
175,112,8.42,1,9310,93mW/m2,1.53,1.72,-0.19,59693
174,111,8.46,1,9300,93mW/m2,1.53,1.72,-0.19,59871
174,111,8.46,1,9300,93mW/m2,1.53,1.72,-0.19,60048
175,112,8.42,1,9305,93mW/m2,1.53,1.72,-0.19,60225
175,112,8.42,1,9295,93mW/m2,1.53,1.72,-0.19,60402
175,112,8.42,1,9580,96mW/m2,1.53,1.72,-0.19,60585
175,112,8.42,1,9325,93mW/m2,1.53,1.72,-0.19,60763
175,112,8.42,1,9300,93mW/m2,1.53,1.72,-0.19,60941
175,112,8.42,1,9065,91mW/m2,1.53,1.72,-0.19,61118
175,112,8.42,1,4030,40mW/m2,1.52,1.71,-0.19,61295
109,70,5.19,1,4325,43mW/m2,1.45,1.63,-0.18,61472
109,70,5.19,1,4085,41mW/m2,1.41,1.58,-0.17,61649
106,65,5.80,1,3640,36mW/m2,1.39,1.54,-0.16,61826
106,65,5.80,1,3510,35mW/m2,1.43,1.65,-0.17,62003



*/
