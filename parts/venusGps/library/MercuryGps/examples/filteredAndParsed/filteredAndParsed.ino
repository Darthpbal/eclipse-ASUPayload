#include <Mercury.h>             //include the library

#ifdef _VARIANT_ARDUINO_DUE_X_
    Mercury venus(&Serial1);         // declare an instance of the library, passing the software serial object to the Mercury constructor.
#else
    #include <SoftwareSerial.h>     //include SoftwareSerial
    SoftwareSerial venusSerial(10,11);  // setup pin 10 and 11 as rx and tx for a software serial port.
    Mercury venus(&venusSerial);         // declare an instance of the library, passing the software serial object to the Mercury constructor.
#endif



void setup() {
    Serial.begin(9600);
    venus.begin(9600);

    // turn on filtered mode.
    // This makes the read function
    // continuously read data until
    // the desired tag is matched

    Serial.println(F("\nStartup finished"));
}



void loop() {
    char gpsData[100] = "";

    venus.setRunMode(filtered);


    Serial.println(F("Filter GPRMC"));
    venus.setGpsTag("GPRMC");       // set the filter
    for(int i = 0; i < 11; i++){
        venus.readLine();           // read line into library
        venus.getLine(gpsData);    // retreive data from library
        Serial.println(gpsData);   // print data
    }
    Serial.println(F("\n"));    // seperate groups of filters with newline




    Serial.println(F("Filter GPGGA"));
    venus.setGpsTag("GPGGA");
    for(int i = 0; i < 11; i++){
        venus.readLine();
        venus.getLine(gpsData);
        Serial.println(gpsData);
    }
    Serial.println(F("\n"));    // seperate groups of filters with newline




    // to find out which field is which, check out this page. Fields are zero indexed.
    // http://www.gpsinformation.org/dale/nmea.htm#nmea
    // getField simply reads the gpsData that was gathered in readline
    Serial.println(F("parse 2d with time"));
    venus.setGpsTag("GPRMC");
    Serial.println(F("time, lat, n/s, long, e/w"));
    for(int i = 0; i < 11; i++){
        venus.readLine();

        // time
        venus.getField(gpsData, 1);
        Serial.print(gpsData);
        Serial.print(F(", "));
        //latitude
        venus.getField(gpsData, 3);
        Serial.print(gpsData);
        Serial.print(F(", "));
        // north/south
        venus.getField(gpsData, 4);
        Serial.print(gpsData);
        Serial.print(F(", "));
        // longitude
        venus.getField(gpsData, 5);
        Serial.print(gpsData);
        Serial.print(F(", "));
        // eastwest
        venus.getField(gpsData, 6);
        Serial.print(gpsData);
        Serial.println();   // end with a newline
    }
    Serial.println(F("\n"));    // seperate groups of filters with newline




    Serial.println(F("parse 3d with time"));
    venus.setGpsTag("GPGGA");
    Serial.println(F("time, lat, n/s, long, e/w, alt"));
    for(int i = 0; i < 11; i++){
        venus.readLine();

        // time
        venus.getField(gpsData, 1);
        Serial.print(gpsData);
        Serial.print(F(", "));
        // latitude
        venus.getField(gpsData, 2);
        Serial.print(gpsData);
        Serial.print(F(", "));
        // north/south
        venus.getField(gpsData, 3);
        Serial.print(gpsData);
        Serial.print(F(", "));
        // longitude
        venus.getField(gpsData, 4);
        Serial.print(gpsData);
        Serial.print(F(", "));
        // east/west
        venus.getField(gpsData, 5);
        Serial.print(gpsData);
        Serial.print(F(", "));
        // altitude
        venus.getField(gpsData, 9);
        Serial.print(gpsData);
        Serial.println();
    }
    Serial.println(F("\n"));    // seperate groups of filters with newline





    Serial.println(F("raw"));
    venus.setRunMode(raw);       // set the filter
    for(int i = 0; i < 11; i++){
        venus.readLine();           // read line into library
        venus.getLine(gpsData);    // retreive data from library
        Serial.println(gpsData);   // print data
    }
    Serial.println(F("\n"));    // seperate groups of filters with newline




    // easily see when the demo is over.
    Serial.println(F("demo over"));
    Serial.println(F("\n"));
    Serial.println(F("\n"));
}












/*
resulting output of this program


Startup finished
Filter GPRMC
$GPRMC,115949.000,V,0000.0000,N,00000.0000,E,000.0,000.0,280606,,,N*7D
$GPRMC,115950.000,V,0000.0000,N,00000.0000,E,000.0,000.0,280606,,,N*75
$GPRMC,115951.000,V,0000.0000,N,00000.0000,E,000.0,000.0,280606,,,N*74
$GPRMC,115952.000,V,0000.0000,N,00000.0000,E,000.0,000.0,280606,,,N*77
$GPRMC,115953.000,V,0000.0000,N,00000.0000,E,000.0,000.0,280606,,,N*76
$GPRMC,110551.000,V,0000.0000,N,00000.0000,E,000.0,000.0,090617,,,N*7E
$GPRMC,110552.000,V,0000.0000,N,00000.0000,E,000.0,000.0,090617,,,N*7D
$GPRMC,110553.000,V,0000.0000,N,00000.0000,E,000.0,000.0,090617,,,N*7C
$GPRMC,110554.000,V,0000.0000,N,00000.0000,E,000.0,000.0,090617,,,N*7B
$GPRMC,110555.000,V,0000.0000,N,00000.0000,E,000.0,000.0,090617,,,N*7A
$GPRMC,110556.000,V,0000.0000,N,00000.0000,E,000.0,000.0,090617,,,N*79


Filter GPGGA
$GPGGA,110557.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*6A
$GPGGA,110558.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*65
$GPGGA,110559.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*64
$GPGGA,110600.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*6B
$GPGGA,110601.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*6A
$GPGGA,110602.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*69
$GPGGA,110603.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*68
$GPGGA,110604.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*6F
$GPGGA,110605.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*6E
$GPGGA,110606.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*6D
$GPGGA,110607.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*6C


parse 2d with time
time, lat, n/s, long, e/w
110607.000, 0000.0000, N, 00000.0000, E
110608.000, 0000.0000, N, 00000.0000, E
110609.000, 0000.0000, N, 00000.0000, E
110610.000, 0000.0000, N, 00000.0000, E
110611.000, 0000.0000, N, 00000.0000, E
110612.000, 0000.0000, N, 00000.0000, E
110613.000, 0000.0000, N, 00000.0000, E
110614.000, 0000.0000, N, 00000.0000, E
110615.000, 0000.0000, N, 00000.0000, E
110616.000, 0000.0000, N, 00000.0000, E
110617.000, 0000.0000, N, 00000.0000, E


parse 3d with time
time, lat, n/s, long, e/w, alt
110618.000, 0000.0000, N, 00000.0000, E, 0.0
110619.000, 0000.0000, N, 00000.0000, E, 0.0
110620.000, 0000.0000, N, 00000.0000, E, 0.0
110620.000, 0000.0000, N, 00000.0000, E, 0.0
110621.000, 0000.0000, N, 00000.0000, E, 0.0
110622.000, 0000.0000, N, 00000.0000, E, 0.0
110623.000, 0000.0000, N, 00000.0000, E, 0.0
110624.000, 0000.0000, N, 00000.0000, E, 0.0
110625.000, 0000.0000, N, 00000.0000, E, 0.0
110626.000, 0000.0000, N, 00000.0000, E, 0.0
110627.000, 0000.0000, N, 00000.0000, E, 0.0



raw
$GPGSA,A,1,,,,,,,,,,,,,0.0,0.0,0.0*30
$GPGSV,1,1,03,23,00,000,44,01,00,000,43,03,00,000,41*4B
$GPRMC,115949.000,V,0000.0000,N,00000.0000,E,000.0,000.0,280606,,,N*7D
$GPVTG,000.0,T,,M,000.0,N,000.0,K,N*02
$GPGGA,115950.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*64
$GPGSA,A,1,,,,,,,,,,,,,0.0,0.0,0.0*30
$GPGSV,1,1,04,23,00,000,44,01,00,000,43,03,00,000,41,31,00,000,00*7E
$GPRMC,115950.000,V,0000.0000,N,00000.0000,E,000.0,000.0,280606,,,N*75
$GPVTG,000.0,T,,M,000.0,N,000.0,K,N*02
$GPGGA,115951.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,0.0,M,,0000*65
$GPGSA,A,1,,,,,,,,,,,,,0.0,0.0,0.0*30
$GPGSV,1,1,04,23,00,000,45,01,00,000,43,03,00,000,41,31,00,000,38*74




demo over



*/
