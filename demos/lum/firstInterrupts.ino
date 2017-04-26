#include <SparkFunTSL2561.h>
#include <Wire.h>




SFE_TSL2561 light;

unsigned int data0, data1;
volatile unsigned int vData0, vData1;
volatile bool errFlag = false;
double lux;    // Resulting lux value
boolean good,  // True if neither sensor is saturated
        gain;     // Gain setting, 0 = X1, 1 = X16;

const unsigned int lum_pin = 2;
volatile unsigned int lum_live_counts = 0;
unsigned int lum_cnt;


//assuming that persistence 0 shoul configure the interrupt to flag every ADC cycle, I wanna say this function is not needed...
void lum_interrupt(){
    lum_live_counts++;
}







void setup(){
    Serial.begin(9600);
    pinMode(lum_pin, INPUT);

    light.begin();
    gain = 0;
    // If time = 0, integration will be 13.7ms
    // If time = 1, integration will be 101ms
    // If time = 2, integration will be 402ms
    // If time = 3, use manual start / stop to perform your own integration
    unsigned char time = 2;
    // light.setTiming(gain,time,ms); //this appears to only be useful if using the ms variable for delaying manually.
    Serial.println( light.setTiming(gain,time), BIN);

    unsigned char control = 1,       //use level interrupt, see setInterruptThreshold()
                    persist = 0;    //every integration cycle generates an interrupt
    unsigned int low = 0,           //Paragraph 5 and 6 of pg 20 indicate the thresholds don't matter if in persistence 0 mode.
                    high = 0;       // I'll set values to zero, just in case, and observe the interrup waveform before and after removal to be sure.
    Serial.println( light.setInterruptControl(control, persist), BIN);
    Serial.println( light.setPowerUp(), BIN);
    attachInterrupt( digitalPinToInterrupt(lum_pin), lum_interrupt, FALLING);//active low interrupt
}






void loop(){
   Serial.println("5 second data window started...");
   Serial.println(errFlag);
   delay(5000);
   if(light.getData(data0, data1)) light.clearInterrupt();
   // light.getData(data0, data1);
   Serial.print(data0);
   Serial.print('\t');
   Serial.print(data1);
   Serial.print('\t');
   // If time = 2, integration will be 402ms
   // if integration time is 402ms, I expect ~12 flags per 5 second window

   lum_cnt = lum_live_counts;
   lum_live_counts = 0;

   Serial.print(lum_cnt);
   Serial.println('\t');
}







// library functions of interest

/*
SFE_TSL2561(void);

boolean begin(void);

boolean setPowerUp(void);

boolean setTiming(boolean gain, unsigned char time);
    // If gain = false (0), device is set to low gain (1X)
    // If gain = high (1), device is set to high gain (16X)
    // If time = 0, integration will be 13.7ms
    // If time = 1, integration will be 101ms
    // If time = 2, integration will be 402ms
    // If time = 3, use manual start / stop
    // Returns true (1) if successful, false (0) if there was an I2C error
    // (Also see getError() below)


boolean setTiming(boolean gain, unsigned char time, unsigned int &ms);
    // Identical to above command, except ms is set to selected integration time
    // If gain = false (0), device is set to low gain (1X)
    // If gain = high (1), device is set to high gain (16X)
    // If time = 0, integration will be 13.7ms
    // If time = 1, integration will be 101ms
    // If time = 2, integration will be 402ms
    // If time = 3, use manual start / stop (ms = 0)
    // ms will be set to requested integration time
    // Returns true (1) if successful, false (0) if there was an I2C error
    // (Also see getError() below)




boolean setInterruptControl(unsigned char control, unsigned char persist);
    // Sets up interrupt operations
    // If control = 0, interrupt output disabled
    // If control = 1, use level interrupt, see setInterruptThreshold()
    // If persist = 0, every integration cycle generates an interrupt
    // If persist = 1, any value outside of threshold generates an interrupt
    // If persist = 2 to 15, value must be outside of threshold for 2 to 15 integration cycles
    // Returns true (1) if successful, false (0) if there was an I2C error
    // (Also see getError() below)


boolean setInterruptThreshold(unsigned int low, unsigned int high);
    // Set interrupt thresholds (channel 0 only)
    // low, high: 16-bit threshold values
    // Returns true (1) if successful, false (0) if there was an I2C error
    // (Also see getError() below)


boolean clearInterrupt(void);
    // Clears an active interrupt
    // Returns true (1) if successful, false (0) if there was an I2C error
    // (Also see getError() below)


boolean getData(unsigned int &CH0, unsigned int &CH1);
boolean getLux(unsigned char gain, unsigned int ms, unsigned int CH0, unsigned int CH1, double &lux);



*/
