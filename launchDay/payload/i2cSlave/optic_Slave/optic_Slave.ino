unsigned long tim = 0;
  unsigned int data0, data1;
  bool readCheck = false;
#include <SparkFunTSL2561.h>
#include <Wire.h>
const bool on = LOW, off = HIGH;
// Create an SFE_TSL2561 object, here called "light":

SFE_TSL2561 light;

// Global variables:

boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
int UVOUT = A0; //Output from the sensor
int REF_3V3 = A1;
void setup()
{
  // Initialize the Serial port:
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
  Serial.begin(9600);
  pinMode(8, INPUT);
  pinMode(13, OUTPUT);

  light.begin();

  // Get factory ID from sensor:
  // (Just for fun, you don't need to do this to operate the sensor)

  unsigned char ID;

//  if (ligh/

  // The light sensor has a default integration time of 402ms,
  // and a default gain of low (1X).

  // If you would like to change either of these, you can
  // do so using the setTiming() command.

  // If gain = false (0), device is set to low gain (1X)
  // If gain = high (1), device is set to high gain (16X)

  gain = 0;

  // If time = 0, integration will be 13.7ms
  // If time = 1, integration will be 101ms
  // If time = 2, integration will be 402ms
  // If time = 3, use manual start / stop to perform your own integration

  unsigned char time = 2;

  // setTiming() will set the third parameter (ms) to the
  // requested integration time in ms (this will be useful later):

//  Serial.println("Set timing...");
  light.setTiming(gain,time,ms);

  // To start taking measurements, power up the sensor:

//  Serial.println("Powerup...");
  light.setPowerUp();

  // The sensor will now gather light during the integration time.
  // After the specified time, you can retrieve the result from the sensor.
  // Once a measurement occurs, another integration period will start.
}

void loop()
{
  // Wait between measurements before retrieving the result
  // (You can also configure the sensor to issue an interrupt
  // when measurements are complete)

  // This sketch uses the TSL2561's built-in integration timer.
  // You can also perform your own manual integration timing
  // by setting "time" to 3 (manual) in setTiming(),
  // then performing a manualStart() and a manualStop() as in the below
  // commented statements:

  // ms = 1000;
  // light.manualStart();
//  delay(ms);
  // light.manualStop();

  // Once integration is complete, we'll retrieve the data.

  // There are two light sensors on the device, one for visible light
  // and one for infrared. Both sensors are needed for lux calculations.

  // Retrieve the data from the device:


int uvLevel = averageAnalogRead(UVOUT);
int refLevel = averageAnalogRead(REF_3V3);
//Use the 3.3V power pin as a reference to get a very accurate output value from sensor
float outputVoltage = 3.3 / refLevel * uvLevel;
float uvIntensity = mapfloat(outputVoltage, 0.99, 2.9, 0.0, 15.0);

if(millis() - tim > ms){
  readCheck = light.getData(data0,data1);
  tim = millis();
}


bool selState = digitalRead(8);
if(selState == on){
digitalWrite(13, HIGH);


  if (readCheck)
  {

    Serial.print(data0);
    Serial.print(",");
    Serial.print(data1);
    Serial.print(",");

    double lux;    // Resulting lux value
    boolean good;  // True if neither sensor is saturated

    good = light.getLux(gain,ms,data0,data1,lux);

    Serial.print(lux);
    Serial.print(",");
    if (good) Serial.print("good,"); else Serial.print("BAD,");
  }
  else
  {
    byte error = light.getError();
    printError(error);
  }


  Serial.print(uvIntensity);
  Serial.print(",");

  Serial.print('\n');


    while(!digitalRead(8));

    digitalWrite(13, LOW);
  }


}

void printError(byte error)
  // If there's an I2C error, this function will
  // print out an explanation.
{
  Serial.print("I2C error: ");
  Serial.print(error,DEC);
  Serial.print(", ");

  switch(error)
  {
    case 0:
      Serial.println("success");
      break;
    case 1:
      Serial.println("data too long for transmit buffer");
      break;
    case 2:
      Serial.println("received NACK on address (disconnected?)");
      break;
    case 3:
      Serial.println("received NACK on data");
      break;
    case 4:
      Serial.println("other error");
      break;
    default:
      Serial.println("unknown error");
  }
}







int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0;

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);
}

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
