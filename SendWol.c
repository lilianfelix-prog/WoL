#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "wol.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


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


int main(int argc, char *argv[])
{

    int sockd; 
    
    //read buffer for possible response
    unsigned char buffer[1024];
    struct sockaddr_in toaddr;

    // initialize socket information: IP/port of receiving socket
    bzero(&toaddr, sizeof(toaddr));
    toaddr.sin_family = AF_INET;
    toaddr.sin_port = htons(PORT);
    toaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //if(inet_pton(AF_INET, "127.0.0.1", &toaddr.sin_addr.s_addr) < 0){
    //    printf("\n Invalid IP Address \n");
    //    return -1;
    //}

    
    packet_t *packet = (packet_t *) malloc(sizeof(packet_t));
    mac_addr_t *mac = (mac_addr_t *) malloc(sizeof(mac_addr_t));


    unsigned char *arg = argv[1];

    printf("argv: %s \n", arg);

    if(parseMacAddr(arg, mac) < 0){
        printf("error could not parse MAC addr: %s", arg);
        return -1;
    } 

    if(packetMaker(mac, packet) < 0){
        printf("error packet was not formated correctly");
        return -1;
    }

    for(__uint8_t i = 0; i < sizeof(packet_t); i++ ){
        printf("%02X ", ((unsigned char *) packet)[i]);
    }

    //Initialize the socket for UDP accepting IPv4 adresses
    if(sockd = socket(AF_INET, SOCK_DGRAM, 0) < 0){
        printf("\n failure creating socket descriptor \n");
        return -1;
    }

    //Connect the file descriptor created to the adresse to send to specified
    if(connect(sockd, (struct sockaddr *)&toaddr, sizeof(toaddr)) < 0 ){
        printf("\n failure connection to: %zu \n", toaddr.sin_addr.s_addr);
        return -1;
    }

    //Send the Magic packet to the connection, waiting for response
    sendto(sockd, &packet, sizeof(packet), 0, (struct sockaddr*) &toaddr, sizeof(toaddr));

    recvfrom(sockd, buffer, sizeof(buffer), 0, (struct sockaddr*) &toaddr, NULL);
    puts(buffer);

    close(sockd);

    free(packet);
    free(mac);

    return 0;


}