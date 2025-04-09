#include "stdmansos.h"
#include "serial_number.h"
#include <stdint.h>
#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"

// TODO: Implement received packet array in utils to reuse in gateway
#define STORED_PACKET_COUNT 100

static struct Packet receivedPacket;
static uint32_t receivedPackets[STORED_PACKET_COUNT];
uint16_t receivedPacketCounter = 0;

void transmit();

void recvRadio();

bool packetAlreadyReceived(uint32_t id);

void appMain(void) {
    radioInit();

    memset(receivedPackets, 0, sizeof receivedPackets);

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
        receivedPackets[receivedPacketCounter % 100] = receivedPacket.id;
        // increment counter of received packets
        receivedPacketCounter++;

        // call transmit function
        transmit(&receivedPacket);
    }
}

bool packetAlreadyReceived(uint32_t id) {
    int i;

    for (i = 0; i < STORED_PACKET_COUNT; i++) {
        if (id == receivedPackets[i]) {
            return true;
        }
    }

    return false;
}
