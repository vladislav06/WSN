#include "stdmansos.h"
#include "serial_number.h"
#include <stdint.h>
#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"
#include "../utilities/circularBuffer.h"

// TODO in this file:
// 1. Make gateway send advertisement packets:
// - hopcount is initially 0,
// - serves to set hopcounts in relays, does not actually receive any data.
// 2. New algorithm for receiving packets:
// - simply outputs what it received,
// - does not check whether the packet was already received or not.
// 3. Remove checks using circularBuffer.
// CONCERNS:
// 1. We need to make sure that the gateway does not resend already received packets.
// Two choices:
// 1.1. Make the routing for the packet strict,
// 1.2. Somehow reject the packet without remembering it 
// (hopCounts? some other packet variable?)

void recvRadio(void);

void appMain(void) {
    initializeCircularBuffer();
    radioInit();
    radioSetReceiveHandle(recvRadio);
    radioOn();
    while (1) {
        mdelay(5000);
        PRINTF("Output from GATEWAY, device ID is: ");
        PRINTF("%u\n", getID());
    }
}


void recvRadio(void) {
    struct Packet packet;
    int len = radioRecv(&packet, sizeof(struct Packet));
//    PRINTF("Packet rec\n");

    if (len > 0) {
        // ignore invalid packets
        if (!checkValidity(&packet)) {
//            PRINTF("Packet invalid, MAGIC:%d\n", packet.magic);
            return;
        }

        // check device type
        if (packet.deviceType != DEVICE_TYPE_RELAY) {
//            PRINTF("Packet invalid, devType:%d\n", packet.deviceType);
            return;
        }

        // Check whether this packet has already been received
        if (packetAlreadyReceived(packet.id)) {
//            PRINTF("packetAlreadyReceived:%d|%d\n", packet.packetID, packet.deviceID);
            return;
        }

        // Write down this packet as a received one
        pushIntoCircularBuffer(packet.id);
        // send to usb

        PRINTF("Data from: %d:%d -> light:%d\n", packet.deviceID, packet.packetID, packet.payload.lightSensorValue);
    }
}