// Decoder Breakout Board library for Particle Photon 2
// PCB 472-1 v0.3 cdint.com

// keep the first and second version components synced with the board version
#define PRM_VERSION "0.3.1"

#include <Wire.h>
#include "LS7866_Registers.h"     // Include LS7866 Register Def's for I2C Counter chip

// XXX application
// ADDR_JUMPERS is the address set by the jumpers on the board
#define ADDR_JUMPERS 0b111
// CHIP_ADDR is the I2C address of the LS7866
// XXX this should be calculated in ChipSetup in the library rather than being a constant
#define CHIP_ADDR LS7866_I2C_FIXED_ADDR + ADDR_JUMPERS

// XXX application
#define CNTR_SIZE 4   // number of bytes to configure counters

// XXX application
#define POLL_INTERVAL 1000

// XXX application
char msgBuff[64];

// XXX ???
int LFlag = 0;
int LFlagCntr = 0;
unsigned long mstrClock = 400000l;
unsigned long nextPollMillis = 0;


// XXX library
/* 
 * Function LS7866_Read
 * Desc     Overloaded function for reading unsigned long value from LS7866 Registers
 * Input    slaveAddress has I2C slave address
 * Input    regAddr has address of LS7866 register to read from
 * Input    numBytes has number of bytes to read
 * Output   pbValue has assembled register value
 */
void LS7866_Read(byte slaveAddress, byte regAddr, unsigned long *pbValue, byte numBytes){
  unsigned long value = 0;
  byte * valPtr = (byte *)&value + numBytes-1;  // configure byte pointer to MSB position 
  
  // Write to Slave and set Register Addr we need to read from
  Wire.beginTransmission(slaveAddress);   // transmit to device
  Wire.write(regAddr);                    // Set Addr of Data we want
  Wire.endTransmission(0);                // Send Transmission do not send Stop
  
  // Send read Request to Slave
  Wire.requestFrom((int)slaveAddress, (int)numBytes, true); // Request from Slave NumBytes and terminate with Stop
  // Read data out of Wire Buffer
  // Data is read from device in Msb to Lsb format
  while(Wire.available()) *valPtr-- = (byte)Wire.read();    // load memory with data
  
  // Passback Value
  *pbValue = value;
}

/* 
 * Function LS7866_Read
 * Desc     Overloaded function for reading 8 bit values from LS7866 registers
 * Input    slaveAddress has I2C slave address
 * Input    regAddr has address of LS7866 register to read from
 * Output   pbValue has register value
 */
void LS7866_Read(byte slaveAddress, byte regAddr, byte *pbValue){
  
  // Write to Slave and set Register Addr we need to read from
  Wire.beginTransmission(slaveAddress);   // transmit to device
  Wire.write(regAddr);                    // Set Addr of Data we want to read
  Wire.endTransmission(0);                // Send Transmission do not send Stop
  
  // Send Request to Slave
  Wire.requestFrom((int)slaveAddress, 1, true); // Request from Slave NumBytes and terminate with Stop
  // Read data out of Wire Buffer
  // Data is read from device in Msb to Lsb format
  while(Wire.available()) *pbValue = (byte)Wire.read();    // receive byte
  
}

/* 
 * Function LS7866_Write
 * Desc     Overloaded function for writing unsigned long value to LS7866 Registers like IDR0 and IDR1.
 * Input    slaveAddress has I2C slave address
 * Input    regAddr has address of LS7866 register to read from
 * Input    value has data to write
 * Input    numBytes has number of bytes to write
 * Output   Value has register value
 */

int LS7866_Write(byte slaveAddress, byte regAddr, unsigned long value, byte numBytes){
  int error = 0;
  byte * valPtr = (byte*)&value + (numBytes - 1);
  Wire.beginTransmission(slaveAddress); // transmit to device
  Wire.write(regAddr);                  // Load Addr we want to write to
  
  // Load Data MSB to LSB int Wire Buffer
  for (byte i = 0; i<numBytes; i++) Wire.write(*valPtr--);  

  error = Wire.endTransmission();       // Send transmitting
  return error;
}

/* 
 * Function LS7866_Write
 * Desc     Overloaded function for writing byte value to LS7866 Registers like MCR0, MCR1, FCR and TPR
 * Input    slaveAddress has I2C slave address for LS7866 Counter
 * Input    regAddr has address of LS7866 register to read from
 * Input    value has Data to write
 */

int LS7866_Write(byte slaveAddress, byte regAddr, byte value){
  int error = 0;
  Wire.beginTransmission(slaveAddress); // transmit to device
  Wire.write(regAddr);                  // Load Addr we want to write to
  Wire.write(value);                    // Load Data to send
  error = Wire.endTransmission();       // Send transmitting
  return error;
}




/* 
 * Function CounterSetup2
 * Desc     Configures counter
 * Input    cntrId identifies each counter on pcb
 * Input    slaveAddress has I2C slave address for LS7866 Counter
 * Input    cntrSize has number of bytes IDRx, CNTR, ODR are configured.
 * Output   none
 */

void ChipSetup(byte chipAddress, byte cntrSize){
    byte DataSet[] = {  MCR0_QUAD_X1 + MCR0_FREE_RUN + MCR0_DISABLE_Z + MCR0_Z_EDGE_FE,                // MCR0
                        MCR1_CNTR_4BYTE + MCR1_COUNT_ENABLE + MCR1_nCLR_DSTR_ON_RD + MCR1_SSTR_ON_RD,   // MCR1                                                                    // MCR1 filled in below
                        FCR_IDX ,                                                                    // FCR
                        0x00, 0x00, 0x08, 0x00,                                                     // IDR0
                        0x00, 0x00, 0x00, 0xFF,                                                     // IDR1
                        TPR_RDST};                                                                  // TPR
    byte numBytes = sizeof(DataSet)/sizeof(byte);

    // Set MCR1 Value in DataSet
    byte mcr1CntrSize = MCR1_CNTR_4BYTE;
    switch(cntrSize){
      case 1: mcr1CntrSize =  MCR1_CNTR_1BYTE; break;
      case 2: mcr1CntrSize =  MCR1_CNTR_2BYTE; break;
      case 3: mcr1CntrSize =  MCR1_CNTR_3BYTE; break;
    }
    DataSet[1] =  mcr1CntrSize + MCR1_COUNT_ENABLE + MCR1_nCLR_DSTR_ON_RD + MCR1_SSTR_ON_RD;

    // reset registers IDR0, IDR1, CNTR, ODR, DSTR, SSTR and LFLAG/ to default values
    LS7866_Write(chipAddress, TPR_ADDR, TPR_MRST);  

    // Write DataSet
    Wire.beginTransmission(chipAddress);                       // transmit to device
    Wire.write(MCR0_ADDR | LS7866_REG_AUTO_INC);                // Add Auto Inc Bit to base Register Value
    for (byte i = 0; i<numBytes; i++) Wire.write(DataSet[i]);   // Load Wire buffer
    Wire.endTransmission();                                     // Send transmitting

    sprintf(msgBuff, "Setup Counter %d at Addr:%02x Bytes:%d MCR0:%02x MCR1:%02x\r\n", ADDR_JUMPERS, chipAddress, numBytes, DataSet[0], DataSet[1]);
    Serial.print(msgBuff);

    // read back MCR0 as a test
    byte mcr0;
    LS7866_Read(chipAddress, MCR0_ADDR, &mcr0);
    sprintf(msgBuff, "MCR0 test: %02x\r\n", mcr0);  
    Serial.print(msgBuff);

}

// XXX library
int CounterCheck(byte CntrId, byte DevAddr, byte cntrSize){
  int error = 0;
  unsigned long CntrVal = 0;
  unsigned long ValLong = 0;
  byte sstrVal = 0;
  byte sstrVal2 = 0;
   
  LS7866_Read(DevAddr, ODR_ADDR, &CntrVal, cntrSize);  // Read ODR which load SSTR
  LS7866_Read(DevAddr, SSTR_ADDR, &sstrVal);            // Read SSTR
  
  if (sstrVal & (SSTR_EQL0 + SSTR_BW)){            // Check if this counter has Flags Set
        LS7866_Write(DevAddr, TPR_ADDR, TPR_RDST);       // Reset DSTR  
        sprintf(msgBuff, "LFlagCntr:%d Cntr:%d was %08lx SSTR:%02x BW:%d EQU0:%d\n",LFlagCntr, CntrId, CntrVal, sstrVal, (sstrVal & SSTR_BW), (sstrVal & SSTR_EQL0));
        Serial.print(msgBuff);
  }
   
  return error;
}

// XXX library
int CounterPoll(byte DevAddr, byte cntrSize){
  int error = 0;
  unsigned long cntrVal = 0;
  unsigned long ValLong = 0;
  byte sstrVal = 0;
  // setLed(LED_GRN);

  LS7866_Read(DevAddr, CNTR_ADDR, &cntrVal, cntrSize);
  LS7866_Read(DevAddr, SSTR_ADDR, &sstrVal);
  LS7866_Write(DevAddr, TPR_ADDR, TPR_RDST);       // Reset DSTR  XXX why?  
  sprintf(msgBuff, "Polled Cntr:%d Addr:%02x CNTR: %d (%08lx) SSTR: %02x\n",ADDR_JUMPERS, DevAddr, cntrVal, cntrVal, sstrVal);
  Serial.print(msgBuff);

  // setLed(LED_OFF);
  return error;
}

void setup() {

  // XXX this is all application 

  // Setup I2C Buss Master  
  // Wire.begin(I2cMstrAddr);    // Set my i2c slave address
  Wire.begin();    // Set i2c master mode
  // Wire.setClock(mstrClock);   // Set Buss Speed to Fast Mode 400K
  // Wire.setWireTimeout();      // Setup I2c Timeouts (default)
  // Setup Serial Monitor
  Serial.begin(9600);         // Setup serial monitor baud rate
  delay(7000);                // Wait for Serial Monitor to start

  sprintf(msgBuff, "Running program version %s\r\n", PRM_VERSION);
  Serial.print(msgBuff);
  // Setup LS7866 registers

  Serial.print("Setting up counter chip.\r\n");
  ChipSetup(CHIP_ADDR, CNTR_SIZE);

  Serial.print("Setup complete.\r\n");
  Serial.print("Starting Device Polling.\r\n");

  nextPollMillis = millis() + POLL_INTERVAL;
 
  }

void loop() {
  
  // XXX application
  
  // Check for polling interval
  if (millis() > nextPollMillis){
    nextPollMillis = millis() + POLL_INTERVAL;   // save next polling time stamp
 
    CounterPoll(CHIP_ADDR, CNTR_SIZE); 

  //Serial.print("\r\n");   // Add Break line
  }

}
