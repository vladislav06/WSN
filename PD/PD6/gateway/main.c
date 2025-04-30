#include "stdmansos.h"
#include "serial_number.h"
#include <stdint.h>
#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"
#include "../utilities/debug.h"

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

void sendAdv(void);

void appMain(void) {
    radioInit();
    radioSetReceiveHandle(recvRadio);
    radioOn();
    while (1) {
        mdelay(5000);
        PRINTF("Output from GATEWAY, device ID is: ");
        PRINTF("%u\n", getID());
        sendAdv();
    }
}

void sendAdv(void) {
    struct Advertisement adv = createAdvPacket(getID(), 0);
    adv.recordedHopCount = 0;
    adv.blacklistedDeviceId = 0;
    calcAdvChecksum(&adv);
    radioSend(&adv, sizeof(struct Advertisement));
}


void recvRadio(void) {
    struct Packet packet;
    int len = radioRecv(&packet, sizeof(struct Packet));
    DEB("Packet rec\n");

    if (len > 0) {
        // ignore invalid packets
        if (!checkPckValidity(&packet)) {
            DEB("Packet invalid, MAGIC:%d\n", packet.magic);
            return;
        }

        // check device type
        if (packet.deviceType != DEVICE_TYPE_RELAY) {
            DEB("Packet invalid, devType:%d\n", packet.deviceType);
            return;
        }

        // send to usb
        PRINTF("Data from: %d:%d -> light:%d\n", packet.deviceID, packet.packetID, packet.payload.lightSensorValue);
    }
}

