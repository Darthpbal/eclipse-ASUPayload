

#include <Wire.h>  //I2C
#include <Adafruit_Sensor.h>  //unified sensor library



//start adafruit lux
// sensor cut for now because of conflict with altimeter
// #include <Adafruit_TSL2561_U.h>
// Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
//end adafruit lux




//start altimeter
#include <Adafruit_MPL3115A2.h>
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
//end altimeter



//start NXP Orientation
#include <Mahony.h>
#include <Madgwick.h>

#define ST_LSM303DLHC_L3GD20        (0)

#define ST_LSM9DS1                  (1)

#define NXP_FXOS8700_FXAS21002      (2)

#define AHRS_VARIANT   NXP_FXOS8700_FXAS21002

#if AHRS_VARIANT == ST_LSM303DLHC_L3GD20
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_LSM303_U.h>
#elif AHRS_VARIANT == ST_LSM9DS1

#elif AHRS_VARIANT == NXP_FXOS8700_FXAS21002
#include <Adafruit_FXAS21002C.h>
#include <Adafruit_FXOS8700.h>
#else
#error "AHRS_VARIANT undefined! Please select a target sensor combination!"
#endif

#if AHRS_VARIANT == ST_LSM303DLHC_L3GD20
Adafruit_L3GD20_Unified       gyro(20);
Adafruit_LSM303_Accel_Unified accel(30301);
Adafruit_LSM303_Mag_Unified   mag(30302);
#elif AHRS_VARIANT == ST_LSM9DS1

#elif AHRS_VARIANT == NXP_FXOS8700_FXAS21002
Adafruit_FXAS21002C gyro = Adafruit_FXAS21002C(0x0021002C);
Adafruit_FXOS8700 accelmag = Adafruit_FXOS8700(0x8700A, 0x8700B);
#endif

float mag_offsets[3]            = { 0.93F, -7.47F, -35.23F };

float mag_softiron_matrix[3][3] = { {  0.943,  0.011,  0.020 },
                                    {  0.022,  0.918, -0.008 },
                                    {  0.020, -0.008,  1.156 } };

float mag_field_strength        = 50.23F;

float gyro_zero_offsets[3]      = { 0.0F, 0.0F, 0.0F };

Mahony filter;

//end NXP Orientation



//start DHT22
#include "DHT.h"
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
//end DHT22


//start thermometer
#include <Adafruit_MAX31865.h>
Adafruit_MAX31865 max = Adafruit_MAX31865(10, 11, 12, 13);
#define RREF 430.0
//end thermometer


void setup() {
    Serial.begin(9600);
   Serial.println(F("\n\nStarting up"));

    max.begin(MAX31865_3WIRE);

    dht.begin();


    //start NXP orientation
    if(!gyro.begin())
    {
    /* There was a problem detecting the gyro ... check your connections */
        Serial.println("Ooops, no gyro detected ... Check your wiring!");
    }

    #if AHRS_VARIANT == NXP_FXOS8700_FXAS21002
    if(!accelmag.begin(ACCEL_RANGE_4G))
    {
        Serial.println("Ooops, no FXOS8700 detected ... Check your wiring!");
    }
    #else
    if (!accel.begin())
    {
        /* There was a problem detecting the accel ... check your connections */
        Serial.println("Ooops, no accel detected ... Check your wiring!");
    }

    if (!mag.begin())
    {
        /* There was a problem detecting the mag ... check your connections */
        Serial.println("Ooops, no mag detected ... Check your wiring!");
    }
    #endif

    // Filter expects 70 samples per second
    // Based on a Bluefruit M0 Feather ... rate should be adjuted for other MCUs
    filter.begin(10);
    //end NXP orientation




    //start altimeter
    if (! baro.begin()) {
        Serial.println("Couldnt find altimeter");
    }
    //end altimeter




    //start adafruit lux
    // if(!tsl.begin())
    // {
    //   /* There was a problem detecting the TSL2561 ... check your connections */
    //   Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    // }
    // configureSensor();
    // irAdaLux, visAdaLux, luxAdaLux,
    //end adafruit lux


   Serial.println(F("Startup finished"));
   Serial.println(F("thermocouple(F), humidity(%), tempDHT(F), heatIndxDHT(F), roll, pitch, yaw, accelX, accelY, accelZ, magX, magY, magZ, gyroX, gyroY, gyroZ, pressAltim(pascals), pressAltim(ATM), altitudeAltim(meters), tempAltim(F), millis\n"));
}



void loop() {

    //start thermometer
    float tempThermoCouple = ( (max.temperature(100, RREF) * 9) / 5) + 32;
    Serial.print( tempThermoCouple , 3 );
    Serial.print(", ");
    //end thermometer


    //start DHT22
    float h = dht.readHumidity();
    Serial.print(h, 3);
    Serial.print(F(", "));

    float f = dht.readTemperature(true);
    Serial.print(f, 3);
    Serial.print(F(", "));

    float hif = dht.computeHeatIndex(f, h);
    Serial.print(hif, 3);
    Serial.print(F(", "));
    //end DHT22


    //start NXP orientation
    sensors_event_t gyro_event;
    sensors_event_t accel_event;
    sensors_event_t mag_event;

    // Get new data samples
    gyro.getEvent(&gyro_event);
    #if AHRS_VARIANT == NXP_FXOS8700_FXAS21002
        accelmag.getEvent(&accel_event, &mag_event);
    #else
        accel.getEvent(&accel_event);
        mag.getEvent(&mag_event);
    #endif

    // Apply mag offset compensation (base values in uTesla)
    float x = mag_event.magnetic.x - mag_offsets[0];
    float y = mag_event.magnetic.y - mag_offsets[1];
    float z = mag_event.magnetic.z - mag_offsets[2];

    // Apply mag soft iron error compensation
    float mx = x * mag_softiron_matrix[0][0] + y * mag_softiron_matrix[0][1] + z * mag_softiron_matrix[0][2];
    float my = x * mag_softiron_matrix[1][0] + y * mag_softiron_matrix[1][1] + z * mag_softiron_matrix[1][2];
    float mz = x * mag_softiron_matrix[2][0] + y * mag_softiron_matrix[2][1] + z * mag_softiron_matrix[2][2];

    // Apply gyro zero-rate error compensation
    float gx = gyro_event.gyro.x + gyro_zero_offsets[0];
    float gy = gyro_event.gyro.y + gyro_zero_offsets[1];
    float gz = gyro_event.gyro.z + gyro_zero_offsets[2];

    // The filter library expects gyro data in degrees/s, but adafruit sensor
    // uses rad/s so we need to convert them first (or adapt the filter lib
    // where they are being converted)
    gx *= 57.2958F;
    gy *= 57.2958F;
    gz *= 57.2958F;

    float ax = accel_event.acceleration.x;
    float ay = accel_event.acceleration.y;
    float az = accel_event.acceleration.z;

    // Update the filter
    filter.update(gx, gy, gz, ax, ay, az, mx, my, mz);

    // Print the orientation filter output
    // Note: To avoid gimbal lock you should read quaternions not Euler
    // angles, but Euler angles are used here since they are easier to
    // understand looking at the raw values. See the ble fusion sketch for
    // and example of working with quaternion data.
    float roll = filter.getRoll();
    float pitch = filter.getPitch();
    float heading = filter.getYaw();

    Serial.print(roll);
    Serial.print(F(", "));
    Serial.print(pitch);
    Serial.print(F(", "));
    Serial.print(heading);
    Serial.print(F(", "));

    Serial.print(ax);
    Serial.print(F(", "));
    Serial.print(ay);
    Serial.print(F(", "));
    Serial.print(az);
    Serial.print(F(", "));

    Serial.print(mx);
    Serial.print(F(", "));
    Serial.print(my);
    Serial.print(F(", "));
    Serial.print(mz);
    Serial.print(F(", "));

    Serial.print(gx);
    Serial.print(F(", "));
    Serial.print(gy);
    Serial.print(F(", "));
    Serial.print(gz);
    Serial.print(F(", "));
    //end NXP orientation




    //start altimeter
    float pascals = baro.getPressure();
    Serial.print(pascals);
    Serial.print(F(", "));

    Serial.print(pascals/101325);
    Serial.print(F(", "));

    float altm = baro.getAltitude();
    Serial.print(altm);
    Serial.print(F(", "));

    float tempC = baro.getTemperature();
    Serial.print( ( (tempC * 9) / 5) + 32 );
    Serial.print(F(", "));

    //end altimeter



    //start adafruit lux
    // sensors_event_t event;
    // tsl.getEvent(&event);
    //
    // uint16_t visible, ir;
    // tsl.getLuminosity (visible, ir);
    //
    // Serial.print(ir);
    // Serial.print(F(", "));
    //
    // Serial.print(visible);
    // Serial.print(F(", "));
    //
    // /* Display the results (light is measured in lux) */
    // if (event.light)
    // {
    //   Serial.print(event.light);
    // }
    // else
    // {
    //   /* If event.light = 0 lux the sensor is probably saturated
    //      and no reliable data could be generated! */
    //   Serial.println("-1");//overload
    // }
    // Serial.print(F(", "));

    //end adafruit lux




   Serial.print(millis());
    Serial.println();
    delay(10);
}







// adafruit lux related functions
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */
  // Serial.println("------------------------------------");
  // Serial.print  ("Gain:         "); Serial.println("Auto");
  // Serial.print  ("Timing:       "); Serial.println("13 ms");
  // Serial.println("------------------------------------");
}
// end adafruit lux related functions
