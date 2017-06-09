#include <SoftwareSerial.h>
#include "Hermes.h"


SoftwareSerial venusSerialPort(2,3); // rx, tx. Setup the venus SoftwareSerial port
Hermes venus(&venusSerialPort);


void setup() {
    Serial.begin(9600);
    venus.begin(9600);

    // turn on filtered mode.
    // This makes the read function
    // continuously read data until
    // the desired tag is matched
    venus.setRunMode(filtered);

    Serial.println(F("\nStartup finished"));
}



void loop() {
    char gpsData[] = "";




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
        Serial.println();
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
