#include "stdmansos.h"
#include <stdint.h>

#include "./../protocol/protocol.h"
#include "./../utilities/msp430.h"

//Unique id of this mote
uint8_t uniqueId;

void appMain(void) {
    msp430InitDs2401();

    uniqueId = msp430GetUniqueIdentifier();

    while (true) {
        PRINTF("unique identifier\n");
        //PRINTF("unique identifier %d", uniqueId);
        mdelay(100);
    }
}
