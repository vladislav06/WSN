//
// Created by vm on 25.2.4.
//
#include "protocol.h"
#include "stdmansos.h"

#define DEBUG

#include "./../utilities/debug.h"

struct Packet createPacket(uint16_t deviceID, enum DeviceTypes deviceType, uint16_t packetID) {
    struct Packet packet;
    packet.magic = MAGIC;
    packet.deviceType = deviceType;
    packet.deviceID = deviceID;
    packet.packetID = packetID;
    packet.checksum = 0;
    return packet;
}

struct Advertisement createAdvPacket(uint16_t deviceID, uint16_t packetID) {
    struct Advertisement packet;
    packet.magic = MAGIC;
    packet.deviceID = deviceID;
    packet.packetID = packetID;
    packet.checksum = 0;
    packet.blacklistedDeviceId = 0;
    packet.recordedHopCount = 0;
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
void calcPckChecksum(struct Packet *packet) {
    packet->checksum = 0;
    uint16_t crc = crcSlow((uint8_t *) packet, sizeof(struct Packet));
    packet->checksum = crc;
}

/**
 * Calculates checksum of packet and saves it into checksum field
 * @param packet
 */
void calcAdvChecksum(struct Advertisement *packet) {
    packet->checksum = 0;
    uint16_t crc = crcSlow((uint8_t *) packet, sizeof(struct Advertisement));
    packet->checksum = crc;
}

/**
 * Checks whether checksum of a packet matches with what is writen in the packet
 * @param packet
 * @return true if checksums are equal
 */
bool checkPckChecksum(struct Packet *packet) {
    // save packet's crc into temp var, because original crc was calculated with packet->checksum field being 0
    uint16_t crc = packet->checksum;
    packet->checksum = 0;
    uint16_t calculatedCrc = crcSlow((uint8_t *) packet, sizeof(struct Packet));
    packet->checksum = crc;

    if (calculatedCrc != packet->checksum) {
        DEB("checksum mismatch, must be:%d | got:%d\n", calculatedCrc, packet->checksum);
    }
    return calculatedCrc == packet->checksum;
}

/**
 * Checks whether checksum of a packet matches with what is writen in the packet
 * @param packet
 * @return true if checksums are equal
 */
bool checkAdvChecksum(struct Advertisement *packet) {
    // save packet's crc into temp var, because original crc was calculated with packet->checksum field being 0
    uint16_t crc = packet->checksum;
    packet->checksum = 0;
    uint16_t calculatedCrc = crcSlow((uint8_t *) packet, sizeof(struct Advertisement));
    packet->checksum = crc;

    if (calculatedCrc != packet->checksum) {
        DEB("checksum mismatch, must be:%d | got:%d\n", calculatedCrc, packet->checksum);
    }
    return calculatedCrc == packet->checksum;
}

bool checkPckValidity(struct Packet *packet) {
    if (packet->magic != MAGIC) {
        DEB("magic missmatch\n");
        return false;
    }
    if (!checkPckChecksum(packet)) {
        DEB("checksum missmatch\n");
        return false;
    }
    return true;
}

bool checkAdvValidity(struct Advertisement *packet) {
    if (packet->magic != MAGIC) {
        DEB("magic mismatch\n");
        return false;
    }
    if (!checkAdvChecksum(packet)) {
        DEB("checksum mismatch\n");
        return false;
    }
    return true;
}
