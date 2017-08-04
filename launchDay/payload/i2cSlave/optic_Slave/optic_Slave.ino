#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

int cs = 8;




int UVOUT = A0; //Output from the sensor
int REF_3V3 = A1; //3.3V power on the Arduino board


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

void setup(void)
{
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    pinMode(cs, INPUT);
  Serial.begin(9600);

  /* Initialise the sensor */
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  /* Display some basic information on this sensor */
//  displaySensorDetails();

  /* Setup the sensor gain and integration time */
//  configureSensor();

  /* We're ready to go! */
  digitalWrite(13, LOW);
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void)
{

    int uvLevel = averageAnalogRead(UVOUT);
    int refLevel = averageAnalogRead(REF_3V3);

    //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
    float outputVoltage = 3.3 / refLevel * uvLevel;

    float uvIntensity = mapfloat(outputVoltage, 0.99, 2.9, 0.0, 15.0);




    /* Get a new sensor event */
    sensors_event_t event;
    tsl.getEvent(&event);








    if(digitalRead(cs)){
        digitalWrite(13, HIGH);

        /* Display the results (light is measured in lux) */
//        if (event.light) 
        Serial.print(event.light);
//        else Serial.print("Sensor overload");

        Serial.print(F(","));
        Serial.println(uvIntensity);



        while(digitalRead(cs)) Serial.println(digitalRead(cs));
        digitalWrite(13, LOW);
    }
}
