#include "stdmansos.h"
#include <stdint.h>
#include "./../protocol/protocol.h"
//Unique id of this mote

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
   struct Packet packet = {};


    packet.uid = UID;
    packet.packetID = packetID;

    //checksum is xor of 2 fields
    packet.checksum= UID ^ packetID;

    radioSend(&packet, sizeof(packet));
}
