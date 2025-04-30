#include "stdmansos.h"
#include "serial_number.h"
#include <stdint.h>
#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"
// Uncomment this if you need debug outputs.
//#define DEBUG
#include "../utilities/debug.h"

void recvRadio(void);

void sendAdv(void);

static uint16_t advID = 0;

void appMain(void) {
    radioInit();
    radioSetChannel(19);
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

    struct AdvertisementStart advs = createAdvStartPacket(getID(), advID);
    calcAdvStartChecksum(&advs);
    radioSend(&advs, sizeof(struct AdvertisementStart));
    mdelay(500);

    struct Advertisement adv = createAdvPacket(getID(), advID);
    adv.recordedHopCount = 0;
    adv.blacklistedDeviceId = 0;
    calcAdvChecksum(&adv);
    radioSend(&adv, sizeof(struct Advertisement));
    advID++;
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

