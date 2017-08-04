
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>

// Power by connecting Vin to 3-5V, GND to GND
// Uses I2C - connect SCL to the SCL pin, SDA to SDA pin
// See the Wire tutorial for pinouts for each Arduino
// http://arduino.cc/en/reference/wire
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
const bool on = LOW, off = HIGH;
void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT);
  pinMode(13, OUTPUT);
}

void loop() {

  if (! baro.begin()) {
    Serial.println("Couldnt find sensor");
    return;
  }

  float pascals = baro.getPressure();
  float altm = baro.getAltitude();
  float tempC = baro.getTemperature();


  bool selState = digitalRead(8);
  if(selState == on){
    digitalWrite(13, HIGH);

    Serial.print(pascals); Serial.print(",");
    Serial.print(altm); Serial.print(",");
    Serial.print( tempC ); Serial.print(",");
    Serial.print('\n');

    while(!digitalRead(8));

    digitalWrite(13, LOW);
  }

}





/*

looks like test results are good.

The sensor says that the temp readout is 72.72
degrees F and I have a thermometer in my bedroom
where I'm testing this sensor and it also reads 72
degrees. While I can't confirm the altitude yet
without running alongside the GPS, but a quick
conversion from pascals to atm almost hits 1.0,
which means it's almost right on.



97494.00 pascals  0.96 atm  324.06 meters 72.72 *F
97491.75 pascals  0.96 atm  323.94 meters 72.72 *F
97496.00 pascals  0.96 atm  323.81 meters 72.72 *F
97494.50 pascals  0.96 atm  323.81 meters 72.84 *F
97493.50 pascals  0.96 atm  324.00 meters 72.72 *F
97495.25 pascals  0.96 atm  324.00 meters 72.72 *F
97494.75 pascals  0.96 atm  324.13 meters 72.72 *F
97494.25 pascals  0.96 atm  324.19 meters 72.72 *F
97495.75 pascals  0.96 atm  324.00 meters 72.84 *F
97492.75 pascals  0.96 atm  323.81 meters 72.84 *F
97492.75 pascals  0.96 atm  324.06 meters 72.84 *F
97492.75 pascals  0.96 atm  323.94 meters 72.84 *F
97496.25 pascals  0.96 atm  324.00 meters 72.84 *F









arduino due test results 7/3/17

47109.50 pascals  0.46 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F
47483.25 pascals  0.47 atm  11777.37 meters 93.20 *F




arduino due original code test results 7/3/17

47109.50 pascals
13.95 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C

47483.25 pascals
14.06 Inches (Hg)
11777.37 meters
34.00*C






test on UNO 7/3/17


97085.50 pascals  0.96 atm  358.94 meters 72.39 *F
97084.75 pascals  0.96 atm  359.06 meters 72.39 *F
97086.50 pascals  0.96 atm  359.13 meters 72.39 *F
97086.25 pascals  0.96 atm  359.13 meters 72.39 *F
97087.25 pascals  0.96 atm  359.13 meters 72.39 *F
97087.50 pascals  0.96 atm  359.38 meters 72.39 *F
97086.00 pascals  0.96 atm  359.25 meters 72.39 *F
97082.50 pascals  0.96 atm  358.94 meters 72.39 *F
97085.25 pascals  0.96 atm  358.94 meters 72.39 *F
97085.00 pascals  0.96 atm  359.06 meters 72.39 *F
97086.25 pascals  0.96 atm  359.31 meters 72.39 *F
97087.25 pascals  0.96 atm  359.19 meters 72.39 *F
97082.00 pascals  0.96 atm  359.13 meters 72.27 *F


1434.75 pascals
0.42 Inches (Hg)
358.88 meters
22.62*C

97092.75 pascals
28.75 Inches (Hg)
358.50 meters
22.69*C

97088.75 pascals
28.75 Inches (Hg)
358.63 meters
22.69*C

97091.25 pascals
28.75 Inches (Hg)
358.63 meters
22.69*C

97092.75 pascals
28.75 Inches (Hg)
358.63 meters
22.62*C

97088.50 pascals
28.75 Inches (Hg)
358.81 meters
22.62*C

97090.00 pascals
28.75 Inches (Hg)
358.94 meters
22.62*C

97092.25 pascals
28.75 Inches (Hg)
358.69 meters
22.62*C

97091.25 pascals
28.75 Inches (Hg)
358.94 meters
22.62*C

*/
