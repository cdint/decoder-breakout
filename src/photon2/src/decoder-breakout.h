#ifndef _DECODER_BREAKOUT_LIBRARY
#define _DECODER_BREAKOUT_LIBRARY

// keep the first and second version components synced with the board version
#define DECODER_BREAKOUT_LIBRARY_VERSION "0.1.5"

#include "Particle.h"

// DecoderBreakout models the LS7366R Quadrature Counter chip from LSI Logic
class DecoderBreakout {
public:
    // constructor takes address jumpers and counter size
    DecoderBreakout(byte addrJumpers, byte cntrSize);
    // setup the chip
    bool ChipSetup();
    // get the counter value
    unsigned long CounterPoll();
    // check if the counter has flags set
    int CounterCheck();

    // GetAddressJumpers returns the address jumpers
    byte GetAddressJumpers() { return _addrJumpers; }
    // GetChipAddress returns the chip address
    byte GetChipAddress() { return _chipAddress; }
    // GetCounterSize returns the counter size
    byte GetCounterSize() { return _cntrSize; }
    // GetSSTR returns the SSTR value
    byte GetSSTR() { return _SSTR; }
    // GetMCR0 returns the MCR0 value
    byte GetMCR0() { return _DataSet[0]; }
    // GetMCR1 returns the MCR1 value
    byte GetMCR1() { return _DataSet[1]; }  

private:
    // number of bytes in the counter
    byte _cntrSize;
    // address jumpers
    byte _addrJumpers;
    // chip address (derived from jumpers)
    byte _chipAddress;
    // MCR data set
    byte _DataSet[12];
    // SSTR value
    byte _SSTR;

};

#endif