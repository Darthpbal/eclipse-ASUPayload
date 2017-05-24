/*
The important thing to get down in this situation is how to control the value
refresh rate independently from the print rate without corrupting the sensor
calculated output value.
*/


/*
 *    FILE: demo01.pde
 *  AUTHOR: Rob Tillaart
 *    DATE: 2011 05 16
 *
 * PURPOSE: prototype TSL235R monitoring
 *
 * Digital Pin layout ARDUINO
 * =============================
 *  2     IRQ 0    - to TSL235R
 *
 * PIN 1 - GND
 * PIN 2 - VDD - 5V
 * PIN 3 - SIGNAL
 *
 */



/*Serial monitor capture of disconnected sensor, nominal room light, then high
brightness, then low brightness.


capture at 3.3V rail power
FREQ: 0   = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 9389   = 94 mW/m2
FREQ: 9353   = 94 mW/m2
FREQ: 9360   = 94 mW/m2
FREQ: 9423   = 94 mW/m2
FREQ: 9408   = 94 mW/m2
FREQ: 9407   = 94 mW/m2
FREQ: 9321   = 93 mW/m2
FREQ: 7629   = 76 mW/m2
FREQ: 8512   = 85 mW/m2
FREQ: 38026  = 380 mW/m2
FREQ: 27175  = 272 mW/m2
FREQ: 23946  = 239 mW/m2
FREQ: 2452   = 25 mW/m2
FREQ: 332  = 3 mW/m2
FREQ: 227  = 2 mW/m2
FREQ: 226  = 2 mW/m2
FREQ: 235  = 2 mW/m2
FREQ: 241  = 2 mW/m2
FREQ: 248  = 2 mW/m2




results at 5V rail power
FREQ: 0   = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 0  = 0 mW/m2
FREQ: 489  = 5 mW/m2
FREQ: 3392   = 34 mW/m2
FREQ: 4637   = 46 mW/m2
FREQ: 5288   = 53 mW/m2
FREQ: 5199   = 52 mW/m2
FREQ: 5523   = 55 mW/m2
FREQ: 4811   = 48 mW/m2
FREQ: 5824   = 58 mW/m2
FREQ: 26363  = 264 mW/m2
FREQ: 25580  = 256 mW/m2
FREQ: 23641  = 236 mW/m2
FREQ: 23642  = 236 mW/m2
FREQ: 21418  = 214 mW/m2
FREQ: 22279  = 223 mW/m2
FREQ: 16250  = 163 mW/m2
FREQ: 4020   = 40 mW/m2
FREQ: 263  = 3 mW/m2
FREQ: 107  = 1 mW/m2
FREQ: 96   = 1 mW/m2
FREQ: 101  = 1 mW/m2
FREQ: 132  = 1 mW/m2
FREQ: 135  = 1 mW/m2





FREQ: 7758	 = 78 mW/m2	....	1000
FREQ: 7799	 = 78 mW/m2	....	2000
FREQ: 7763	 = 78 mW/m2	....	3000
FREQ: 7666	 = 77 mW/m2	....	4000
FREQ: 7577	 = 76 mW/m2	....	5000
FREQ: 7596	 = 76 mW/m2	....	6000
FREQ: 7317	 = 73 mW/m2	ping	7000
FREQ: 7206	 = 72 mW/m2	ping	8000
FREQ: 7221	 = 72 mW/m2	ping	9000
FREQ: 7264	 = 73 mW/m2	....	10000
FREQ: 7462	 = 75 mW/m2	....	11000
FREQ: 7297	 = 73 mW/m2	....	12000
FREQ: 7287	 = 73 mW/m2	....	13000
FREQ: 7238	 = 72 mW/m2	....	14000
FREQ: 7180	 = 72 mW/m2	....	150

*/

volatile unsigned long livePulseCount = 0;
unsigned long oldPulseCount = 0;
unsigned long tempPulseCount = 0;
unsigned long lastSampleTime;

bool ledState = LOW;

void frequencyPulseCount()
{
  livePulseCount++;
}

///////////////////////////////////////////////////////////////////
//
// SETUP
//
void setup()
{
  Serial.begin(115200);
  Serial.println("START");
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
  attachInterrupt(0, frequencyPulseCount, RISING);
  pinMode(8, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, ledState);
}

///////////////////////////////////////////////////////////////////
//
// MAIN LOOP
//
void loop()
{
  if (millis() - lastSampleTime >= 1000)
  {
    lastSampleTime = millis();
    tempPulseCount = livePulseCount;
    unsigned long hz = tempPulseCount - oldPulseCount;
    Serial.print("FREQ: ");
    Serial.print(hz);
    Serial.print("\t = ");
    Serial.print((hz+50)/100);  // +50 == rounding last digit
    Serial.print(" mW/m2\t");
    oldPulseCount = tempPulseCount;

    if(digitalRead(8)) Serial.print("....\t");
    else Serial.print("ping\t");
    Serial.println(lastSampleTime);
    ledState = !ledState;
    digitalWrite(13, ledState);
  }
}
// END OF FILE
