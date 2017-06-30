/*
This file demonstrates basic printing of lines to the serial monitor.

By default the library filters nothing and parses nothing, acting as a SoftwareSerial
port monitor to just echo that data to the arduino ide serial monitor.
*/




#include <Mercury.h>             //include the library

#ifdef _VARIANT_ARDUINO_DUE_X_
    Mercury venus(&Serial1);         // declare an instance of the library, passing the software serial object to the Mercury constructor.
#else
    #include <SoftwareSerial.h>     //include SoftwareSerial
    SoftwareSerial venusSerial(10,11);  // setup pin 10 and 11 as rx and tx for a software serial port.
    Mercury venus(&venusSerial);         // declare an instance of the library, passing the software serial object to the Mercury constructor.
#endif


void setup() {
    Serial.begin(9600);     // set arduino to computer baud
    venus.begin(9600);      // set arduino to gps baud

    Serial.println(F("\nStartup finished."));      // let me know whats happening.
}


void loop() {
    char sentence[100] = "";    // make a variable to store the data in


    // This reads a line into the library, then stores it into the sentence
    // variable and then prints that sentence ten times. This is to show the
    //preferred method for grabbing the content from the library.
    for(int i = 0; i < 11; i++){

      venus.readLine();         // read data from the gps and store in the library
      venus.getLine(sentence);  // retreive that data from the library

      Serial.println(sentence); // print that data
                                // ???
                                // profit
    }



    // This method for using the data gathered from the gps. The other method
    // seems "safer" to me, but what nice about this method is you don't need to
    // make your own sentence variable like I do at the beginning of the loop. I
    // think this saves a little memory and clocks too if that's important to you.
    for(int i = 0; i < 11; i++){
      venus.readLine();     //read data from gps and store in the sentence variable inside the library

      Serial.println(venus.sentence);  //access the library sentence variable directly to print.
    }

}
