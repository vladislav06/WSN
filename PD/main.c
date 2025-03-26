#include "stdmansos.h"
#include <stdint.h>

//Unique id of this mote
#define UID 0x6996

uint16_t packetID = 0;

void transmit();

void appMain(void) {

    radioInit();
    radioOn();

    while (true) {
        transmit();
        packetID++;
        mdelay(100);
    }
}

void transmit() {
    // 2 byte id| 2 byte data| 2 byte checksum
    uint16_t packet[3];

    packet[0] = UID;
    packet[1] = packetID;

    //checksum is xor of 2 fields
    packet[2] = UID ^ packetID;

    radioSend(packet, 6);
}
