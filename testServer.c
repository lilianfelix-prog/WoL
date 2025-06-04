#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "wol.h"
#define MAXLINE 1000

int main()
{   
    unsigned char buffer[1024] = {0};
    char *message = "packet received";
    int listenfd, len;
    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));

    listenfd = socket(AF_INET, SOCK_DGRAM, 0);        
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(WOL_PORT);
    servaddr.sin_family = AF_INET; 
 
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  
    len = sizeof(cliaddr);
    int n = recvfrom(listenfd, buffer, sizeof(packet_t),
            0, (struct sockaddr*)&cliaddr,&len); 
    buffer[n] = '\0';
    for(__uint8_t i = 0; i < sizeof(packet_t); i++ ){
        printf("%02X ", ((unsigned char *) buffer)[i]);
    }

    sendto(listenfd, message, MAXLINE, 0,
          (struct sockaddr*)&cliaddr, sizeof(cliaddr));

    close(listenfd);
    
    return 0;
}