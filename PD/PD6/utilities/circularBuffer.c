// Defines functions in here
#include "circularBuffer.h"

// Needed for function implementation
#include <stdint.h>
#include <stdmansos.h>

// Stores the received packets
static uint32_t receivedPackets[STORED_PACKET_COUNT];
// Keeps track of how many packets were received
uint16_t receivedPacketCounter = 0;

void initializeCircularBuffer() {
    memset(receivedPackets, 0, sizeof receivedPackets);
}

bool packetAlreadyReceived(uint32_t id) {
    int i;

    for (i = 0; i < STORED_PACKET_COUNT; i++) {
        if (id == receivedPackets[i]) {
            return true;
        }
    }

    return false;
}

void pushIntoCircularBuffer(uint32_t id) {
    receivedPackets[receivedPacketCounter % 100] = id;
    receivedPacketCounter++;

    return;
}