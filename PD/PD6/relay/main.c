#include "stdmansos.h"
#include "serial_number.h"
#include <stdint.h>
#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"

void appMain(void) {
    //msp430InitDs2401();

    uniqueId = getID();

    while (true) {
        PRINTF("unique identifier\n");
        //PRINTF("unique identifier %d", uniqueId);
        mdelay(100);
    }
}
