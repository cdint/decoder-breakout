// This program will setup Counters 0 - 2 and then poll the counters every 2 sec
// 
// 
// by Mark

// Verison Histroy
//  Ver   Date    Descriptsion
//  1.00   9/13/23 
#define PRM_VERSION "1.00"

#include <Wire.h>
#include "LS7866_Registers.h"     // Include LS7866 Register Def's

// Define which counters will be used
//#define CNTR_0  0
//#define CNTR_1  1
//#define CNTR_2  2
#define CNTR_7  7

#define BRD_INT_PIN 2 // INT0 is Pin2 , INT1 is Pin3
#define CNTR_SIZE 4   // number of bytes to configure counters

#define LED_OFF  0
#define LED_RED  1
#define LED_GRN  2

#define LED_FLASH_INTERVAL  2000   // 2 Sec
//#define LED_FLASH_INTERVAL  500   // .5 Sec
#define LED_FLASH_ON_TIME   50     // 50mS

const char fmtStartPromt[] = "APP LS7866-01 Ver %s\r\nI2C Master Address:%d Clock:%06ld\r\n";

/*
const int ledPin1 = 8;
const int ledPin2 = 9;
// const int SyncPin = 13;
*/

char msgBuff[64];
int I2cMstrAddr = 7;

int LFlag = 0;
int LFlagCntr = 0;
unsigned long mstrClock = 400000l;
unsigned long tsPollingInterval = 0;


/* 
 * Function setLed
 * Desc     configures drive currnt in TriColor LED
 * Input    Led Color State
 * Output   Led off, Led Green or Led Red
 */
/*
int setLed(int pState){
  switch(pState){
    default:
    case LED_OFF:
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, LOW);
      break;
    case LED_RED:
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, LOW);
      break;
    
    case LED_GRN:
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, HIGH);
      break;
  }
  return pState;
}
*/

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

void CounterSetup2(byte cntrId, byte slaveAddress, byte cntrSize){
    byte mcr1CntrSize = MCR1_CNTR_4BYTE;
    byte DataSet[] = {  MCR0_QUAD_X4 + MCR0_FREE_RUN + MCR0_DISABLE_Z + MCR0_Z_EDGE_FE,                // MCR0
                        MCR1_CNTR_4BYTE + MCR1_COUNT_ENABLE + MCR1_nCLR_DSTR_ON_RD + MCR1_SSTR_ON_RD,                                                                       // MCR1 filled in below
                        FCR_IDX ,                                                                    // FCR
                        0x00, 0x00, 0x08, 0x00,                                                     // IDR0
                        0x00, 0x00, 0x00, 0xFF,                                                     // IDR1
                        TPR_RDST};                                                                  // TPR
    byte numBytes = sizeof(DataSet)/sizeof(byte);

    // Set MCR1 Value in DataSet
    switch(cntrSize){
      case 1: mcr1CntrSize =  MCR1_CNTR_1BYTE; break;
      case 2: mcr1CntrSize =  MCR1_CNTR_2BYTE; break;
      case 3: mcr1CntrSize =  MCR1_CNTR_3BYTE; break;
      case 4:
      default:
              break;
    }
    DataSet[1] =  mcr1CntrSize + MCR1_COUNT_ENABLE + MCR1_nCLR_DSTR_ON_RD + MCR1_SSTR_ON_RD;
    LS7866_Write(slaveAddress, TPR_ADDR, TPR_MRST);  
    Wire.beginTransmission(slaveAddress);                       // transmit to device
    Wire.write(MCR0_ADDR | LS7866_REG_AUTO_INC);                // Add Auto Inc Bit to base Register Value
    for (byte i = 0; i<numBytes; i++) Wire.write(DataSet[i]);   // Load Wire buffer
    Wire.endTransmission();                                     // Send transmitting

    sprintf(msgBuff, "Setup2 Counter %d at Addr:%02x Bytes:%d MCR0:%02x MCR1:%02x\r\n", cntrId, slaveAddress, numBytes, DataSet[0], DataSet[1]);
    Serial.print(msgBuff);
    
}
int CounterCheck(byte CntrId, byte DevAddr, byte cntrSize){
  int error = 0;
  unsigned long CntrVal = 0;
  unsigned long ValLong = 0;
  byte sstrVal = 0;
  byte sstrVal2 = 0;
   
  LS7866_Read (DevAddr, ODR_ADDR, &CntrVal, cntrSize);  // Read ODR which load SSTR
  LS7866_Read(DevAddr, SSTR_ADDR, &sstrVal);            // Read SSTR
  
  if (sstrVal & (SSTR_EQL0 + SSTR_BW)){            // Check if this counter has Flags Set
        LS7866_Write(DevAddr, TPR_ADDR, TPR_RDST);       // Reset DSTR  
        sprintf(msgBuff, "LFlagCntr:%d Cntr:%d was %08lx SSTR:%02x BW:%d EQU0:%d\n",LFlagCntr, CntrId, CntrVal, sstrVal, (sstrVal & SSTR_BW), (sstrVal & SSTR_EQL0));
        Serial.print(msgBuff);
  }
   
  return error;
}


int CounterPoll(byte CntrId, byte DevAddr, byte cntrSize){
  int error = 0;
  unsigned long cntrVal = 0;
  unsigned long ValLong = 0;
  byte sstrVal = 0;
  // setLed(LED_GRN);

  LS7866_Read(DevAddr, CNTR_ADDR, &cntrVal, cntrSize);
  LS7866_Read(DevAddr, SSTR_ADDR, &sstrVal);
  LS7866_Write(DevAddr, TPR_ADDR, TPR_RDST);       // Reset DSTR  
  sprintf(msgBuff, "Polled Cntr:%d Addr:%02x CNTR: %08lx SSTR: %02x\n",CntrId, DevAddr, cntrVal, sstrVal);
  Serial.print(msgBuff);

  // setLed(LED_OFF);
  return error;
}

void setup() {
  // put your setup code here, to run once:
  
  // configure pins to drive Tri color led
  // pinMode(ledPin1, OUTPUT);
  // pinMode(ledPin2, OUTPUT);

  // setLed(LED_RED);            // Show this board is Active
  
  // Setup I2C Buss Master  
  // Wire.begin(I2cMstrAddr);    // Set my i2c slave address
  Wire.begin();    // Set i2c master mode
  Wire.setClock(mstrClock);   // Set Buss Speed to Fast Mode 400K
  // Wire.setWireTimeout();      // Setup I2c Timeouts (default)

  // Setup Serial Monitor
  Serial.begin(9600);         // Setup serial monitor baud rate
  sprintf(msgBuff, fmtStartPromt, PRM_VERSION, I2cMstrAddr, mstrClock);
  Serial.print(msgBuff);

  
  
  // Setup defined Counters
  Serial.print("Setting up Counters.\r\n");
#ifdef CNTR_0
  CounterSetup2(0, LS7866_I2C_FIXED_ADDR + 0, CNTR_SIZE);
#endif
#ifdef CNTR_1
  CounterSetup2(1, LS7866_I2C_FIXED_ADDR + 1, CNTR_SIZE);
#endif
#ifdef CNTR_2
  CounterSetup2(2, LS7866_I2C_FIXED_ADDR + 2, CNTR_SIZE);
#endif
#ifdef CNTR_7
  CounterSetup2(7, LS7866_I2C_FIXED_ADDR + 7, CNTR_SIZE);
#endif
  Serial.print("Setup complete.\r\n");
  Serial.print("Starting Device Polling.\r\n");

 
  // setLed(LED_OFF);  
  // extend on time for led
  tsPollingInterval = millis() + 500;
  }

void loop() {
  
  
  // Check for polling interval
  if (millis() > tsPollingInterval){
    tsPollingInterval = millis() + LED_FLASH_INTERVAL;   // save next polling time stamp

  // Poll defined counters
#ifdef CNTR_0
    CounterPoll(0, LS7866_I2C_FIXED_ADDR + 0, CNTR_SIZE); 
#endif
#ifdef CNTR_1    
    CounterPoll(1, LS7866_I2C_FIXED_ADDR + 1, CNTR_SIZE); 
#endif
#ifdef CNTR_2     
    CounterPoll(2, LS7866_I2C_FIXED_ADDR + 2, CNTR_SIZE); 
#endif
#ifdef CNTR_7     
    CounterPoll(7, LS7866_I2C_FIXED_ADDR + 7, CNTR_SIZE); 
#endif
  //Serial.print("\r\n");   // Add Break line
  }

}
