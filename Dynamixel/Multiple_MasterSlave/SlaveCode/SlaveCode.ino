/*
  Control a Dynamixel AX-12 connected to a Slave Arduino by moving
  a Dynamixel AX-12 connected to the master Arduino.

  On Slave:
  - Connect the AX-12 to the Serial0 line
  - Connect Rx and Tx lines (pins 10 and 11) to the Tx and Rx lines of the Master.

  - Dynamixel AX-12 Id = 1.

*/

#include <SoftwareSerial.h>
#include "DynamixelSerial.h" // Modified from the online library. Added function setTorqueEnable(id, value).

SoftwareSerial BTSerial(10, 11); //Rx, Tx

// String inString = "";    // string to hold input

// Motor Id
const int dofNb = 5;
const int id[dofNb] = {1, 2, 3, 4, 5};

// Data serialization variables
const int NUMBER_OF_FIELDS = dofNb; // how many comma separated fields we expect
int values[NUMBER_OF_FIELDS];   // temporary array holding values for all the fields
int fieldIndex = 0;            // the current field being received
bool dynamixelErr = 0;

// position vector
int pos[NUMBER_OF_FIELDS];


void setup() {
  // Open serial communications
  BTSerial.begin(38400);

  // Initialize pos and values
  for (int i = 0; i < dofNb; i++) {
    pos[i] = 512;
    values[i] = 0;
  }

  // Built in LED used to show error state
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Dynamixel serial initialization
  Dynamixel.begin(1000000, 2); // Initialize the servo at 1Mbps and Pin Control 2
  delay(100);


  // Homing of the slave AX-12
  for (int i = 0; i < dofNb; i++) {
    Dynamixel.setTorqueEnable(id[i], 1); // Enable torque on the Dynamixel motor
    delay(50);
    Dynamixel.move(id[i], pos[i]); // Home the AX-12 to the central position
    delay(100);
  }

}

void loop() {
  if (BTSerial.available())
  {
    deserialize(); // Receive and de-serialize new data
  }

  for (int i = 0; i < NUMBER_OF_FIELDS; i++) {
    // Sanity check
    if (pos[i] > -1 || pos[i] < 1024) {
      // Move the slave to the new position
      Dynamixel.move(id[i], pos[i]);
    }
  }


}


void deserialize() {

  bool receivingData = true;

  char ch_init = BTSerial.read();

  if (ch_init == 'a') { // We follow the format a123,456, ... , 789\n

    while (receivingData) {

      if (BTSerial.peek() != -1) // be sure that the next char is available
      {
        char ch = BTSerial.read();

        if ((ch >= '0' && ch <= '9')) // is this an ascii digit between 0 and 9?
        {
          // yes, accumulate the value
          values[fieldIndex] = (values[fieldIndex] * 10) + (ch - '0');
        }
        else if (ch == ',')  // comma is our separator, so move on to the next field
        {
          if (fieldIndex < NUMBER_OF_FIELDS - 1)
            fieldIndex++;   // increment field index
        }
        else if (ch == '-')
        {
          // When the dynamixel library fails to read the current state, it returns a -1.
          // We can use the '-' as a possible sign of error.
          dynamixelErr = 1;
          digitalWrite(LED_BUILTIN, HIGH);
          receivingData = false;

          while (BTSerial.available() > 0) {
            char garbage = BTSerial.read();
          }
          
          return;

        }
        else
        {
          receivingData = false;

          // any character not a digit or comma ends the acquisition of fields

          for (int i = 0; i < NUMBER_OF_FIELDS; i++) {
            pos[i] = values[i];
          }

          // Clear data inside temp array "values"
          for (int i = 0; i < NUMBER_OF_FIELDS; i++) {
            values[i] = 0;
          }

          fieldIndex = 0;  // ready to start over
        }
      }
    }
  }

  // Clear data inside temp array "values"
  for (int i = 0; i < NUMBER_OF_FIELDS; i++) {
    values[i] = 0;
  }
}

