#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 adc15Bit;

void setup() {
    Serial.begin(9600);
    adc15Bit.begin();
}



void loop() {
    float volt = 0.0;
    volt = getVoltageWGain(0, GAIN_TWOTHIRDS, 0);
    Serial.print(volt, 4);
    Serial.print('\t');

    volt = getVoltageWGain(1, GAIN_TWOTHIRDS, 0);
    Serial.print(volt, 4);
    Serial.print('\t');

    volt = getVoltageWGain(0, GAIN_TWOTHIRDS, 1);
    Serial.print(volt, 4);
    Serial.print('\n');
    delay(100);
}



float getVoltageWGain(int channel, adsGain_t gain, bool single0_diff1){
    int16_t adcVal = 0;

    adc15Bit.setGain(gain);

    //read from the desired channel in single ended or differential
    //differential 0-1 is channel 0 and differential 2-3 is channel 1
    if(!single0_diff1){
        adcVal = adc15Bit.readADC_SingleEnded(channel);
    }
    else {
        if(channel % 2 == 0)adcVal = adc15Bit.readADC_Differential_0_1();
        else adcVal = adc15Bit.readADC_Differential_2_3();
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
