/* 
 * Project: L07_00I2CScanner
 * Description: code along exercise to create Inter-integrated Circuit (I2C)
 * Author: Rebecca Snyder
 * Date: 2023-10-24
 * ````
 * 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 *  must install librarie "IoTClassroom_CNM" with ctrl-shift-p
 * */

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

#include <Wire.h>

byte i;
byte count;

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial to be available
  delay(3000); // Wait for Serial Monitor
  Serial.printf("opening I2C communication \n");
  Wire.begin();
  Serial.printf("Beginning I2C Scan \n");
}

void loop() {
  count = 0;
  for(i=0;i<=127;i++){
    Wire.beginTransmission(i); //opens communication line to device "i"
    if(Wire.endTransmission()==0){ //closes communicaiton line and get back a number between 0 and 4. 0 means successfull. Not 0 is not successful
      Serial.printf("Found address: %i (0x%02X) \n",i,i); //X means print number as Hex. 02 accounts for number of places. "Found address: 42 (0x2A)"
      count++;
      delay(1);
    }
  }
  Serial.printf("Done. Found %i device(s). \n",count);
}
