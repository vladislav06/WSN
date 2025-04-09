#include "stdmansos.h"
#include "serial_number.h"
#include <stdint.h>
#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"
#include "./../utilities/circularBuffer.h"

// TODO: Implement received packet array in utils to reuse in gateway
static struct Packet receivedPacket;

void transmit();

void recvRadio();

void appMain(void) {
    radioInit();

    initializeCircularBuffer();

    radioSetReceiveHandle(recvRadio);

    radioOn();

    while (true) {
        // TODO: Define printf here (if needed...)
        mdelay(1000);
        PRINTF("%lu\n", getID() & 0xFFFF);
    }
}

void transmit(struct Packet *packet) {
    packet->deviceType = DEVICE_TYPE_RELAY;
    calcChecksum(&packet);

    PRINTF("packetID: %d\n", packet->packetID);
    PRINTF("checksum: %u\n", packet->checksum);

    radioSend(&packet, sizeof(packet));
}

void recvRadio() {
    int16_t len;

    len = radioRecv(&receivedPacket, sizeof(receivedPacket));

    if (len > 0) {
        // begin checking whether we received the correct data...

        // Check Magic
        if (receivedPacket.magic != MAGIC) {
            return;
        }
        
        // Check checksum
        if (!checkChecksum(&receivedPacket)) {
            return;
        }

        // Check whether this packed has already been received
        if (packetAlreadyReceived(receivedPacket.id)) {
            return;
        }

        // Write down this packet as a sent one
        pushIntoCircularBuffer(receivedPacket.id);

        // call transmit function
        transmit(&receivedPacket);
    }
}
