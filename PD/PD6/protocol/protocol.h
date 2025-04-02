// Unique protocol identificator
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define MAGIC 0x6996

struct Payload{
    uint8_t lightSensorValue;
};

struct Packet {
    // Number to identify this protocol packets
    uint16_t magic;

    union{
        struct{
            // Uniquie device identificator
            uint16_t deviceID;

            // Uniquie packet identificator
            uint16_t packetID;
        };
        uint32_t id;
    };

    // Packet payload
    struct Payload payload;

    // Packet checksum for error check
    uint16_t checksum;
};

#endif