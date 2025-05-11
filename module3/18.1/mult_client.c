#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

void error (const char *msg){
    perror (msg);
    exit(EXIT_FAILURE);
}

int main() {
    int tcp_sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Создание TCP сокета
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(tcp_sock);
        exit(EXIT_FAILURE);
    }

    // Подключение к серверу
    if (connect(tcp_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(tcp_sock);
        exit(EXIT_FAILURE);
    }

    printf("Подключено к серверу %s:%d\n", SERVER_IP, SERVER_PORT);

    //Receiving request for operation
    ssize_t num_bytes = recv(tcp_sock, buffer, sizeof(buffer) - 1, 0);
    if (num_bytes <= 0) {
        perror("recv");
    }
    buffer[num_bytes] = '\0';  // Завершаем строку
    printf("Server: %s\n", buffer);
    // Ввод сообщения от пользователя
    fgets(buffer, sizeof(buffer), stdin);
    num_bytes=send(tcp_sock,&buffer,strlen(buffer),0);
    if (num_bytes<0){
        error("Error write");
    }
    //Receiving invitation for 1-st arg
    memset(&buffer,0,sizeof(buffer));
    num_bytes=recv(tcp_sock, &buffer,sizeof(buffer)-1,0);
    if (num_bytes<0){
        error("Error recv");
    }
    printf("%s\n",buffer);
    
    //Entering 1-st arg
    memset(&buffer,0,sizeof(buffer));
    fgets(buffer,BUFFER_SIZE-1,stdin);
    num_bytes=send(tcp_sock,&buffer,strlen(buffer),0);
    if (num_bytes<0){
        error("Error send");
    }
    //Receiving invitation for 2-nd arg
    memset(&buffer,0,sizeof(buffer));
    num_bytes=recv(tcp_sock, &buffer,sizeof(buffer)-1,0);
    if (num_bytes<0){
        error("Error recv");
    }
    printf("%s\n",buffer);
    //Entering 2-nd arg
    memset(&buffer,0,sizeof(buffer));
    fgets(buffer,BUFFER_SIZE-1,stdin);
    num_bytes=send(tcp_sock,&buffer,strlen(buffer),0);
    if (num_bytes<0){
        error("Error send");
    }
    //Receiving result
    memset(&buffer,0,sizeof(buffer));
    num_bytes=recv(tcp_sock, &buffer,sizeof(buffer)-1,0);
    if (num_bytes<0){
        error("Error recv");
    }
    printf("%s\n",buffer);

    // Закрытие сокета
    close(tcp_sock);
    return 0;
}
