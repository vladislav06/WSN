// Unique protocol identificator
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include "stdmansos.h"

#define MAGIC 0x6996

enum DeviceTypes {
    DEVICE_TYPE_SENSOR = 1,
    DEVICE_TYPE_RELAY,
    DEVICE_TYPE_GATEWAY
};

struct Payload {
    uint16_t lightSensorValue;
};

struct Packet {
    // Number to identify this protocol's packets
    uint16_t magic;

    // Device type to distinguish between Sensor and Relay
    uint8_t deviceType;

    union {
        struct {
            // Unique device identificator
            uint16_t deviceID;

            // Unique packet identificator
            uint16_t packetID;
        };
        uint32_t id;
    };

    // Packet payload
    struct Payload payload;

    // Packet checksum for error check
    uint16_t checksum;
};


struct Packet createPacket(uint16_t deviceID, enum DeviceTypes deviceType, uint16_t packetID);

/**
 * Calculates checksum of packet and saves it into checksum field
 * @param packet
 */
void calcChecksum(struct Packet *packet);

/**
 * Checks whether checksum of a packet matches with what is writen in the packet
 * @param packet
 */
bool checkChecksum(struct Packet *packet);

/**
 * Checks whether packet is valid, i.e. has valid MAGIC and checksum
 * @param packet
 * @return
 */
bool checkValidity(struct Packet *packet);

#endif