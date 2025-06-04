#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "wol.h"
#define MAXLINE 1000
#define PASSWORD "12345"

int main()
{   
    unsigned char buffer[100] = {0};
    char *msg1 = "\n Enter password: \n";
    char *msg2 = "\n correct, sending packet.. \n";
    int listen_sock, len, new_sock ;
    struct sockaddr_in servaddr, cliaddr, woladdr;
    bzero(&servaddr, sizeof(servaddr));

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);        
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET; 
 
    bind(listen_sock, (struct sockaddr*)&servaddr, sizeof(servaddr));
    
    len = sizeof(cliaddr);
    
    listen(listen_sock, 1);

    new_sock = accept(listen_sock, (struct sockaddr*) &cliaddr, &len);
    
    send(new_sock, msg1, MAXLINE, 0);
    
    int n = recv(new_sock, buffer, sizeof(buffer), 0);
    buffer[n] = '\0';
    if(strncmp((char*)buffer, PASSWORD, strlen(PASSWORD)) == 0){
        send(new_sock, msg2, strlen(msg1), 0);
        
        system("./sendwol A1:B4:C5:C1:DD:3E");
    }