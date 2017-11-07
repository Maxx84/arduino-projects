#include <SoftwareSerial.h>
#include <Wire.h> //For I2C/SMBus


const int nbSensors = 2;
byte i2cAddress[] = {0x06, 0X08};
//byte i2cAddress = 0x06; // Slave address (SingleTact), default 0x04
//byte i2cAddress2 = 0x08; // Slave address (SingleTact), default 0x04
short threshold[nbSensors];
short baseline[nbSensors];


void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(500000);  // start serial for output
  Serial.flush();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  for(int i = 0; i<nbSensors; i++)
  {
    baseline[i] = 0;
    threshold[i] = 80;
  }

  Serial.println("Calibrating the sensor...");
  
  sensorInit(baseline, nbSensors);
  
  Serial.println("Done.");
  
  Serial.print("Baseline value = ");
  
  for(int i = 0; i<nbSensors; i++){
    Serial.println(baseline[i]);
  }
}


void loop()
{

  short data[nbSensors];
  
  for(int i = 0; i<nbSensors; i++) {
    data[i] = readDataFromSensor(i2cAddress[i]);
    data[i] *= 10;
  }

  //Serial.print("I2C Sensor Data:");
  Serial.print(-200);
  Serial.print(" ");
  Serial.print(800);
  Serial.print(" ");

  for(int i = 0; i<nbSensors; i++) {
    
    if ((data[i] - baseline[i]) > threshold[i])
    {
      Serial.print(400 + 200*i);
      threshold[i] = 30;
    }
    else
    {
      Serial.print(300 + 200*i);
      threshold[i] = 80;
    }

    Serial.print(" ");

    
    Serial.print(data[i] - baseline[i]);
    Serial.print(" ");
  }
  
  Serial.println();
  //Serial.print("\n");
  //delay(5); // Change this if you are getting values too quickly
}


short readDataFromSensor(short address)
{
  byte i2cPacketLength = 6;//i2c packet length. Just need 6 bytes from each slave
  byte outgoingI2CBuffer[3];//outgoing array buffer
  byte incomingI2CBuffer[6];//incoming array buffer

  outgoingI2CBuffer[0] = 0x01;//I2c read command
  outgoingI2CBuffer[1] = 128;//Slave data offset
  outgoingI2CBuffer[2] = i2cPacketLength;//require 6 bytes

  Wire.beginTransmission(address); // transmit to device
  Wire.write(outgoingI2CBuffer, 3);// send out command
  byte error = Wire.endTransmission(); // stop transmitting and check slave status
  if (error != 0) return -1; //if slave not exists or has error, return -1
  Wire.requestFrom(address, i2cPacketLength);//require 6 bytes from slave

  byte incomeCount = 0;
  while (incomeCount < i2cPacketLength)    // slave may send less than requested
  {
    if (Wire.available())
    {
      incomingI2CBuffer[incomeCount] = Wire.read(); // receive a byte as character
      incomeCount++;
    }
    else
    {
      delayMicroseconds(10); //Wait 10us
    }
  }

  short rawData = (incomingI2CBuffer[4] << 8) + incomingI2CBuffer[5]; //get the raw data

  return rawData;
}

void sensorInit(short average[], int _nbSensors) 
{
  int i;
  double sum[_nbSensors];
  for (i=0; i<_nbSensors; i++) {
    sum[i] = 0.0;
  }
  
  int steps = 0;
  
  double t0 = millis();

  while((millis() - t0) < 10000) //10 seconds
  {
    
    for(i=0; i<_nbSensors; i++)
    {
      sum[i] = sum[i] + readDataFromSensor(i2cAddress[i]);
    }
    
    steps = steps + 1;
  }

  for(i=0; i<_nbSensors; i++)
  {
    average[i] = (short) (10 * sum[i] / (double) steps);
  }
}
