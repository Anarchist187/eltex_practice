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
    printf("%s\n", buffer);
    while (1){
        // Ввод сообщения от пользователя
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        // Проверка на выход
        if (strcmp(buffer, "exit") == 0) {
            num_bytes=send(tcp_sock,&buffer,strlen(buffer),0);
            if (num_bytes<0){
                error("Error send");
            }
            break;
        }
        num_bytes=send(tcp_sock,&buffer,strlen(buffer),0);
        if (num_bytes<0){
            error("Error send");
            break;
        }
        //Receiving invitation for 1-st arg
        memset(&buffer,0,sizeof(buffer));
        num_bytes=recv(tcp_sock, &buffer,sizeof(buffer)-1,0);
        if (num_bytes<0){
            error("Error recv");
            break;
        }
        buffer[num_bytes] = '\0';
        printf("%s\n",buffer);
    }

    // Закрытие сокета
    close(tcp_sock);
    return 0;
}
