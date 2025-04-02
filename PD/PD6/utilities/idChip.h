#ifndef ID_CHIP_H
#define ID_CHIP_H
#include <stdint.h>

// Function that returns the unique identifier of the msp430 chip,
// that is stored in ds2401 (acts as an abstraction layer in case of change)
uint64_t getID();

#endif