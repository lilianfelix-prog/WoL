#include <stdio.h>
#include <string.h>
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
    char buffer[100] = {0};
    char *password = "12345";
    char *cmd_wol = "./sendwol A1:B4:C5:C1:DD:3E";
    char *msg1 = "\n Enter password: \n";
    char *msg2 = "\n correct, sending packet.. \n";
    int listen_sock, len, new_sock ;
    struct sockaddr_in servaddr, cliaddr, woladdr;
    bzero(&servaddr, sizeof(servaddr));

    listen_sock = socket(AF_INET, SOCK_STREAM, 0); 
    if (listen_sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET; 
 
    if (bind(listen_sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(listen_sock);
        exit(1);
    }
    
    // Listen for connections
    if (listen(listen_sock, 5) < 0) {
        perror("Listen failed");
        close(listen_sock);
        exit(1);
    }

    len = sizeof(cliaddr);

    new_sock = accept(listen_sock, (struct sockaddr*) &cliaddr, &len);
    
    send(new_sock, msg1, sizeof(msg1), 0);
    
    int n = recv(new_sock, buffer, sizeof(buffer)-1, 0);
    if (n <= 0) {
        printf("No data received or connection closed\n");
        close(new_sock);
        close(listen_sock);
        return 1;
    }
    buffer[n] = '\0';

    char *newline = strchr(buffer, '\n');
    if (newline) *newline = '\0';
    char *carriage = strchr(buffer, '\r');
    if (carriage) *carriage = '\0';

    if(strcmp(buffer, password) == 0){
            
        send(new_sock, msg2, strlen(msg2), 0);
        
        int ret = system(cmd_wol);
        if (ret == -1) {
            char* err_sys = "system() failed";
            send(new_sock, err_sys, strlen(err_sys), 0);
        } 
    }

    close(new_sock);
    close(listen_sock);

    return 0;
}