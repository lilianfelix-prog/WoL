#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "wol.h"
#include <sys/socket.h>

int main(int argc, char *argv[])
{

    packet_t *packet = (packet_t *) malloc(sizeof(packet_t));
    mac_addr_t *mac = (mac_addr_t *) malloc(sizeof(mac_addr_t));

    unsigned char *arg = argv[1];

    printf("argv: %s \n", arg);

    if(parseMacAddr(arg, mac) < 0){
        printf("error could not parse MAC addr: %s", arg);
        return 0;
    } 

    if(packetMaker(mac, packet) < 0){
        printf("error packet was not formated correctly");
        return 0;
    }

    for(__uint8_t i = 0; i < sizeof(packet_t); i++ ){
        printf("%02X ", ((unsigned char *) packet)[i]);
    }

    free(packet);
    free(mac);

    return 0;


}

//format packet according to the Magic Packets structure with input MAC addr
int packetMaker(mac_addr_t *mac, packet_t *packet)
{
    for(__uint8_t i = 0; i < MAC_ADDR_SIZE; i++){
        packet->header[i] = 0xFF;
    }

    for(__uint8_t i = 0; i < WOL_PAYLOAD_SIZE; i += MAC_ADDR_SIZE){
        memcpy(&packet->payload[i], mac->mac_addr, MAC_ADDR_SIZE);
    }
    
    if(packet->header == NULL || packet->payload == NULL){
        return -1;
    }
    return 0;

}

//parse the input arg to get the MAC addr, 
//todo: check for validity of hex values 0-9 / A-F
int parseMacAddr(unsigned char *macInput, mac_addr_t *mac)
{
    if(macInput == NULL){
        printf("macInput is NULL");
        return -1;
    }
    printf("%hhx", macInput);

    sscanf(macInput, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
        &mac->mac_addr[0], &mac->mac_addr[1], 
        &mac->mac_addr[2], &mac->mac_addr[3], 
        &mac->mac_addr[4], &mac->mac_addr[5]);

    if(mac->mac_addr == NULL){
        return -1;
    }
    return 0;
}

