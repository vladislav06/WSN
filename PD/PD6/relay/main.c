#include "stdmansos.h"
#include "serial_number.h"

#include <stdint.h>

#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"
#include "./../utilities/circularBuffer.h"

// TODO in this file:
// 1. Make relay receive advertisement packets:
// - it increments the packet's hopcount,
// - it records it's id in the blacklist of the packet,
// - it saves advertisement packet id and ignores it if received second time
// - and it resends the packet forward,
// 2. New algorithm for resending packets:
// - it receives a new data packet,
// - if the data packet's hopCount is 0, don't resend it,
// - if the data packet's blacklisted id is equal to the current device id, skip it,
// - if all is well, then:
// -- decrease the hopCount in the packet by 1,
// -- write it's own deviceId into blacklisted Id,
// -- resend the packet
// 3. Remove checks using circularBuffer.

static struct Packet receivedPacket;

void transmit();

void recvRadio();

void appMain(void) {
    radioInit();

    initializeCircularBuffer();

    radioSetReceiveHandle(recvRadio);

    radioOn();

    while (true) {
        mdelay(5000);
        PRINTF("Output from RELAY, device ID is: ");
        PRINTF("%u\n", getID());
    }
}

void transmit(struct Packet *packet) {
    packet->deviceType = DEVICE_TYPE_RELAY;
    calcChecksum(packet);

//    PRINTF("packetID: %d\n", packet->packetID);
//    PRINTF("checksum: %u\n", packet->checksum);

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

        PRINTF("Data from: %d:%d -> light:%d RESENDING\n", receivedPacket.deviceID, receivedPacket.packetID, receivedPacket.payload.lightSensorValue);

        // call transmit function
        transmit(&receivedPacket);
    }
}


void transmitAdv(struct Advertisement *adv) {
    adv->blacklistedDeviceId = adv->deviceID;
    adv->deviceID = getID();
    adv->recordedHopCount++;
    calcAdvChecksum(adv);
    radioSend(adv, sizeof(struct Advertisement));
}

void transmitAdvStart(struct AdvertisementStart *advs) {
    advs->blacklistedDeviceId = advs->deviceID;
    advs->deviceID = getID();
    calcAdvStartChecksum(advs);
    radioSend(advs, sizeof(struct AdvertisementStart));
}