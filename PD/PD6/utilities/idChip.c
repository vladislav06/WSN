// Defines functions in here
#include "idChip.h"

// Needed for function implementation
#include <stdint.h>
#include <stdmansos.h>
#include "serial_number.h"


uint16_t getID() {
    uint16_t result[4];

    serialNumberRead((uint8_t *) result);

    return result[3];
}