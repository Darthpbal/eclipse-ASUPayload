/*
In this program, I'm basically going for detecting multiple
interrupt flags from this sensor while in a delay updating the
values from each photodiode channel and then printing the most up to date values.

this is a proof of concept for fetching new raw data from the lum sensor every time
its made available, while being able to work on other sensors. Then conversion to
lux and saturation detection can be handled later in the loop similarly to how it's
handled in the delay based demo.

I want to see in the serial monitor a 2.5 second delay (arbitrary value)
and want to see multiple flags for each interval, which should be ~6 or 7 counts
per 2.5 second windows for a 402ms light integration time.

This is built off of the Sparkfun TSL2561 luminosity sensor and the library that comes with it.

channel 0 is visible spectrum
channel 1 is ir spectrum
*/

#include <SparkFunTSL2561.h>
#include <Wire.h>




SFE_TSL2561 light;
const unsigned int lum_pin = 2;

unsigned int data0, data1;//variables for holding the data from lum sensor channel 0 and 1
volatile unsigned int vData0, vData1;//variables for handing off channel 0 and 1 from the ISR to main loop
boolean gain;     // Gain setting, 0 = X1, 1 = X16;

unsigned int lum_cnt;//holds the number of "integration finished" flags seen from the sensor. Should be one every integration cycle
volatile unsigned int lum_live_counts = 0; //hand off count between ISR and main loop

unsigned int ms;    //a handle for how many milliseconds the integration time is

int del = 2500;     //arbitrary amount of time for arduino to handle other sensors (in milliseconds) play with this and watch the count per window prediction



//this ISR "IsRunning" and interrupt activation/deactivation was taken from this link
//https://forum.arduino.cc/index.php?topic=344497.0
void lum_interrupt() {
    static boolean IsLumRunning = false;      //start by checking whether or not we're running.
    if(IsLumRunning) return;      //we have been interrupted while already processing an interrupt - ignore this ocurrence
    IsLumRunning = true;      //if we're not already running the interrupt, run it.
    interrupts();         //re-enable interrupts so that interrupt-based functions can be used inside this function like I2C
    //the sparkfun lum sensor requires the interrupt flag to be manually cleared. datasheet pg. 20 paragraph 4
    light.clearInterrupt();
    light.getData(vData0, vData1);        //since a new value is available from the lum sensor, check it
    lum_live_counts++;        //update the flag count for demo purposes
    noInterrupts();       //turn off interrupts so we can't be interrupted while resetting our special variable
    IsLumRunning = false;     //we're no longer running
    return;
}









void setup(){
    Serial.begin(9600);
    pinMode(lum_pin, INPUT);    //lum interrupt pin
    // falling edge because this is an active low interrupt. Seen on datasheet pg 20 paragraph 4
    // Since this is a falling edge interrupt, the adafruit product page hookup guide based on the same sensor recommends using a 10K to 100K ohm pullup resistor on the int pin.
    // linked here: https://learn.adafruit.com/tsl2561?view=all#wiring-up-the-sensor
    attachInterrupt( digitalPinToInterrupt(lum_pin), lum_interrupt, FALLING);   // interrupt signal is "active low" so we're watching for a falling edge


    light.begin();
    gain = 0;
    // If time = 0, integration will be 13.7ms
    // If time = 1, integration will be 101ms
    // If time = 2, integration will be 402ms
    // If time = 3, use manual start / stop to perform your own integration
    // If time = 2, integration will be 402ms
    // if integration time is 402ms,
    // I expect 6 or 7 flags per 2.5 second window
    // and 12 or 13 counts per 5 second window
    // and I expect the lum values to match the values normally seen by the general sparkfun demo
    unsigned char time = 2;
    // light.setTiming(gain,time);      //set integration gain and time mode.
    light.setTiming(gain,time,ms);      //only required if you want the number of milliseconds the integration window is

    unsigned char control = 1,       //use level interrupt, see setInterruptThreshold()
                    persist = 0;    //every integration cycle generates an interrupt
    unsigned int low = 0,           //Paragraph 5 and 6 of pg 20 indicate the thresholds don't matter if in persistence 0 mode.
                    high = 0;       // I'll set values to zero, just in case, and observe the interrup waveform before and after removal to be sure.
    light.setInterruptControl(control, persist);
    light.setPowerUp();             //powerup sensor
    light.clearInterrupt();         //must reset interrupt at startup so that the first falling edge can be detected, if never detected, it's never reset


    Serial.print(del / 1000.0, 1);      //print the delay in a float format to 1 decimal point
    Serial.print(" second data window started... Predicted count for current window is ");
    Serial.print( del / ms, DEC );      // predict how many flags will occur in each window
    Serial.print( " or " );
    Serial.println( (del / ms) + 1, DEC ); //round up
    Serial.println("channel 1\tchannel2\tcountPerWindow");
}






void loop(){
    //***HANDLE OTHER SENSORS***//
    delay(del);//I want to update the lum sensor while doing any other action for arbitrary amounts of time.
    //***END HANDLE OTHER SENSORS***//


    //***HANDLE LUM SENSORS***//
    //hand off data from the interrupt service routine
    data0 = vData0;
    data1 = vData1;

    //log data
    Serial.print(data0);
    Serial.print('\t');
    Serial.print(data1);
    Serial.print('\t');
    //***END HANDLE LUM SENSORS***//

    //reset the lum flag count, used just to show it's handling the ISR every single flag.
    lum_cnt = lum_live_counts;
    lum_live_counts = 0;
    Serial.print(lum_cnt);

    Serial.println('\n');//double newline
}









/*
***RESULTS



output from standard sparkfun example:
TSL2561 example sketch
Got factory ID: 0X50, should be 0X5X
Set timing...
Powerup...
data0: 162 data1: 100 lux: 8.70 (good)
data0: 162 data1: 100 lux: 8.70 (good)
data0: 162 data1: 100 lux: 8.70 (good)
data0: 162 data1: 100 lux: 8.70 (good)
data0: 162 data1: 100 lux: 8.70 (good)
data0: 162 data1: 100 lux: 8.70 (good)
data0: 162 data1: 100 lux: 8.70 (good)



output from this interrupt example at 1 second
1.0 second data window started... Predicted count for current window is 2 or 3
channel 1	channel2	countPerWindow
161           100           3
161           99            2
161           99            3
161           99            2
161           100           3
162           100           2
161           100           3
161           99            2
162           100           3
161           100           3
161           100           2
161           100           3
162           100           2
162           100           3
162           100           2
162           100           3
162           100           2



output from this interrupt example at 2.5 second
2.5 second data window started... Predicted count for current window is 6 or 7
channel 1	channel2	countPerWindow
161            99            6
159            98            7
161            99            6
161            99            6
161            99            7
160            99            6
161            99            6
161            99            7
161            99            6
161            99            6
161            99            7
161            99            6
161            99            6
161            99            7
160            99            6
161            99            6
161            100           6
161            99            7



output from this interrupt example at 5 second
5.0 second data window started... Predicted count for current window is 12 or 13
channel 1	channel2	countPerWindow
161            99            13
160            99            13
160            99            12
160            99            13
161            99            12
160            99            13
160            99            13
160            99            12
160            99            13
160            99            13
160            98            12
160            99            13
160            99            13
160            99            12
162            100           13
162            100           13
162            100           12
162            100           13
162            100           12



output from this interrupt example at 7 second
7.0 second data window started... Predicted count for current window is 17 or 18
channel 1	channel2	countPerWindow
163           100           18
162           100           17
163           100           18
163           100           18
163           100           17
163           100           18
162           100           18
163           100           18
163           100           17
163           100           18
163           100           18
163           100           17
163           100           18
163           100           18
163           101           17
163           101           18
164           101           18
164           101           18
163           101           17




normal method, but using arbitrary delay instead of the integration time
and alternating between uncovered and covered
5.0 second data window started...
channel 1	channel2
data0: 162 data1: 100 lux: 8.70 (good)
data0: 22 data1: 17 lux: 0.34 (good)
data0: 162 data1: 100 lux: 8.70 (good)
data0: 23 data1: 18 lux: 0.30 (good)
data0: 161 data1: 99 lux: 8.74 (good)
data0: 26 data1: 19 lux: 0.67 (good)
data0: 162 data1: 100 lux: 8.70 (good)



interrupt method alternating between covered and uncovered
5.0 second data window started... Predicted count for current window is 12 or 13
channel 1	channel2	countPerWindow
163           100           13
25            19            13
163           100           13
27            20            12
163           100           13
26            20            12
163           100           13
27            20            13
163           100           12
*/
