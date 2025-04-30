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

/*
    Main packet structure - carries the sensor value
*/
struct Packet {
    // Number to identify this protocol's packets
    uint16_t magic;

    // Device type to distinguish between Sensor and Relay
    uint8_t deviceType;

    // TODO: Consider whether this id is still needed, may be simplified
    // Or removed altogether in favor of something else.
    union {
        struct {
            // Unique device identificator
            uint16_t deviceID;

            // Unique packet identificator
            uint16_t packetID;
        };
        uint32_t id;
    };

    // Target relay ID, only one relay will receive this packet
    uint16_t target;

    // Packet payload
    struct Payload payload;

    // Data about how many hops this packet has left
    uint16_t hopCount;

    // Packet checksum for error check
    uint16_t checksum;
};

/*
    Advertisement packet structure - used to set hopCount in origin (sensor)
*/
struct Advertisement {
    // Number to identify this protocol's packets
    uint16_t magic;

    // TODO: Consider whether this id is still needed, may be simplified
    // Or removed altogether in favor of something else.
    union {
        struct {
            // Unique device identificator
            uint16_t deviceID;

            // Unique packet identificator
            uint16_t packetID;
        };
        uint32_t id;
    };

    // Blacklisted relay ID to avoid resending to the same one
    uint16_t blacklistedDeviceId;

    // Recorded hops
    uint16_t recordedHopCount;

    // Packet checksum for error check
    uint16_t checksum;
};


struct Packet createPacket(uint16_t deviceID, enum DeviceTypes deviceType, uint16_t packetID);

struct Advertisement createAdvPacket(uint16_t deviceID, uint16_t packetID);

/**
 * Calculates checksum of packet and saves it into checksum field
 * @param packet
 */
void calcPckChecksum(struct Packet *packet);

/**
 * Calculates checksum of packet and saves it into checksum field
 * @param packet
 */
void calcAdvChecksum(struct Advertisement *packet);

/**
 * Checks whether checksum of a packet matches with what is writen in the packet
 * @param packet
 */
bool checkPckChecksum(struct Packet *packet);

/**
 * Checks whether checksum of a packet matches with what is writen in the packet
 * @param packet
 */
bool checkAdvChecksum(struct Advertisement *packet);


/**
 * Checks whether packet is valid, i.e. has valid MAGIC and checksum
 * @param packet
 * @return
 */
bool checkPckValidity(struct Packet *packet);

/**
 * Checks whether packet is valid, i.e. has valid MAGIC and checksum
 * @param packet
 * @return
 */
bool checkAdvValidity(struct Advertisement *packet);

#endif