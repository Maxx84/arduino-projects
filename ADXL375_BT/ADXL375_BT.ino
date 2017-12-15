// Arduino interface for the ADXL375 accelerometer (+- 200g)
// Data from the accelerometer are read via I2C and stored on a SD card connected via SPI.
// Start and stop acquisition commands are sent via Bluetooth using a HC-05 BT Module.

#include "I2Cdev.h"
#include <Wire.h>
#include "SdFat.h"
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "HKIG_ADXL375.h"

//////////////////////////////////
//      Logging control via BT  //
//////////////////////////////////
bool startLog = false; // Start command received via BT
bool loggingData = false; // Used to manage the loop. When !startLog, we want to save the current data.

HKIG_ADXL375 shock;
//////////////////////////////////
//      File system object      //
//////////////////////////////////
SdFat sd;
SdFile dataFile;
#define DEVICE_ID 0x53
#define FILE_BASE_NAME "A_Data"                       // Base for the filenames created, an ID will follow after this
#define MEASUREMENT_TIME 20000                        // Time the program will run before it turns off
#define FLUSH_INTERVAL 200                            // Time before the data is flushed on the SD card, can cause corruption on unflushed data of power breaks during measurements
char fileName[13] = FILE_BASE_NAME "00.dat";

//////////////////////////////////////////
//   Error messages stored in flash.    //
//////////////////////////////////////////
#define error(msg) sd.errorHalt(F(msg))

//////////////////////////////////////////
//          Other definitions           //
//////////////////////////////////////////
long int prev_flush = 0;
int lines = 0;
long unsigned start_time;

//////////////////////////////////////////
//          Struct for data             //
//////////////////////////////////////////
struct dataStore {
  unsigned long timestamp;
  int16_t accX;
  int16_t accY;
  int16_t accZ;
} sensorData;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);                       // LED is lit until start of measurements
  digitalWrite(LED_BUILTIN, HIGH);                    // LED turns off when SPI is initialized

  Wire.begin();
  //Wire.setClock(400000L);                           // Faster I2C clock if needed

  Serial.begin(115200); // Change to the buad rate of the BT chip.
  while (!Serial);                                    // Wait until finished

  Serial.println(F("Initializing I2C devices..."));
  Serial.println(F("Testing device connections..."));

  if (mpu_initialize()) {
    Serial.println(F("MPU Initialized!"));
  }
  else {
    Serial.println(F("Something went wrong with the MPU Initialization!"));
    while (1);
    {
      digitalWrite(LED_BUILTIN, HIGH);          // If ADXL375 is not found, the program stops, and the led blinks
      delay(1000);                              // Useful if no terminal is connectec
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
    }
  }

  delay(500);

  Serial.println(F("Initializing SD card..."));         // Verify connection

  int chipSelect = 4;

  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {        // If failed to connect to SD storage, LED lights up
    sd.initErrorHalt();
    SPI.end;
    digitalWrite(LED_BUILTIN, HIGH);                  // If failed to connect to SD storage, LED lights up
  }

  Serial.println(F("Press 's' to start and 'q' to stop logging."));
}


void loop() {

  if (Serial.available())
  {
    rxBTData();
  }

  if (startLog && !loggingData) {
    loggingData = true;

    filecreate();                                       // Create file and write header

    Serial.println(F("SD Initialized, measurements begin"));

    start_time = millis();
  }

  if (loggingData) {
    sensors_event_t event;                              // Creates a struct "event" from adafruids sensor library
    //shock.getEvent(&event);
    shock.getEvent2(&event);                            // Gets data from ADXL375
    sensorData.timestamp = micros();                    // Collects used data in another strucs for easier storage
    sensorData.accX = event.acceleration.x; //(LSB = 49mg)
    sensorData.accY = event.acceleration.y;
    sensorData.accZ = event.acceleration.z;
    dataFile.write((const int16_t *)&sensorData, sizeof(sensorData));   // Writes to file
    lines++;                                            // Number of times loop is run, only used when setup is connected to terminal

    if (millis() - prev_flush >= FLUSH_INTERVAL)       // flushes/syncs data every FLUSH_INTERVAL
    {
      prev_flush = millis();
      dataFile.sync();
    }

    //if (millis() - start_time >= MEASUREMENT_TIME)      // Finished program after MEASUREMENT_TIME
    if (!startLog)      // Stop log command received
    {
      dataFile.close();
      Serial.print("File finished, "); Serial.print(lines); Serial.print(" number of measurements in "); Serial.print((millis() - start_time) / 1000); Serial.println(" seconds.");
      delay(20);
      SPI.end();
      digitalWrite(LED_BUILTIN, HIGH);                  // Lights LED when measurements are finished

      loggingData = false;
    }
  }
}



void filecreate() {
  Serial.print("Creating file...");
  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  // Find an unused file name.
  if (BASE_NAME_SIZE > 6) {
    error("FILE_BASE_NAME too long");
  }
  while (sd.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    }
    else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    }
    else {
      error("Can't create file name");
    }
  }
  if (!dataFile.open(fileName, O_CREAT | O_WRITE)) {
    error("file.open");
  }
  Serial.print(fileName); Serial.println(" created");
}


bool mpu_initialize()
{
  /* Initialise the sensor */
  Serial.println("Accelerometer Test...");
  if (!shock.begin())
  {
    /* There was a problem detecting the ADXL375 ... check your connections */
    Serial.println("Ooops, no ADXL375 detected ... Check your wiring!");
    return false;
  }
  /* Set the range to whatever is appropriate for your project */
  else
  {
    /* Set datarate and data format to a appropriate values, also calibration of offset */
    shock.setDataRate(ADXL375_DATARATE_800_HZ);                   // Datarate
    shock.writeRegister(ADXL375_REG_DATA_FORMAT, 0b1011);         // Data format
    shock.setOffsetX(-3);
    shock.setOffsetY(-2);
    shock.setOffsetZ(7);
    Serial.print("Connected"); Serial.println("");
    return true;
  }
}


// Bluetooth (more delays, sometimes the chars take more time to arrive!
void rxBTData() {

  char ch_init = Serial.read();
  Serial.println(ch_init);

  if (ch_init == 's')
  {
    startLog = true;

    while (Serial.available())
      Serial.read();

    Serial.println(F("Start logging command received."));
  }
  else if (ch_init == 'q')
  {
    //quit program
    startLog = false;

    while (Serial.available())
      Serial.read();

    Serial.println(F("Stop logging command received"));
  }
  else {
    Serial.println(F("Unknown command"));
  }
}

