#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stdmansos.h>

#define STORED_PACKET_COUNT 100

// Function that initializes it with 0
// (to avoid reading garbage data)
void initializeCircularBuffer();

// Function that checks whether a packet has already been received
bool packetAlreadyReceived(uint32_t id);

// Function that adds a packet to the list of received packets
void pushIntoCircularBuffer(uint32_t id);

#endif