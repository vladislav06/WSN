#include "stdmansos.h"
#include "serial_number.h"

#include <stdint.h>

#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"
#include "./../utilities/circularBuffer.h"

static struct Packet receivedPacket;

void transmit();

void recvRadio();

void appMain(void) {
    radioInit();

    initializeCircularBuffer();

    radioSetReceiveHandle(recvRadio);

    radioOn();

    while (true) {
        mdelay(1000);
        PRINTF("Output from RELAY, device ID is: ");
        PRINTF("%lu\n", getID() & 0xFFFF);
    }
}

void transmit(struct Packet *packet) {
    packet->deviceType = DEVICE_TYPE_RELAY;
    calcChecksum(packet);

    PRINTF("packetID: %d\n", packet->packetID);
    PRINTF("checksum: %u\n", packet->checksum);

    radioSend(packet, sizeof(struct Packet));
}

void recvRadio() {
    int16_t len;

    len = radioRecv(&receivedPacket, sizeof(struct Packet));
//    PRINTF("Packet rec\n");

    if (len > 0) {

        // ignore invalid packets
        if (!checkValidity(&receivedPacket)) {
//            PRINTF("Packet invalid, MAGIC:%d\n", receivedPacket.magic);
            return;
        }


        // Check whether this packet has already been received
        if (packetAlreadyReceived(receivedPacket.id)) {
//            PRINTF("packetAlreadyReceived:%d|%d\n", receivedPacket.packetID, receivedPacket.deviceID);
            return;
        }

        // Write down this packet as a sent one
        pushIntoCircularBuffer(receivedPacket.id);
//        PRINTF("Data from: %d: %d\n", receivedPacket.deviceID, receivedPacket.payload.lightSensorValue);

        // call transmit function
        transmit(&receivedPacket);
    }
}
