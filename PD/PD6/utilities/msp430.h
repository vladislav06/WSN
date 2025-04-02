#ifndef MSP430_H
#define MSP430_H
#include <stdint.h>

// Function that returns initializes ds2403 for further actions
void msp430InitDs2401();

// Function that returns the unique identifier of the msp430 chip,
// that is stored in ds2401 (acts as an abstraction layer in case of change)
uint64_t msp430GetUniqueIdentifier();

#endif