#include "stdmansos.h"
#include "serial_number.h"
#include <stdint.h>
#include "./../protocol/protocol.h"
#include "./../utilities/idChip.h"

uint16_t packetID = 0;

void transmit();

void appMain(void) {
    radioInit();
    radioOn();


    while (true) {
        transmit();
        mdelay(1000);
        PRINTF("Output from SENSOR, device ID is: ");
        PRINTF("%lu\n", getID() & 0xFFFF);
    }
}

void transmit() {

    // create packet and fill packet
    // use first 2 bytes of id and hope that no collision will occur
    struct Packet packet = createPacket(getID() & 0xFFFF, DEVICE_TYPE_SENSOR, packetID);
    packetID++;

    struct Payload payload;
    payload.lightSensorValue = adcRead(5);
    packet.payload = payload;

    calcChecksum(&packet);

    PRINTF("packetID: %d\n", packet.packetID);
    PRINTF("checksum: %u\n", packet.checksum);


    radioSend(&packet, sizeof(struct Packet));
}

