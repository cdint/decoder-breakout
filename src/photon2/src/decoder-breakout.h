#ifndef _DECODER_BREAKOUT_LIBRARY
#define _DECODER_BREAKOUT_LIBRARY

#include "Particle.h"

void ChipSetup(byte addrJumpers, byte cntrSize);
unsigned long CounterPoll(byte DevAddr, byte cntrSize);
byte jumpers2chipAddr(byte jumpers);

#endif