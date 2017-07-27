//determines how printing is handled for launch debug and graphing contexts
enum configuration { launch, debug, plot };
const configuration mode = debug;
char delim;  //the seperator that will be printed to seperate all values.



#include <Wire.h>               //I2C class


// 9DOF
#include <Adafruit_Sensor.h>
#include "Mahony.h"
#include "Madgwick.h"
#include "Adafruit_FXAS21002C.h"
#include "Adafruit_FXOS8700.h"

Adafruit_FXAS21002C gyro = Adafruit_FXAS21002C(0x0021002C);
Adafruit_FXOS8700 accelmag = Adafruit_FXOS8700(0x8700A, 0x8700B);


float mag_offsets[3]            = { 0.93F, -7.47F, -35.23F };
float mag_softiron_matrix[3][3] = { {  0.943,  0.011,  0.020 },
                                    {  0.022,  0.918, -0.008 },
                                    {  0.020, -0.008,  1.156 } };
float mag_field_strength        = 50.23F;
float gyro_zero_offsets[3]      = { 0.0F, 0.0F, 0.0F };
Mahony filter;
// end 9DOF


//altimeter
#include "Adafruit_MPL3115A2.h"    // altimeter
Adafruit_MPL3115A2 altimeter = Adafruit_MPL3115A2();
//end altimeter




void setup() {
    Serial.begin(9600);
    while (!Serial);  // Wait for serial port to connect (ATmega32U4 type PCBAs)

    delay(100);
    Serial.println(("Starting up..."));
    Serial.print(F("mode = "));
    if(mode == launch)  Serial.println(F("launch"));
    else if(mode == debug)  Serial.println(F("\ndebug"));
    else if(mode == plot)   Serial.println(F("\nplot"));



    //set delimiter type
    if(mode == plot) delim = '\t';
    else delim = ',';

    // 9DOF
    if(!gyro.begin())
    {
      /* There was a problem detecting the gyro ... check your connections */
      Serial.println("Ooops, no gyro detected ... Check your wiring!");
      if(mode == debug) while(1);
    }
    if(!accelmag.begin(ACCEL_RANGE_4G))
    {
      Serial.println("Ooops, no FXOS8700 detected ... Check your wiring!");
      if(mode == debug) while(1);
    }
    // Filter expects 70 samples per second
    // Based on a Bluefruit M0 Feather ... rate should be adjuted for other MCUs
    filter.begin(10);
    // end 9DOF





    String header = ""; //create header variable

    header += "pascals(absPres),";
    header += "atmospheres(absPres),";
    header += "altitudeMeters(absPres),";
    header += "tempC(absPres),";

    header += "accelXG(9DOF),";
    header += "accelYG(9DOF),";
    header += "accelZG(9DOF),";
    header += "gyroXdegPerSec(9DOF),";
    header += "gyroYdegPerSec(9DOF),";
    header += "gyroZdegPerSec(9DOF),";
    header += "magXuTesla(9DOF),";
    header += "magYuTesla(9DOF),";
    header += "magZuTesla(9DOF),";
    header += "roll(9DOF),";
    header += "pitch(9DOF),";
    header += "heading(9DOF),";


    header += "millis\n";
    if(mode  == debug) Serial.print(header);
}



void loop() {
    String logLine = "";        //create a sentence string for logging


    //altimeter
    if (!altimeter.begin()) {
        Serial.println("Couldnt find altimeter");
        return;
    }
    float pascals = altimeter.getPressure();
    logLine += pascals;
    logLine += delim;

    logLine += pascals / 101325;
    logLine += delim;

    logLine = altimeter.getAltitude();
    logLine += delim;

    // logLine = altimeter.getTemperature(); // in celcius
    logLine = ( ( 9 * altimeter.getTemperature() ) / 5) + 32; // in fahrenheight
    logLine += delim;
    //end altimeter

    logLine += '\n';

    // 9DOF
    sensors_event_t gyro_event;
    sensors_event_t accel_event;
    sensors_event_t mag_event;

    gyro.getEvent(&gyro_event);
    accelmag.getEvent(&accel_event, &mag_event);

    float x = mag_event.magnetic.x - mag_offsets[0];
    float y = mag_event.magnetic.y - mag_offsets[1];
    float z = mag_event.magnetic.z - mag_offsets[2];
    float mx = x * mag_softiron_matrix[0][0] + y * mag_softiron_matrix[0][1] + z * mag_softiron_matrix[0][2];
    float my = x * mag_softiron_matrix[1][0] + y * mag_softiron_matrix[1][1] + z * mag_softiron_matrix[1][2];
    float mz = x * mag_softiron_matrix[2][0] + y * mag_softiron_matrix[2][1] + z * mag_softiron_matrix[2][2];
    float gx = gyro_event.gyro.x + gyro_zero_offsets[0];
    float gy = gyro_event.gyro.y + gyro_zero_offsets[1];
    float gz = gyro_event.gyro.z + gyro_zero_offsets[2];
    gx *= 57.2958F;
    gy *= 57.2958F;
    gz *= 57.2958F;

    float ax = accel_event.acceleration.x;
    float ay = accel_event.acceleration.y;
    float az = accel_event.acceleration.z;

    filter.update(gx, gy, gz,
                  ax, ay, az,
                  mx, my, mz);

//    logLine += ax;
//    logLine += delim;
//    logLine += ay;
//    logLine += delim;
//    logLine += az;
//    logLine += delim;
//
//    logLine += gx;
//    logLine += delim;
//    logLine += gy;
//    logLine += delim;
//    logLine += gz;
//    logLine += delim;
//
//    logLine += mx;
//    logLine += delim;
//    logLine += my;
//    logLine += delim;
//    logLine += mz;
//    logLine += delim;

    logLine += filter.getRoll();
    logLine += delim;
    logLine += filter.getPitch();
    logLine += delim;
    logLine += filter.getYaw();
    logLine += delim;
    // end 9DOF

    if(mode != plot){
        logLine += millis();    // timestamp
    }


    logLine += '\n';    // last character should be a newline
    logLine += '\n';
    Serial.print(logLine);
    // if(mode == debug) Serial.print(logLine);
    // else if(mode == plot) Serial.print(logLine);
    // else if(mode == launch) lineLogger(logLine);

//    delay(20);     //for sanity

}
