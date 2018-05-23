/*
  Control a Dynamixel AX-12 connected to a Slave Arduino by moving 
  a Dynamixel AX-12 connected to the master Arduino.
  
  On Slave:
  - Connect the AX-12 to the Serial0 line
  - Connect Rx and Tx lines (pins 10 and 11) to the Tx and Rx lines of the Master.

  - Dynamixel AX-12 Id = 1.

*/

#include <Servo.h>
#include <SoftwareSerial.h>
#include "DynamixelSerial.h" // Modified from the online library. Added function setTorqueEnable(id, value).

Servo myservo;  // create servo object to control a servo
SoftwareSerial BTSerial(10, 11); //Rx, Tx

String inString = "";    // string to hold input
//int value = 90;

int pos = 512;

void setup() {
  // Open serial communications
  BTSerial.begin(38400);

  // Dynamixel serial initialization
  Dynamixel.begin(1000000,2);  // Initialize the servo at 1Mbps and Pin Control 2
  delay(100);



//  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
//  delay(400);
//  myservo.write(value);
//
//  delay(15);
  
  // Homing of the slave AX-12
  Dynamixel.setTorqueEnable(1,1); // Enable torque on the Dynamixel motor
  delay(100);
  Dynamixel.move(1,pos); // Home the AX-12 to the central position
  delay(500);

}

void loop() {
  
  // Read serial input:
  while (BTSerial.available() > 0) {
    int inChar = BTSerial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      pos = inString.toInt();
//
//      myservo.write(value);
//      delay(15);

      // Sanity check. Expected values between 0 and 1023
      if (pos > -1 || pos < 1024) {
        // Move the slave to the new position
        Dynamixel.move(1,pos);
      }

      // clear the string for new input:
      inString = "";
    }
  }

}
