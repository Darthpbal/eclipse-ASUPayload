#include <SoftwareSerial.h>
#include <Mercury.h>

SoftwareSerial venusSerialPort(2,3);
Mercury venus(&venusSerialPort);

void setup() {
    Serial.begin(9600);
    venus.begin(9600);

    Serial.println(F("\ntest begin\n\n"));

    byte byteRead = 0x00;
    //command to query the software version
    byte message[] = {0xA0, 0xA1, 0x00, 0x02, 0x03, 0x00, 0x03, 0X0D, 0X0A};
    // Respond with the software version which should look like this
    // 0xA0 0xA1 0x00 0x0E 0x80 0x01 0x00 0x01 0x01 0x01 0x00 0x01 0x03 0x0E 0x00 0x07 0x01 0x12 0x98 0x0D 0x0A

    Serial.println(F("sending command to query software"));
    Serial.println();
    for(int i = 0; i < 9; i++){
        Serial.print(message[i], HEX);
        Serial.print(' ');
    }
    Serial.println();

    Serial.println(F("listening for acknowledgement\n\n"));
    venusSerialPort.write(message, 9);


    do{
        byteRead = venusSerialPort.read();
    }
    while( byteRead != 0xA0 );
    Serial.print(F("A0 "));
    do{
        while(venusSerialPort.available() == 0);
        byteRead = venusSerialPort.read();
        Serial.print(byteRead, HEX);
        Serial.print(' ');
    }
    while(byteRead != 0x0A);


    Serial.println(F("\ntest finished\n\n"));
}



void loop() {
    delay(5000);
}
