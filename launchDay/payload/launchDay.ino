/*
ASU Eclipse Ballooning Project Launch Code for August 2017 Total Solar Eclipse



Last known compilation stats : (date) *last additions
Sketch uses 0 bytes (0%) of program storage space. Maximum is X bytes.
Global variables use 0 bytes (0%) of dynamic memory, leaving X bytes for local variables. Maximum is X bytes.



HEADER DEFINITION:
example: "column a, column b, column c"

column a         = definition and meaning of header with units
column b         = definition and meaning of header with units
column c         = definition and meaning of header with units



**visually friendly GPS header difinition:
(GPS data is logged by cropping the GPGGA tag off of the GPS sentence and logging)
*credit - http://www.gpsinformation.org/dale/nmea.htm#GGA
GGA - essential fix data which provide 3D location and accuracy data.
$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
Where:
     GGA          Global Positioning System Fix Data
     123519       Fix taken at 12:35:19 UTC
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     1            Fix quality: 0 = invalid
                               1 = GPS fix (SPS)
                               2 = DGPS fix
                               3 = PPS fix
            			       4 = Real Time Kinematic
            			       5 = Float RTK
                               6 = estimated (dead reckoning) (2.3 feature)
            			       7 = Manual input mode
            			       8 = Simulation mode
     08           Number of satellites being tracked
     0.9          Horizontal dilution of position
     545.4,M      Altitude, Meters, above mean sea level
     46.9,M       Height of geoid (mean sea level) above WGS84
                      ellipsoid
     (empty field) time in seconds since last DGPS update
     (empty field) DGPS station ID number
     *47          the checksum data, always begins with *


*/


////////////////////////////////////////////////////////////////////////////////
//               Start of including headers
////////////////////////////////////////////////////////////////////////////////


#include <Wire.h>               //I2C class
#include <SoftwareSerial.h>     //Software serial


////////////////////////////////////////////////////////////////////////////////
//                End of including headers
////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////
//   Declare globals like pin numbers or constants/volatile variables here.
////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
//                          End of global variables
////////////////////////////////////////////////////////////////////////////////








////////////////////////////////////////////////////////////////////////////////
//                          Start of setup
////////////////////////////////////////////////////////////////////////////////
void setup() {

}//end of setup

////////////////////////////////////////////////////////////////////////////////
//                          End of setup
////////////////////////////////////////////////////////////////////////////////














////////////////////////////////////////////////////////////////////////////////
//                          Start of loop
////////////////////////////////////////////////////////////////////////////////
void loop() {

}// end of loop

////////////////////////////////////////////////////////////////////////////////
//                     End of function definitions
////////////////////////////////////////////////////////////////////////////////
