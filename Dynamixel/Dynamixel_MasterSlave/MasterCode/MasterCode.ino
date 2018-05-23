/*
  Control a Dynamixel AX-12 connected to a Slave Arduino by moving 
  a Dynamixel AX-12 connected to the master Arduino.
  
  On Master: 
  - Connect the Rx line (pin 10) to the Tx line of the Slave.
  - Connect the Tx line (pin 11) to the Rx line of the Slave.
  - Set the Serial Terminal to 38400 and "new line"
  - Connect the Dynamixel motor on Serial0.

  Also works with a BT Chip set in Master mode, connected to pins 10 and 11.

*/

#include <SoftwareSerial.h>

#include "DynamixelSerial.h" // Modified from the online library. 
// Added function setTorqueEnable(id, value).
// Changed value of TX_DELAY_TIME from 400 to 50

SoftwareSerial BTSerial(10, 11); //Rx, Tx

int pos = 512;

void setup() {
  //Bluetooth serial initialization
  BTSerial.begin(38400);

  // Dynamixel serial initialization
  Dynamixel.begin(1000000,2);  // Initialize the servo at 1Mbps and Pin Control 2
  delay(500);

  // Homing of the master AX-12
  Dynamixel.setTorqueEnable(2,1); // Enable torque on the Dynamixel motor
  delay(500);
  Dynamixel.move(2,pos); // Home the AX-12 to the central position
  delay(500);

  // Disable torque on the motor. This allows to backdrive it.
  Dynamixel.setTorqueEnable(2,0); 
  delay(500);
}

void loop() {

  pos = Dynamixel.readPosition(2); // Read position of the master AX-12
  delay(50);
  
  BTSerial.print(pos); // Send position information to the slave AX-12
  BTSerial.print("\n");
}
