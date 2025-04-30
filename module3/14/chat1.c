#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define MSG_SIZE 64

int main(){
    char buf[MSG_SIZE];
    int structlen;
    int udp_socket=socket(AF_INET, SOCK_DGRAM,0);
    if (udp_socket<0){
        perror("Error socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_port=htons(5555);
    server.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    client.sin_family = AF_INET;
    client.sin_port=htons(4444);
    client.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    if (bind(udp_socket,(struct sockaddr *)&client, sizeof(client))<0){
        perror("Error bind");
        exit(EXIT_FAILURE);
    }
    while (1){
        fgets(buf,MSG_SIZE,stdin);
        if(sendto( udp_socket, buf, sizeof(buf), 0, (struct sockaddr *)&server, sizeof(server)) < 0){
            perror("Error sendto");
            exit(EXIT_FAILURE);
        }
        if(strncmp(buf,"exit",4)==0){
            break;
        }
        if (recvfrom(udp_socket, buf, MSG_SIZE, 0, (struct sockaddr *)&client, &structlen)<0){
            perror("Error recvfrom");
            exit(EXIT_FAILURE);
        }
        printf("Received: %s\n",buf);
        if (strncmp(buf,"exit",4)==0){
            break;
        }
    }
    close(udp_socket);
}