
#define MAC_ADDR_SIZE 6 

#define WOL_PASSWD_SIZE 4

#define WOL_PAYLOAD_SIZE 6*16

typedef struct
{
    unsigned char mac_addr[MAC_ADDR_SIZE];
} mac_addr_t;

// the MagicPacket contains the sync stream (6 bytes of 0xFF)
// followed by the target MAC adress repeated 16 times
typedef struct
{
    unsigned char header[MAC_ADDR_SIZE];
    unsigned char payload[WOL_PAYLOAD_SIZE];

} packet_t;

int packetMaker(mac_addr_t *mac, packet_t *packet);

int parseMacAddr(unsigned char *macInput, mac_addr_t *mac);

