
//example output from desk lamp on and off state with hand waving after

// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 34 data1: 2 lux: 15.90 (good)
// data0: 69 data1: 5 lux: 31.83 (good)
// data0: 52 data1: 4 lux: 23.87 (good)
// data0: 41 data1: 3 lux: 18.90 (good)
// data0: 32 data1: 2 lux: 14.91 (good)
// data0: 30 data1: 2 lux: 13.92 (good)
// data0: 28 data1: 2 lux: 12.93 (good)
// data0: 27 data1: 2 lux: 12.43 (good)
// data0: 27 data1: 2 lux: 12.43 (good)
// data0: 26 data1: 2 lux: 11.94 (good)
// data0: 26 data1: 2 lux: 11.94 (good)
// data0: 25 data1: 2 lux: 11.44 (good)
// data0: 25 data1: 2 lux: 11.44 (good)
// data0: 25 data1: 2 lux: 11.44 (good)
// data0: 25 data1: 2 lux: 11.44 (good)
// data0: 25 data1: 2 lux: 11.44 (good)
// data0: 24 data1: 2 lux: 10.94 (good)
// data0: 24 data1: 2 lux: 10.94 (good)
// data0: 11 data1: 1 lux: 4.97 (good)
// data0: 9 data1: 1 lux: 3.97 (good)
// data0: 7 data1: 1 lux: 2.95 (good)
// data0: 14 data1: 1 lux: 6.46 (good)
// data0: 14 data1: 1 lux: 6.46 (good)



// another example from only light from my laptop monitor and covering the sensor
// data0: 0 data1: 0 lux: 0.00 (good)
// data0: 0 data1: 0 lux: 0.00 (good)
// data0: 0 data1: 0 lux: 0.00 (good)
// data0: 0 data1: 0 lux: 0.00 (good)
// data0: 0 data1: 0 lux: 0.00 (good)
// data0: 0 data1: 0 lux: 0.00 (good)
// data0: 0 data1: 0 lux: 0.00 (good)
// data0: 0 data1: 0 lux: 0.00 (good)
// data0: 0 data1: 0 lux: 0.00 (good)
// data0: 0 data1: 0 lux: 0.00 (good)
// data0: 0 data1: 0 lux: 0.00 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)
// data0: 1 data1: 0 lux: 0.49 (good)




/*testing while at asu noble library covering and uncovering the sensor
question: This sensor is a combo visible and UV sensor, and I believe
that is what the two "data" values are. The question is which one is which?
Maybe data0 is visible (because higher value) and data1 is UV?
data0: 1588 data1: 270 lux: 640.55 (good)
data0: 1589 data1: 270 lux: 641.07 (good)
data0: 1587 data1: 270 lux: 640.03 (good)
data0: 1587 data1: 270 lux: 640.03 (good)
data0: 634 data1: 115 lux: 250.75 (good)
data0: 98 data1: 25 lux: 33.31 (good)
data0: 69 data1: 17 lux: 23.93 (good)
data0: 54 data1: 14 lux: 18.17 (good)
data0: 48 data1: 12 lux: 16.51 (good)
data0: 45 data1: 11 lux: 15.68 (good)
data0: 44 data1: 11 lux: 15.13 (good)
data0: 43 data1: 11 lux: 14.59 (good)
data0: 721 data1: 127 lux: 287.79 (good)
data0: 1587 data1: 269 lux: 640.72 (good)
data0: 1594 data1: 271 lux: 642.99 (good)
data0: 1590 data1: 271 lux: 640.91 (good)
data0: 1583 data1: 270 lux: 637.95 (good)
data0: 1584 data1: 270 lux: 638.47 (good)
data0: 1582 data1: 269 lux: 638.12 (good)
data0: 1584 data1: 270 lux: 638.47 (good)
data0: 1471 data1: 248 lux: 594.80 (good)
data0: 247 data1: 50 lux: 93.96 (good)
data0: 65 data1: 16 lux: 22.56 (good)
data0: 52 data1: 13 lux: 17.89 (good)
data0: 54 data1: 13 lux: 18.97 (good)
data0: 53 data1: 13 lux: 18.43 (good)
data0: 53 data1: 13 lux: 18.43 (good)
data0: 53 data1: 13 lux: 18.43 (good)
data0: 50 data1: 12 lux: 17.59 (good)
data0: 958 data1: 165 lux: 384.98 (good)
data0: 1596 data1: 271 lux: 644.03 (good)
data0: 1595 data1: 271 lux: 643.51 (good)
*/











#include <SparkFunTSL2561.h>
#include <Wire.h>


SFE_TSL2561 light;


boolean gain;
unsigned int ms;

void setup()
{

  Serial.begin(9600);

  light.begin();

  gain = 0;

  // If time = 0, integration will be 13.7ms
  // If time = 1, integration will be 101ms
  // If time = 2, integration will be 402ms
  // If time = 3, use manual start / stop to perform your own integration
  unsigned char time = 2;

  // setTiming() will set the third parameter (ms) to the
  // requested integration time in ms (this will be useful later):

  light.setTiming(gain,time,ms);

  // To start taking measurements, power up the sensor:

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
  delay(ms);
  // light.manualStop();

  // Once integration is complete, we'll retrieve the data.

  // There are two light sensors on the device, one for visible light
  // and one for infrared. Both sensors are needed for lux calculations.

  // Retrieve the data from the device:

  unsigned int data0, data1;

  if (light.getData(data0,data1))
  {
    // getData() returned true, communication was successful

    Serial.print("data0: ");
    Serial.print(data0);
    Serial.print(" data1: ");
    Serial.print(data1);

    // To calculate lux, pass all your settings and readings
    // to the getLux() function.

    // The getLux() function will return 1 if the calculation
    // was successful, or 0 if one or both of the sensors was
    // saturated (too much light). If this happens, you can
    // reduce the integration time and/or gain.
    // For more information see the hookup guide at: https://learn.sparkfun.com/tutorials/getting-started-with-the-tsl2561-luminosity-sensor

    double lux;    // Resulting lux value
    boolean good;  // True if neither sensor is saturated

    // Perform lux calculation:

    good = light.getLux(gain,ms,data0,data1,lux);

    // Print out the results:

    Serial.print(" lux: ");
    Serial.print(lux);
    if (good) Serial.println(" (good)"); else Serial.println(" (BAD)");
  }
  else
  {
    // getData() returned false because of an I2C error, inform the user.

    byte error = light.getError();
    printError(error);
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
