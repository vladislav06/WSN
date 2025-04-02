// Defines functions in here
#include "idChip.h"

// Needed for function implementation
#include <stdint.h>
#include <stdmansos.h>
#include "serial_number.h"


uint64_t getID() {
    uint64_t result;

    serialNumberRead((uint8_t *) &result);

    return result;
}