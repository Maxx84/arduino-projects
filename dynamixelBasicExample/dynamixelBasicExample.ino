#include "DynamixelSoftSerial.h"
#include "math.h"

#define PI 3.1415

// Serial communication, to command amplitude, offset, and frequency
const int NUMBER_OF_FIELDS = 3; // how many comma separated fields we expect
double values[NUMBER_OF_FIELDS];   // array holding values for all the fields
bool decimals = false;
int decimalDigit = 10.0;
int fieldIndex = 0;            // the current field being received
int sign = 1;

// Control multiple motors
const int dofNb = 1; // Number of Dynamixel motors connected
const int id[dofNb] = {10}; // IDs of the connected motors

double A_odd = 0; //deg
double A_even = 0; //deg
double freq = 1.0;
double lambda_odd = 9999;
double lambda_even = 9999;
double delta = PI / 2;

void setup() {

  Serial.begin(115200);
  //Serial.println("Dynamixel Basic Example");

  Dynamixel.begin(1000000, 2); // Initialize the servo at 1Mbps and Pin Control 2

  for (int i = 1; i <= dofNb; i++) {
    Dynamixel.setEndless(id[i], OFF);
  }

  delay(1000);
}

void loop() {
  if (Serial.available())
  {
    //rxData();
  }

  for (int i = 1; i <= dofNb; i++) {
    double position = 512 + 3.41 * 10 * sin(2 * PI * freq * millis() / 1000.0);


    if (i % 2 == 0) { // even
      position = 512 + 3.41 * A_even * sin((2 * PI * freq * millis() / 1000.0) + (i / lambda_even) + delta); // 3.41 = 512 counts / 150 deg
    }
    else { // Odd
      position = 512 + 3.41 * A_odd * sin((2 * PI * freq * millis() / 1000.0) + (i / lambda_odd));
    }

    Dynamixel.move(i, position);
  }

  //delay (20);
}


void rxData() {


  char ch_init = Serial.read();

  if (ch_init == 'a' || ch_init == 'A') { // Amplitude

    while (Serial.available())
    {
      char ch = Serial.read();

      if ((ch >= '0' && ch <= '9') && !decimals) // is this an ascii digit between 0 and 9?
      {
        // yes, accumulate the value
        values[fieldIndex] = (values[fieldIndex] * 10) + (ch - '0');
      }
      else if ((ch >= '0' && ch <= '9') && decimals) // This goes into the decimal digits
      {
        values[fieldIndex] = values[fieldIndex] + ((ch - '0') / ((double)decimalDigit));
        decimalDigit = decimalDigit * 10;
      }
      else if (ch == ',')  // comma is our separator, so move on to the next field
      {
        values[fieldIndex] = values[fieldIndex] * sign;
        sign = 1;
        decimals = false;
        decimalDigit = 10;
        if (fieldIndex < NUMBER_OF_FIELDS - 1)
          fieldIndex++;   // increment field index
      }
      else if (ch == '.') // Decimal point
      {
        decimals = true;
      }
      else if (ch == '-')
      {
        sign = -1;
      }
      else
      {
        // any character not a digit or comma ends the acquisition of fields
        values[fieldIndex] = values[fieldIndex] * sign;
        sign = 1;

        A_odd = values[0];
        A_even = values[1];

        if ((A_odd > 90) || (A_odd < -90) || (A_even > 90) || (A_even < -90)) {
          A_odd = 0;
          A_even = 0;
          Serial.println("Amplitude too large... take it easy!");
        }
        else {
          Serial.print("A_odd = ");
          Serial.print(A_odd);
          Serial.print(" A_even = ");
          Serial.println(A_even);
        }

        for (int i = 0; i < NUMBER_OF_FIELDS; i++) {
          values[i] = 0;
        }

        fieldIndex = 0;  // ready to start over
        decimals = false;
        decimalDigit = 10;
      }
    }
  } // /Amplitude
  else if (ch_init == 'f' || ch_init == 'F') { // Frequency

    Serial.println("in f");
    
    while (Serial.available())
    {
      char ch = Serial.read();

      if ((ch >= '0' && ch <= '9') && !decimals) // is this an ascii digit between 0 and 9?
      {
        // yes, accumulate the value
        values[0] = (values[0] * 10) + (ch - '0');
      }
      else if ((ch >= '0' && ch <= '9') && decimals) // This goes into the decimal digits
      {
        values[0] = values[0] + ((ch - '0') / ((double)decimalDigit));
        decimalDigit = decimalDigit * 10;
      }
      else if (ch == '.') // Decimal point
      {
        decimals = true;
      }
      else if (ch == '-')
      {
        sign = -1;
      }
      else
      {
        // any character not a digit or comma ends the acquisition of fields
        values[0] = values[0] * sign;
        sign = 1;

        freq = values[0];

        if ((freq > 4) || (freq < -4)) {
          freq = 0;
          Serial.println("Frequency too large... take it easy!");
        }
        else {
          Serial.print("Frequency = ");
          Serial.println(freq);
        }

        for (int i = 0; i < NUMBER_OF_FIELDS; i++) {
          values[i] = 0;
        }

        fieldIndex = 0;  // ready to start over
        decimals = false;
        decimalDigit = 10;
      }
    }
  } // /Frequency
  else if (ch_init == 's' || ch_init == 'S') { // side winding

    A_odd = 40; //deg
    A_even = 20; //deg
    freq = 0.5;
    lambda_odd = 1;
    lambda_even = 1;
    delta = PI / 4;

    while (Serial.available())
    {
      Serial.read(); // empty the buffer
    }
    Serial.println("Side winding gait");
  } // /side winding
  else if (ch_init == 'r' || ch_init == 'R') { // rolling


    A_odd = 20; //deg
    A_even = 20; //deg
    freq = 0.5;
    lambda_odd = 9999;
    lambda_even = 9999;
    delta = PI / 2;

    while (Serial.available())
    {
      Serial.read(); // empty the buffer
    }

    Serial.println("Rolling gait");
  } // /rolling
  else if (ch_init == 'l' || ch_init == 'L') { // linear progression


    A_odd = 20; //deg
    A_even = 20; //deg
    freq = 0.5;
    lambda_odd = 9999;
    lambda_even = 9999;
    delta = PI / 4;

    while (Serial.available())
    {
      Serial.read(); // empty the buffer
    }

    Serial.println("Rolling gait");
  } // /linear progression
  else if (ch_init == 'q' || ch_init == 'Q') { // stop

    A_odd = 0; //deg
    A_even = 0; //deg
    freq = 0;
    lambda_odd = 9999;
    lambda_even = 9999;
    delta = PI / 2;

    while (Serial.available())
    {
      Serial.read(); // empty the buffer
    }

    Serial.println("Stop");
  } // /stop

}
