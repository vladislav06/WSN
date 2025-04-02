// Defines functions in here
#include "msp430.h"

// Needed for function implementation
#include <stdint.h>
#include <stdmansos.h>
#include "serial_number.h"

void msp430InitDs2401() {
    serialNumberInit();
}

uint64_t msp430GetUniqueIdentifier() {
    uint8_t serial[8];

    serialNumberRead(&serial);

    uint64_t result = &serial;

    return result;
}