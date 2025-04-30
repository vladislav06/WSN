#include "stdmansos.h"
#include "serial_number.h"

#include <stdint.h>

#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"
#include "./../utilities/debug.h"

uint16_t packetID = 0;

uint16_t timeCounter = 0;

void recvRadio();

void transmit();

void processAdv(struct Advertisement *receivedPacket);

void processAdvStart(struct AdvertisementStart *receivedPacket);

struct RoutingTableEntry {
    uint16_t devId;
    uint16_t hopCount;
};

struct RoutingTableEntry routing = {0};
bool nextAdvOverWrites = false;
static uint16_t lastAdvId = 0;


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
    struct Packet receivedPacket;
    int16_t len;

    len = radioRecv(&receivedPacket, sizeof(struct Packet));
    DEB("Packet rec\n");

    if (len > 0) {
        // check if packet contains magic
        if (receivedPacket.magic != MAGIC) {
            DEB("Packet invalid, MAGIC:%d\n", receivedPacket.magic);
            return;
        }

        // actions dependant on packetType
        switch (receivedPacket.packetType) {
            case PACKET_TYPE_PACKET:
                break;
            case PACKET_TYPE_ADVERTISEMENT:
                // cast packet to adv type
                processAdv((struct Advertisement *) &receivedPacket);
                break;
            case PACKET_TYPE_ADVERTISEMENT_START:
                // cast packet to advStart type
                processAdvStart((struct AdvertisementStart *) &receivedPacket);
                break;
            default:
                DEB("Invalid packet or invalid packet type received\n");
                break;
        }
    }
}

void processAdv(struct Advertisement *receivedPacket) {
    // cast to adv packet
    struct Advertisement advPacket = *receivedPacket;

    // ignore invalid packets
    if (!checkAdvValidity(&advPacket)) {
        DEB("Packet invalid, MAGIC:%d\n", receivedPacket->magic);
        return;
    }

    if (advPacket.recordedHopCount < routing.hopCount || nextAdvOverWrites) {
        nextAdvOverWrites = false;
        routing.hopCount = advPacket.recordedHopCount;
        routing.devId = advPacket.deviceID;
    }
}

void processAdvStart(struct AdvertisementStart *receivedPacket) {
    // cast to adv packet
    struct AdvertisementStart advsPacket = *receivedPacket;

    // ignore invalid packets
    if (!checkAdvStartChecksum(&advsPacket)) {
        DEB("Packet invalid, MAGIC:%d\n", receivedPacket->magic);
        return;
    }
    nextAdvOverWrites = true;
}


void transmit() {
    // create packet and fill packet
    // use first 2 bytes of id and hope that no collision will occur
    struct Packet packet = createPacket(getID(), DEVICE_TYPE_SENSOR, packetID);
    packetID++;

    struct Payload payload;
    payload.lightSensorValue = adcRead(5);
    packet.payload = payload;
    // deduct 1 for sending this packet (consider it as the first step)
    packet.hopCount = routing.hopCount - 1;
    packet.target = routing.devId;

    calcPckChecksum(&packet);

    PRINTF("packetID: %d\n", packet.packetID);
    PRINTF("checksum: %u\n", packet.checksum);

    radioSend(&packet, sizeof(struct Packet));
}