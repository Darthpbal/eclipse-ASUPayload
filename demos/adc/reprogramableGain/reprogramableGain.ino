/*
***This chart gain comparison chart is from Adafruit
The ADC input range (or gain) can be changed via the following
functions, but be careful never to exceed VDD +0.3V max, or to
exceed the upper and lower limits if you adjust the input range!
Setting these values incorrectly may destroy your ADC!
                                                               ADS1015  ADS1115
                                                               -------  -------
ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
*/





/*
The point of this demo is to show a way to directly return
a decimal float value with configurable gain in a single
understandable line.

The content of this test will be measuring two potentiometers at different gain
levels in single ended mode (one signal wire) and in differential (two signal wires
subtracted) in negative direction and show reprogramming gain on the fly.
The various gain levels will scale 11 unsigned bits over positive voltages or 12
signed bits of resolution over positive and negative voltage across the input
range that corresponds to your gain setting. So I'll see a different ADC value at
each gain but the conversion will convert the value to a float correctly anyway
using the provided gain argument to choose a multiplier.

My test signals are two potentiometers left and right, left configured to 1.0V,
the right configured to 1.5V.
Three gain settings at 2/3, 1, and 2 will be tested, and these voltage values
are within these safe input limits for their corresponding gain setting.
channel   = label               = actual input voltage          = resulting voltage
channel 1 = right potentiometer = 1.008 (aim ~1.0V)             = 1.0200
channel 2 = left potentiometer  = 1.515 (aim ~1.5V)             = 1.5120
channel 3 = positive diff input =  ~1.0V result -0.485 (~-0.5V) = -0.4920
channel 4 = negative diff input =  ~1.5V result -0.485 (~-0.5V) = -0.4920




test run results captured from serial monitor

rightGain2/3	leftGain1	diff2-3GainX2	diff2-3GainX4
+-6.144V	+-4.096V	+-2.048V	+-1.024V
3mV/div	2mV/div	1mV/div	1mV/div
1.0200	1.5120	-0.4920	-0.4920
1.0200	1.5120	-0.4920	-0.4920
1.0200	1.5120	-0.4920	-0.4920
1.0200	1.5120	-0.4920	-0.4920
1.0200	1.5120	-0.4920	-0.4920
1.0200	1.5120	-0.4920	-0.4920
1.0200	1.5120	-0.4920	-0.4920
1.0200	1.5120	-0.4920	-0.4920
1.0200	1.5120	-0.4920	-0.4920
1.0200	1.5120	-0.4920	-0.4920
1.0200	1.5120	-0.4920	-0.4920
1.0200	1.5120	-0.4920	-0.4920




as a double checking measure that column three and four
aren't just printing the same value because of some mistake, I've
rerun the same test but with a print statement inserted into the
voltage conversion function to show the raw ADC value.
Serial.print(adcVal);
Serial.print(',');


rightGain2/3	leftGain1	diff2-3GainX2	diff2-3GainX4
+-6.144V	+-4.096V	+-2.048V	+-1.024V
3mV/div	2mV/div	1mV/div	1mV/div
337,1.0110	749,1.4980	-487,-0.4870	-974,-0.4870
337,1.0110	749,1.4980	-487,-0.4870	-975,-0.4875
337,1.0110	749,1.4980	-488,-0.4880	-975,-0.4875
337,1.0110	749,1.4980	-487,-0.4870	-975,-0.4875
337,1.0110	749,1.4980	-488,-0.4880	-975,-0.4875
337,1.0110	750,1.5000	-488,-0.4880	-975,-0.4875
337,1.0110	750,1.5000	-488,-0.4880	-975,-0.4875
337,1.0110	750,1.5000	-488,-0.4880	-975,-0.4875
337,1.0110	750,1.5000	-488,-0.4880	-975,-0.4875
337,1.0110	750,1.5000	-488,-0.4880	-975,-0.4875
337,1.0110	750,1.5000	-488,-0.4880	-975,-0.4875
337,1.0110	750,1.5000	-488,-0.4880	-976,-0.4880
337,1.0110	750,1.5000	-488,-0.4880	-976,-0.4880



*/

#include <Wire.h>
#include <Adafruit_ADS1015.h>


Adafruit_ADS1015 adc11Bit;

const int potRightGain23 = 0,
     potLeftGain1= 1,
     diff_rightMinusLeftGainTWO = 1,
     diff_rightMinusLeftGainFOUR = 1;


void setup() {
    Serial.begin(9600);

    adc11Bit.begin();

    Serial.print("rightGain2/3\t");
    Serial.print("leftGain1\t");
    Serial.print("diff2-3GainX2\t");
    Serial.println("diff2-3GainX4");

    Serial.print("+-6.144V\t");
    Serial.print("+-4.096V\t");
    Serial.print("+-2.048V\t");
    Serial.println("+-1.024V");

    Serial.print("3mV/div\t");
    Serial.print("2mV/div\t");
    Serial.print("1mV/div\t");
    Serial.println("1mV/div");
}



void loop() {
    float voltage = 0.0;
    voltage = getVoltageWGain(potRightGain23, GAIN_TWOTHIRDS, 0);
    Serial.print(voltage, 4);
    Serial.print("\t");

    voltage = getVoltageWGain(potLeftGain1, GAIN_ONE, 0);
    Serial.print(voltage, 4);
    Serial.print("\t");

    voltage = getVoltageWGain(diff_rightMinusLeftGainTWO, GAIN_TWO, 1);
    Serial.print(voltage, 4);
    Serial.print("\t");

    voltage = getVoltageWGain(diff_rightMinusLeftGainFOUR, GAIN_FOUR, 1);
    Serial.print(voltage, 4);

    Serial.println();

    delay(500);
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
