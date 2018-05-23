/*
  Control a Servo connected to a Slave Arduino.
  
  On Slave:
  - Connect the servo line to pin 9. 
  - Connect Rx and Tx lines (pins 0 and 1) to the Tx and Rx lines of the Master.

  Also works with a BT Chip set in Master mode, connected to pins 0 and 1.

*/
#include <Servo.h>

Servo myservo;  // create servo object to control a servo


String inString = "";    // string to hold input
int value = 90;

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  delay(400);
  myservo.write(value);

  delay(15);
  // Open serial communications and wait for port to open:
  Serial.begin(38400);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // Read serial input:
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      value = inString.toInt();

      Serial.println(value);
      myservo.write(value);
      delay(15);

      // clear the string for new input:
      inString = "";
    }
  }

//  if (value == 0)
//    digitalWrite(LED_BUILTIN, LOW);
//  else if (value == 1)
//    digitalWrite(LED_BUILTIN, HIGH);
//  if (value >= 0 || value <= 180)


}
