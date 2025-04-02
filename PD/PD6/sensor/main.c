#include "stdmansos.h"
#include "serial_number.h"
#include <stdint.h>
#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"

uint16_t packetID = 0;

void transmit();

void appMain(void) {
    radioInit();
    radioOn();

    uint64_t id = getID();
    PRINTF("%lu\n", id);

    while (true) {
        transmit();
        packetID++;
        mdelay(100);
    }
}

void transmit() {
    // 2 byte id| 2 byte data| 2 byte checksum
    struct Packet packet = {};

    packet.magic = MAGIC;
    packet.packetID = packetID;
    packetID++;
    packet.deviceID =

            radioSend(&packet, sizeof(packet));
}

