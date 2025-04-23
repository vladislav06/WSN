#include "stdmansos.h"
#include "serial_number.h"
#include <stdint.h>
#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"
#include "../utilities/circularBuffer.h"


void recvRadio(void);

void appMain(void) {
    radioInit();
    radioSetReceiveHandle(recvRadio);
    radioOn();
    while (1) {
        mdelay(1000);
    }
}


void recvRadio(void) {
    struct Packet packet;
    int len = radioRecv(&packet, sizeof(struct Packet));
    if (len > 0) {
        // ignore invalid packets
        if (checkValidity(&packet)) {
            return;
        }
        // check device type
        if (packet.deviceType != DEVICE_TYPE_RELAY) {
            return;
        }

        // Check whether this packet has already been received
        if (packetAlreadyReceived(packet.id)) {
            return;
        }

        // Write down this packet as a received one
        pushIntoCircularBuffer(packet.id);
        // send to usb

        PRINTF("Data from: %d: %d", packet.deviceID, packet.payload.lightSensorValue)
    }
}