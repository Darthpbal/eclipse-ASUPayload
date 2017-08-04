#include <Adafruit_MPL3115A2.h>
int cs = 8;
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();


float tempC;
float altm;
float pascals;

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void setup() {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    Serial.begin(9600); //turn on serial monitor
    baro.begin();   //initialize pressure sensor mySensor
    pinMode(cs, INPUT);
    digitalWrite(13, LOW);
}

void loop() {
    float pascals = baro.getPressure();
    float altm = baro.getAltitude();
    float tempC = baro.getTemperature();



    if(digitalRead(cs)){
        digitalWrite(13, HIGH);

        Serial.print(tempC);
        Serial.print(F(","));
        Serial.print(pascals);
        Serial.print(F(","));
        Serial.println(altm);

        while(digitalRead(cs));
        digitalWrite(13, LOW);
    }



}
