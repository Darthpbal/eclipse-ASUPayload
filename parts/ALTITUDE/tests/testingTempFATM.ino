

#include <Wire.h>
#include <Adafruit_MPL3115A2.h>

// Power by connecting Vin to 3-5V, GND to GND
// Uses I2C - connect SCL to the SCL pin, SDA to SDA pin
// See the Wire tutorial for pinouts for each Arduino
// http://arduino.cc/en/reference/wire
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (! baro.begin()) {
    Serial.println("Couldnt find sensor");
    return;
  }

  float pascals = baro.getPressure();
  // Our weather page presents pressure in Inches (Hg)
  // Use http://www.onlineconversion.com/pressure.htm for other units
  Serial.print(pascals); Serial.print(" pascals\t");

  Serial.print(pascals/101325); Serial.print(" atm\t");

  float altm = baro.getAltitude();
  Serial.print(altm); Serial.print(" meters\t");

  float tempC = baro.getTemperature();
  Serial.print( ( (tempC * 9) / 5) + 32 ); Serial.println(" *F");

  delay(250);
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







*/
