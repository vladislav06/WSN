//
// Created by vm on 25.2.4.
//
#include "protocol.h"
#include "stdmansos.h"

struct Packet createPacket(uint16_t deviceID, enum DeviceTypes deviceType, uint16_t packetID) {
    struct Packet packet;
    packet.magic = MAGIC;
    packet.deviceType = deviceType;
    packet.deviceID = deviceID;
    packet.packetID = packetID;
    packet.checksum = 0;
    return packet;
}


// source: https://barrgroup.com/blog/crc-series-part-3-crc-implementation-code-cc
typedef uint16_t crc;
#define WIDTH  (8 * sizeof(uint16_t))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8

crc crcSlow(uint8_t const message[], uint32_t nBytes);

crc crcSlow(uint8_t const message[], uint32_t nBytes) {
    crc remainder = 0;
    /*
     * Perform modulo-2 division, a byte at a time.
     */
    int byte = 0;
    for (; byte < nBytes; ++byte) {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (message[byte] << (WIDTH - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        uint8_t bit = 8;
        for (; bit > 0; --bit) {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT) {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            } else {
                remainder = (remainder << 1);
            }
        }
    }
    return remainder;
}

/**
 * Calculates checksum of packet and saves it into checksum field
 * @param packet
 */
void calcChecksum(struct Packet *packet) {
    uint16_t crc = crcSlow((uint8_t *) packet, sizeof(struct Packet));
    packet->checksum = crc;
}

/**
 * Checks whether checksum of a packet matches with what is writen in the packet
 * @param packet
 * @return true if checksums are equal
 */
bool checkChecksum(struct Packet *packet) {
    // save packet's crc into temp var, because original crc was calculated with packet->checksum field being 0
    uint16_t crc = packet->checksum;
    packet->checksum = 0;
    uint16_t calculatedCrc = crcSlow((uint8_t *) packet, sizeof(struct Packet));
    packet->checksum = crc;

    if (calculatedCrc != packet->checksum) {
//        PRINTF("checksum mismatch, must be:%d | got:%d\n", calculatedCrc, packet->checksum);
    }
    return calculatedCrc == packet->checksum;
}

bool checkValidity(struct Packet *packet) {
    if (packet->magic != MAGIC) {
//        PRINTF("magic missmatch\n");
        return false;
    }
    if (!checkChecksum(packet)) {
//        PRINTF("checksum missmatch\n");
        return false;
    }
    return true;
}
