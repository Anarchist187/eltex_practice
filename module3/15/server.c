#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>


#define BUF_SIZE 256

int nclients;

void printusers(){
    if (nclients)
        printf("%d users connected\n",nclients);
    else    
        printf("No users connected\n");
}

void error (const char *msg){
    perror (msg);
    exit(EXIT_FAILURE);
}

float sum(float a, float b){
    return a+b;
}

float sub (float a, float b){
    return a-b;
}

float mult (float a, float b){
    return a*b;
}

float divide (float a, float b){
    return (b==0? NAN : a/b);
}
void process(int);

int main (int argc, char *argv[]){
    char buff[BUF_SIZE];
    int tcp_sock, tcp_newsock, pid, port;
    socklen_t clien;
    struct sockaddr_in serv_addr,cli_addr;
    if (argc < 2){
        error("Error num args");
    }
    port=atoi(argv[1]);
    tcp_sock=socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock<0){
        error("Error socket");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(port);

    if (bind(tcp_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
        error("Error bind");
    }
    listen(tcp_sock,5);
    printf("Server running\n");
    clien=sizeof(cli_addr);
    while (1){
        tcp_newsock=accept(tcp_sock,(struct sockaddr *)&cli_addr,&clien);
        if (tcp_newsock<0){
            error("Error accept");
        }
        printf ("Client connected!\n");
        nclients++;
        printusers();
        switch (pid=fork()){
            case -1:
                error ("Error fork");
            case 0:
                close(tcp_sock);
                process(tcp_newsock);
                exit(EXIT_SUCCESS);
            default:
                close(tcp_newsock);
                break;
        }
    }
    close (tcp_sock);
    exit(EXIT_SUCCESS);
}
void process(int sock){
    int num_bytes;
    char buff[BUF_SIZE];
    float a,b,result;
    char action;
    if(send(sock,"Server: Enter action (+ - * /): ", 32,0)<0){
        error("Error send");
    }
    num_bytes=recv(sock,buff,BUF_SIZE-1,0);
    if (num_bytes<0){
        error("Error recv");
    }
    action=buff[0];
    if(send(sock,"Server: Enter 1st arg: ", 23,0)<0){
        error("Error send");
    }
    num_bytes=recv(sock,buff,BUF_SIZE-1,0);
    if (num_bytes<0){
        error("Error recv");
    }
    buff[num_bytes]='\0';
    a=atoi(buff);
    if(send(sock,"Server: Enter 2nd arg: ", 23,0)<0){
        error("Error send");
    }
    num_bytes=recv(sock,buff,BUF_SIZE-1,0);
    if (num_bytes<0){
        error("Error recv");
    }
    buff[num_bytes]='\0';
    b=atoi(buff);
    switch (action){
        case '+': result=sum(a,b); break;
        case '-': result=sub(a,b);break;
        case '*': result=mult(a,b);break;
        case '/': result=divide(a,b);break;
        default:
            if(send(sock,"Invalid operation", 17,0)<0){
                error("Error send");
            }
            nclients--;
            printusers();
            return;
    }
    snprintf(buff,sizeof(buff),"Server: Result: %.2f\n",result);
    if(send( sock, buff, sizeof(buff), 0) < 0){
        error("Error send");
    }
    nclients--;
    printf("-disconnected\n");
    printusers();
    return;
}