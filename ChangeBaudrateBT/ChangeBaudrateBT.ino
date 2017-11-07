/*

AUTHOR: Hazim Bitar (techbitar)
DATE: Aug 29, 2013
LICENSE: Public domain (use at your own risk)
CONTACT: techbitar at gmail dot com (techbitar.com)

http://www.instructables.com/id/Modify-The-HC-05-Bluetooth-Module-Defaults-Using-A/?ALLSTEPS


Wire the HC-05 and Arduino Uno per instructions.
BEFORE YOU CONNECT THE ARDUINO TO THE USB remove the VCC (power) red wire from the HC-05 so it's not getting any power from the Arduino. All other wires are still connected.
Now connect the Arduino Uno to the USB cable extended from your PC.
Make sure the HC-05 module is NOT PAIRED with any other Bluetooth device.
Re-connect the Arduino Uno 5V wire to the HC-05's VCC (5V power) pin.
The HC-05 LED will blink on and off at about 2 second intervals. Now the HC-05 is in AT command mode ready to accept commands to change configuration and settings.
To test if everything is wired correctly,  open the Serial Monitor from the Arduino IDE and type "AT" and click SEND. You should see an "OK"
If you don't see an "OK" check your wiring.

Commands (Enable NL&CR):

To return HC-05 to mfg. default settings:                                             AT+ORGL
To get version of your HC-05 enter:                                                   AT+VERSION?
To change device name from the default HC-05 to let's say MYBLUE enter:               AT+NAME=MYBLUE
To change default security code from 1234 to 2987 enter:                              AT+PSWD=2987
To change HC-05 baud rate from default 9600 to 115200, 1 stop bit, 0 parity enter:    AT+UART=115200,1,0

*/


#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11); // RX | TX

void setup()
{
  pinMode(9, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(9, HIGH);
  Serial.begin(9600);
  Serial.println("Enter AT commands:");
  BTSerial.begin(38400);  // HC-05 default speed in AT command more
}

void loop()
{

  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available())
    Serial.write(BTSerial.read());

  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTSerial.write(Serial.read());
}
