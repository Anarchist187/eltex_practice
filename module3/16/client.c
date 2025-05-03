#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define BUF_SIZE 256

void error (const char *msg){
    perror (msg);
    exit(EXIT_FAILURE);
}

void menu(){
    printf("Enter operation:\n");
    printf("+ - add\n");
    printf("- - substract\n");
    printf("* - multiply\n");
    printf("/ - divide\n");
    printf("Your option: ");
}

int main (int argc, char *argv[]){
    int tcp_sock, port, num_bytes,bytes_sent,action,fd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buff[BUF_SIZE];

    if (argc<3){
        printf("Usage: %s <hostname> <port>\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    port=atoi(argv[2]);
    tcp_sock=socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock<0){
        error("Error socket");
    }
    server=gethostbyname(argv[1]);
    if (server==NULL){ 
        error("Error gethostbyname");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    bcopy((char *)server->h_addr_list[0], (char*)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port=htons(port);

    if (connect(tcp_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
        error("Error connect");
    }
    memset(&buff,0,sizeof(buff));

    printf ("Client running...\n");
    //Receiving request for option
    num_bytes=recv(tcp_sock, &buff,sizeof(buff)-1,0);
    if (num_bytes<0){
        error("Error recv");
    }
    printf("%s\n",buff);
    fgets(buff,BUF_SIZE-1,stdin);
    num_bytes=send(tcp_sock,&buff,strlen(buff),0);
    if (num_bytes<0){
        error("Error write");
    }
    buff[num_bytes]='\0';
    action=atoi(buff);
    if (action==1){
        //Receiving request for operation
        num_bytes=recv(tcp_sock, &buff,sizeof(buff)-1,0);
        if (num_bytes<0){
            error("Error recv");
        }
        printf("%s\n",buff);
        fgets(buff,BUF_SIZE-1,stdin);
        num_bytes=send(tcp_sock,&buff,strlen(buff),0);
        if (num_bytes<0){
            error("Error write");
        }
        //Receiving invitation for 1-st arg
        memset(&buff,0,sizeof(buff));
        num_bytes=recv(tcp_sock, &buff,sizeof(buff)-1,0);
        if (num_bytes<0){
            error("Error recv");
        }
        printf("%s\n",buff);
        //Entering 1-st arg
        memset(&buff,0,sizeof(buff));
        fgets(buff,BUF_SIZE-1,stdin);
        num_bytes=send(tcp_sock,&buff,strlen(buff),0);
        if (num_bytes<0){
            error("Error send");
        }
        //Receiving invitation for 2-nd arg
        memset(&buff,0,sizeof(buff));
        num_bytes=recv(tcp_sock, &buff,sizeof(buff)-1,0);
        if (num_bytes<0){
            error("Error recv");
        }
        printf("%s\n",buff);
        //Entering 2-nd arg
        memset(&buff,0,sizeof(buff));
        fgets(buff,BUF_SIZE-1,stdin);
        num_bytes=send(tcp_sock,&buff,strlen(buff),0);
        if (num_bytes<0){
            error("Error send");
        }
        //Receiving result
        memset(&buff,0,sizeof(buff));
        num_bytes=recv(tcp_sock, &buff,sizeof(buff)-1,0);
        if (num_bytes<0){
            error("Error recv");
        }
        printf("%s\n",buff);
    } else if (action==2){
        memset(&buff,0,sizeof(buff));
        num_bytes=recv(tcp_sock, &buff,sizeof(buff)-1,0);
        if (num_bytes<0){
            error("Error recv");
        }
        printf("%s\n",buff);
        memset(&buff,0,sizeof(buff));
        scanf("%s",buff);
        fd=open(buff,O_RDONLY);
        if (fd==-1){
            error("Cannot open file");
        }
        num_bytes=send(tcp_sock,&buff,strlen(buff),0);
        if (num_bytes<0){
            error("Error send");
        }
        while (num_bytes=read(fd,buff,sizeof(buff)-1)){
            bytes_sent=send(tcp_sock,&buff,strlen(buff),0);
            if (bytes_sent<0){
            error("Error send");
            }
        }
        memset(&buff,0,sizeof(buff));
        num_bytes=recv(tcp_sock, &buff,sizeof(buff)-1,0);
        if (num_bytes<0){
            error("Error recv");
        }
        close(tcp_sock);
    }
}