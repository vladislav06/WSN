// --------------------------------------------
// PD5: Receiver.
// Receives the packets sent from the transmitter and counts the expected packet count:
// Usually, the transmitter sends 10 packets per second.
// If we manage to receive 10, then it's working great!
// If not, then we are most likely losing some packets.
// Displays received packet count, also displays their RSSI and LQI.
//
// Receiver written by Iļja Krasavins, ik22076
// Transmitter written by Vladislavs Agarkovs, va22017
// --------------------------------------------

#include "stdmansos.h"
#include "./../protocol/protocol.h"

/// GLOBAL VARIALBES
// Packet is stored here
static struct Packet receivedPacket;
// Counter for packets
uint16_t counter = 0;
// Keeping packet RSSI
int16_t totalPacketRSSI = 0;
// Keeping packet LQI
uint16_t totalPacketLQI = 0;

void recvRadio(void)
{    
    int16_t len;

    len = radioRecv(&receivedPacket, sizeof(receivedPacket));

    if (len > 0) {
	// begin checking whether we received the correct data...
        // Check ID
	if (receivedPacket.uid != UID) {
	    return;
	}
	
        // Check checksum
	if ((receivedPacket.uid ^ receivedPacket.packetID) != receivedPacket.checksum) {
	    return;
	}

	// increment counter
	counter++;

	// record RSSI
	totalPacketRSSI += radioGetLastRSSI();

	// record LQI
	totalPacketLQI += radioGetLastLQI();
    }
}

void appMain(void)
{
    radioInit();   

    PRINTF("Receiver started, waiting for packets...\n");
	
    radioSetReceiveHandle(recvRadio);
    radioOn();

    // Counter check and reset loop
    while (1) {
        mdelay(1000);

	// Display received packets
        PRINTF("Received packet count: %d\n", counter);

	// Display received packet RSSI
	PRINTF("Received packet RSSI, total: %d\n", totalPacketRSSI);
	PRINTF("Received packet RSSI, average: %d\n", totalPacketRSSI / counter);

	// Display received packet LQI
	PRINTF("Received packet LQI, total: %d\n", totalPacketLQI);
	PRINTF("Received packet LQI, average: %d\n", totalPacketLQI / counter);

	// Print divider for better readability
	PRINTF("\n===\n\n");

	// Reset global variables
	counter = 0;
	totalPacketRSSI = 0;
	totalPacketLQI = 0;
    }
}
