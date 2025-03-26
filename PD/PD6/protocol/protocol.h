#define UID 0x6996

struct Packet {
    uint16_t uid;
    uint16_t packetID;
    uint16_t checksum;
};
