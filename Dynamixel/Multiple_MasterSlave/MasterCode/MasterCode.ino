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

// Motor Id
const int dofNb = 5;
const int id[dofNb] = {1, 2, 3, 4, 5};

// Data serialization variables
const int NUMBER_OF_FIELDS = dofNb; // how many comma separated fields we expect

// position vector
int pos[NUMBER_OF_FIELDS];

String message;

void setup() {
  //Bluetooth serial initialization
  BTSerial.begin(38400);

  // Initialize pos
  for (int i = 0; i < dofNb; i++) {
    pos[i] = 512;
  }
  
  // Dynamixel serial initialization
  Dynamixel.begin(1000000,2);  // Initialize the servo at 1Mbps and Pin Control 2
  delay(500);

  // Homing of the master AX-12
  for (int i = 0; i < dofNb; i++) {
    Dynamixel.setTorqueEnable(id[i],1); // Enable torque on the Dynamixel motor
    delay(50);
    Dynamixel.move(id[i],pos[i]); // Home the AX-12 to the central position
    delay(100);
  
    // Disable torque on the motor. This allows to backdrive it.
    Dynamixel.setTorqueEnable(id[i],0); 
    delay(50);
  }
}

void loop() {

  for (int i = 0; i < dofNb; i++) {
    pos[i] = Dynamixel.readPosition(id[i]); // Read position of the master AX-12
  }

  delay(200);

  message = serialize(pos, dofNb);
  
  BTSerial.print(message); // Send position information to the slave AX-12
//  BTSerial.print("\n");
  //delay (20);
}


String serialize(int _arr[], int _size) {

  // The format is
  // a123,456, ... , 789\n
  String _str = "a";

  for (int i = 0; i < _size; i++) {

    // Put a ',' only after the first value.
    if (i > 0) {
      _str = String(_str + ",");
    }
    
    _str = String(_str + _arr[i]);
  }

  // Note: endline char here! Do not repeat
  _str = String(_str + "\n");

  return _str;
}
