#include "stdmansos.h"
#include "serial_number.h"

#include <stdint.h>

#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"
#include "./../utilities/debug.h"

static struct Packet receivedPacket;

void recvRadio();

void processRegular(struct Packet *packet);

void processAdv(struct Advertisement *receivedPacket);

void processAdvStart(struct AdvertisementStart *receivedPacket);

void transmitRegular(struct Packet *packet);

void transmitAdv(struct Advertisement *adv);

void transmitAdvStart(struct AdvertisementStart *advs);

struct RoutingTableEntry {
    uint16_t devId;
    uint16_t hopCount;
};

struct RoutingTableEntry routing = {0};
bool nextAdvOverWrites = true;
static uint16_t lastAdvId = 0;

void appMain(void) {
    radioInit();

    radioSetReceiveHandle(recvRadio);

    radioOn();

    while (true) {
        mdelay(5000);
        PRINTF("Output from RELAY, device ID is: ");
        PRINTF("%u\n", getID());
    }
}

void recvRadio() {
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
                processRegular(&receivedPacket);
                break;
            case PACKET_TYPE_ADVERTISEMENT:
                processAdv((struct Advertisement *) &receivedPacket);
                break;
            case PACKET_TYPE_ADVERTISEMENT_START:
                processAdvStart((struct AdvertisementStart *) &receivedPacket);
                break;
            default:
                DEB("Invalid packet or invalid packet type received\n");
                break;
        }
    }
}

void processRegular(struct Packet *receivedPacket) {
    // ignore invalid packets
    if (!checkPckValidity(receivedPacket)) {
        DEB("Packet invalid, MAGIC:%d\n", receivedPacket->magic);
        return;
    }

    // ignore packets where hopCount is 0
    if (!receivedPacket->hopCount) {
        DEB("Packet hop count has reached 0\n");
        return;
    }

    // call transmit function for regular packet
    transmitRegular(receivedPacket);
    return;
}

void processAdv(struct Advertisement *receivedPacket) {
    // cast to adv packet
    struct Advertisement advPacket = *receivedPacket;

    // ignore invalid packets
    if (!checkAdvValidity(&advPacket)) {
        DEB("Packet invalid, MAGIC:%d\n", receivedPacket->magic);
        return;
    }

    // ignore packets that should be ignored
    if (advsPacket.packetID == lastAdvId) {
        DEB("Received packet ID is equal to lastAdvId, skipped\n");
        return;
    }
    
    // ignore the blacklisted packets
    if (advPacket.blacklistedDeviceId == getID()) {
        DEB("In blacklist\n");
        return;
    }

    // overwrite hopCount if new hopCount is less OR if we were instructed to
    if (advPacket.recordedHopCount < routing.hopCount || nextAdvOverWrites) {
        nextAdvOverWrites = false;
        routing.hopCount = advPacket.recordedHopCount;
        routing.devId = advPacket.deviceID;
    }

    // Save packet id to avoid retransmission
    lastAdvId = advPacket.packetID;

    //send to the rest of the network
    transmitAdv(&advPacket);
    return;
}

void processAdvStart(struct AdvertisementStart *receivedPacket) {
    // cast to advStart packet
    struct AdvertisementStart advsPacket = *receivedPacket;

    // ignore invalid packets
    if (!checkAdvStartChecksum(&advsPacket)) {
        DEB("Packet invalid, MAGIC:%d\n", receivedPacket->magic);
        return;
    }

    // ignore packets that should be ignored
    if (advsPacket.packetID == lastAdvId) {
        DEB("Received packet ID is equal to lastAdvId, skipped\n");
        return;
    }

    // ignore the blacklisted packets
    if (advsPacket.blacklistedDeviceId == getID()) {
        DEB("In blacklist\n");
        return;
    }

    // Save packet id to avoid retransmission
    lastAdvId = advsPacket.packetID;
    // since advStart was received, routing must be reset
    nextAdvOverWrites = true;

    transmitAdvStart(&advsPacket);
    return;
}

void transmitRegular(struct Packet *packet) {
    packet->deviceType = DEVICE_TYPE_RELAY;
    packet->hopCount = (packet->hopCount) - 1;
    calcPckChecksum(packet);
    packet->target = routing.devId;

    DEB("packetID: %d\n", packet->packetID);
    DEB("checksum: %u\n", packet->checksum);

    radioSend(packet, sizeof(struct Packet));
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
