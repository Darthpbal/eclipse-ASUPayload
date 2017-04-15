/*
This demo runs a stripped down version of the sparkfun library example, but sampling the
millis function before and after the main data aquisition library functions to
determine how fast those functions are, to see if it would be an issue using
within an interrupt.


This specific test simply samples millis before the data gathering parts and
then right after and prints the difference. Then it prints the corresponding lux
value, then a newline.

The testing environment is covering the sensor with my hand and flashing LEDs on
them to cause a high change in luminosity to see if that affects the data access
time.



Here is a capture of the serial monitor while running this program, it shows an
average access time around 1 - 2 milliseconds
1
6.23

2
6.11

1
6.35

1
6.15

1
6.35

1
6.15

1
6.35

2
6.23

2
5.61

1
5.36

1
6.44

1
6.19

1
4.65

1
1.75

2
1.75

1
1.75

1
1.67

1
4.94

1
4.82

1
3.04

2
2.79

2
2.79

1
2.79

1
3.00

1
3.00

1
3.00

1
3.20

2
5.15

1
5.23

1
1.17

1
1.00

1
1.88

1
2.79

2
3.00

1
3.00

1
3.04

2
1.26

1
0.51

1
1.34

2
2.91

2
2.67

1
1.96

1
6.19

1
6.19

1
6.43

1
6.64

2
3.66

1
4.07

1
7.46

1
8.88

1
9.01

1
9.10

2
9.26

2
9.59

1
21.95

1
86.41

2
467.81

1
455.23

2
387.75

2
276.64

1
29.40

1
10.48

2
24.49

2
141.02

1
15.39

1
68.37

2
27.66

1
80.42

2
83.17

1
11.94

1
12.80

2
62.33

1
156.25

2
160.45

1
167.38

2
35.98

1
11.68

2
8.08

1
5.67

1
5.30

2
5.34

1
5.14

1
5.01

1
5.05

1
5.05


*/


#include <SparkFunTSL2561.h>
#include <Wire.h>

SFE_TSL2561 light;

boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds

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
}




void loop(){
  delay(ms);
  unsigned int chanl0, chanl1;
  double lux, luxMain;    // Resulting lux value
  boolean good;  // True if neither sensor is saturated

  unsigned int startTime;
  startTime = millis();
  if (light.getData(chanl0,chanl1))
  {
    good = light.getLux(gain,ms,chanl0,chanl1,lux);

    Serial.println(millis() - startTime);
  }
  else
  {
    byte error = light.getError();
    printError(error);
  }
  Serial.println(lux);
  Serial.println();
}







// If there's an I2C error, this function will
// print out an explanation.
void printError(byte error){
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
