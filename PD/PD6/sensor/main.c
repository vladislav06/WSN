#include "stdmansos.h"
#include "serial_number.h"

#include <stdint.h>

#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"
#include "./../utilities/debug.h"

uint16_t packetID = 0;

uint16_t recordedHopCount = UINT16_MAX;

uint16_t timeCounter = 0;

void recvRadio();

void transmit();

void appMain(void) {
    radioInit();

    radioSetReceiveHandle(recvRadio);

    radioOn();

    while (true) {
        transmit();
        mdelay(1000);
        timeCounter++;
        // Output this device's type every 5 tics
        if (timeCounter % 5 == 0) {
            PRINTF("Output from SENSOR, device ID is: ");
            PRINTF("%u\n", getID());
        }
    }
}

void recvRadio(void) {
    struct Advertisement packet;
    int len = radioRecv(&packet, sizeof(struct Advertisement));
    DEB("Packet rec\n");

    if (len > 0) {
        // ignore invalid packets
        if (!checkAdvValidity(&packet)) {
            DEB("Packet invalid, MAGIC:%d\n", packet.magic);
            return;
        }

        // record new hopCount
        if (packet.recordedHopCount < recordedHopCount) {
            recordedHopCount = packet.recordedHopCount;
            DEB("Recorded new hopCount: %u\n", recordedHopCount);
        }
    }
}

void transmit() {
    // skip creating packet if the hopCount has not been changed
    if (recordedHopCount == UINT16_MAX) {
        DEB("HopCount not changed, packet not sent");
        return;
    }

    // create packet and fill packet
    // use first 2 bytes of id and hope that no collision will occur
    struct Packet packet = createPacket(getID() , DEVICE_TYPE_SENSOR, packetID);
    packetID++;

    struct Payload payload;
    payload.lightSensorValue = adcRead(5);
    packet.payload = payload;
    // deduct 1 for sending this packet (consider it as the first step)
    packet.hopCount = recordedHopCount - 1;

    calcPckChecksum(&packet);

    PRINTF("packetID: %d\n", packet.packetID);
    PRINTF("checksum: %u\n", packet.checksum);

    radioSend(&packet, sizeof(struct Packet));
}