// Decoder Breakout Board library for Particle Photon 2
// PCB 472-1 v0.3 cdint.com


#include "Particle.h"
#include <Wire.h>
#include "LS7866_Registers.h"     // Include LS7866 Register Def's for I2C Counter chip
#include "decoder-breakout.h"


byte jumpers2chipAddr(byte jumpers){
  return LS7866_I2C_FIXED_ADDR + jumpers;
}

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
  // Request from Slave NumBytes and terminate with Stop
  Wire.requestFrom((int)slaveAddress, (int)numBytes, true); 
  // Read data out of Wire Buffer
  // Data is read from device in Msb to Lsb format
  // XXX buffer overrun possible at the beginning of the value section in RAM 
  // - right now we ask for numBytes from the i2c devices, and then we trust that
  //   the device is only sending us that many bytes.  If the device sends more bytes
  //   than we asked for, we will overwrite memory that we don't own.  This is a security
  //   and reliability issue.  We should check the number of bytes read from the i2c device
  //   and make sure it is the same as the number of bytes we asked for.  If it is not, we
  //   should return an error code and not trust the data that was read from the i2c device.
  // - XXX also, is it a valid read if bytes read < numBytes?  Should the while loop instead
  //   be `for (byte i = 0; i < numBytes; i++)`?
  while(Wire.available()) {
    Serial.print(".");
    *valPtr-- = (byte)Wire.read();    // load memory with data
  }
  Serial.print("\n");
  
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

// Constructor
DecoderBreakout::DecoderBreakout(byte addrJumpers, byte cntrSize){
    _addrJumpers = addrJumpers;
    _chipAddress = jumpers2chipAddr(addrJumpers);
    _cntrSize = cntrSize;
}

/* 
 * Function ChipSetup
 * Desc     Configures counter chip
 * Output   true if successful
 */
bool DecoderBreakout::ChipSetup(){
    _DataSet[0] = MCR0_QUAD_X1 + MCR0_FREE_RUN + MCR0_DISABLE_Z + MCR0_Z_EDGE_FE; // MCR0
    _DataSet[1] = MCR1_CNTR_4BYTE + MCR1_COUNT_ENABLE + MCR1_nCLR_DSTR_ON_RD + MCR1_SSTR_ON_RD; // MCR1
    _DataSet[2] = FCR_IDX; // FCR
    // IDR0
    _DataSet[3] = 0x00;
    _DataSet[4] = 0x00;
    _DataSet[5] = 0x08;
    _DataSet[6] = 0x00;
    // IDR1
    _DataSet[7] = 0x00;
    _DataSet[8] = 0x00;
    _DataSet[9] = 0x00;
    _DataSet[10] = 0xFF;
    // TPR
    _DataSet[11] = TPR_RDST;
    // XXX why is this not always 12?
    byte numBytes = sizeof(_DataSet)/sizeof(byte);

    // Set MCR1 Value in DataSet
    byte mcr1CntrSize = MCR1_CNTR_4BYTE;
    switch(_cntrSize){
      case 1: mcr1CntrSize =  MCR1_CNTR_1BYTE; break;
      case 2: mcr1CntrSize =  MCR1_CNTR_2BYTE; break;
      case 3: mcr1CntrSize =  MCR1_CNTR_3BYTE; break;
    }
    _DataSet[1] =  mcr1CntrSize + MCR1_COUNT_ENABLE + MCR1_nCLR_DSTR_ON_RD + MCR1_SSTR_ON_RD;

    // reset registers IDR0, IDR1, CNTR, ODR, DSTR, SSTR and LFLAG/ to default values
    LS7866_Write(_chipAddress, TPR_ADDR, TPR_MRST);  

    // Write DataSet
    Wire.beginTransmission(_chipAddress);                       // transmit to device
    Wire.write(MCR0_ADDR | LS7866_REG_AUTO_INC);                // Add Auto Inc Bit to base Register Value
    for (byte i = 0; i<numBytes; i++) Wire.write(_DataSet[i]);   // Load Wire buffer
    Wire.endTransmission();                                     // Send transmitting

    // read back MCR0 as a test
    byte mcr0;
    LS7866_Read(_chipAddress, MCR0_ADDR, &mcr0);
    if (mcr0 != _DataSet[0]){
      // set _MCR0 to the value that was read back
      _DataSet[0] = mcr0;
      return false;
    }
    return true;
}


// CounterCheck is used to check if the counter has Flags Set.  
// The Flags are set when the counter has reached the value set in the SSTR register.
// It reset DSTR and returns a nonzero int if the counter has Flags Set.
// XXX review for functionality -- what sets flags?
// XXX should return sstrVal and cntrVal to the caller instead of printing them
int DecoderBreakout::CounterCheck(){
  int reset = 0;
  unsigned long CntrVal = 0;
  unsigned long ValLong = 0;
  byte sstrVal = 0;
  byte sstrVal2 = 0;
  
  byte DevAddr = _chipAddress;
  byte cntrSize = _cntrSize;
  LS7866_Read(DevAddr, ODR_ADDR, &CntrVal, cntrSize);  // Read ODR which load SSTR
  LS7866_Read(DevAddr, SSTR_ADDR, &sstrVal);            // Read SSTR
  
  if (sstrVal & (SSTR_EQL0 + SSTR_BW)){            // Check if this counter has Flags Set
        LS7866_Write(DevAddr, TPR_ADDR, TPR_RDST);       // Reset DSTR 
        reset = 1;  
        // XXX
        // sprintf(msgBuff, "Cntr:%d was %08lx SSTR:%02x BW:%d EQU0:%d\n", CntrId, CntrVal, sstrVal, (sstrVal & SSTR_BW), (sstrVal & SSTR_EQL0));
        // Serial.print(msgBuff);
  }
   
  return reset;
}

// XXX we could return a struct that includes counter and SSTR values for the caller to use
// XXX or we could move SSTR read to a different function
unsigned long DecoderBreakout::CounterPoll(){
  unsigned long cntrVal = 0;
  unsigned long ValLong = 0;
  byte sstrVal = 0;
  // setLed(LED_GRN);

  byte DevAddr = _chipAddress;
  byte cntrSize = _cntrSize;
  LS7866_Read(DevAddr, CNTR_ADDR, &cntrVal, cntrSize);
  // LS7866_Read(DevAddr, SSTR_ADDR, &sstrVal);
  // LS7866_Write(DevAddr, TPR_ADDR, TPR_RDST);       // Reset DSTR -- nope, we don't need this because MCR1 is set such that DSTR get transferred to SSTR on every read
  // sprintf(msgBuff, "Polled Cntr:%d Addr:%02x CNTR: %d (%08lx) SSTR: %02x\n",ADDR_JUMPERS, DevAddr, cntrVal, cntrVal, sstrVal);
  // Serial.print(msgBuff);
  Serial.print("_");
  return cntrVal;
}
