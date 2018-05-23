/*
  Control a Servo connected to a Slave Arduino.
  On Master: 
  - Connect the Rx line (pin 10) to the Tx line of the Slave.
  - Connect the Tx line (pin 11) to the Rx line of the Slave.
  - Set the Serial Terminal to 38400 and "new line"

  Also works with a BT Chip set in Master mode, connected to pins 10 and 11.
  Send numbers between approximately 30 and 150.

*/

#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11); //Rx, Tx


void setup() {
  Serial.begin(38400);

  BTSerial.begin(38400);
}

void loop() {
  if (Serial.available()) {      // If anything comes in Serial (USB),
    
    BTSerial.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)
  }

}
