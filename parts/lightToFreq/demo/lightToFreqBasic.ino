
/*

This file is for the basic test operation of the light
intensity to frequency converter sensor using the pulsin functions


test results:
0's represent a period when the pin was disconnected.
, after this, the low numbers represent bright values
and large numbers appear to represent dark values.
device is alive.
Now we just need the proper conversion for a lum/lux value.

0
0
0
0
0
0
0
0
131
134
112
77
64
55
66
38
34
120
159
192
236
261
300
353
391
409
26
25
24
25
26
27
28

*/


int light2FreqPin = 3;

void setup() {
    Serial.begin(9600);
    pinMode(light2FreqPin, INPUT);
}



void loop() {
    unsigned long lightPulse = pulseIn(light2FreqPin, HIGH);
    Serial.println(lightPulse);
    Serial.println();
    delay(500);
}
