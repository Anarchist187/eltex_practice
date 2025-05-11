#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <math.h>

#define PORT 8888
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

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

int main() {
    int server_fd, epoll_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    float a,b,result;
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    char buffer[BUFFER_SIZE],action;
    const char *welcome_message = "Server: Enter action (+ - * /): \n";

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Привязка сокета к порту
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d\n", PORT);

    // Создание epoll
    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Добавление серверного сокета в epoll
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Ожидание события
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server_fd) {
                // Новый клиент
                int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                if (new_socket == -1) {
                    perror("accept");
                    continue;
                }

                // Отправка приветственного сообщения
                send(new_socket, welcome_message, strlen(welcome_message), 0);
                printf("Client connected.\n");

                // Добавление нового сокета в epoll
                event.events = EPOLLIN;
                event.data.fd = new_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &event) == -1) {
                    perror("epoll_ctl: new_socket");
                    exit(EXIT_FAILURE);
                }
            } else {
                // Обработка данных от клиента
                ssize_t bytes_received = recv(events[i].data.fd, buffer, sizeof(buffer) - 1, 0);
                if (bytes_received <= 0) {
                    // Соединение закрыто клиентом
                    close(events[i].data.fd);
                    printf("Client disconnected.\n");
                } 
                buffer[bytes_received] = '\0';  // Завершаем строку
                printf("Received from client: %s\n", buffer);
                action=buffer[0];
                if(send(events[i].data.fd,"Server: Enter 1st arg: ", 23,0)<0){
                    perror("Error send");
                    exit(EXIT_FAILURE);
                }
                bytes_received=recv(events[i].data.fd, buffer, sizeof(buffer) - 1, 0);
                if (bytes_received<0){
                    perror("Error recv");
                    exit(EXIT_FAILURE);
                }
                buffer[bytes_received]='\0';
                printf("Received from client: %s\n", buffer);
                a=atoi(buffer);
                if(send(events[i].data.fd,"Server: Enter 2nd arg: ", 23,0)<0){
                    perror("Error send");
                    exit(EXIT_FAILURE);
                }
                bytes_received=recv(events[i].data.fd, buffer, sizeof(buffer) - 1, 0);
                if (bytes_received<0){
                    perror("Error recv");
                    exit(EXIT_FAILURE);
                }
                buffer[bytes_received]='\0';
                printf("Received from client: %s\n", buffer);
                b=atoi(buffer);
                switch (action){
                    case '+': result=sum(a,b); break;
                    case '-': result=sub(a,b);break;
                    case '*': result=mult(a,b);break;
                    case '/': result=divide(a,b);break;
                    default:
                        if(send(events[i].data.fd,"Invalid operation", 17,0)<0){
                            perror("Error send");
                            exit(EXIT_FAILURE);
                        }
                        close(events[i].data.fd);
                        continue;
                }
                snprintf(buffer,sizeof(buffer),"Server: Result: %.2f",result);
                if(send(events[i].data.fd, buffer, sizeof(buffer), 0) < 0){
                    perror("Error send");
                    exit(EXIT_FAILURE);
                }
                printf("Sent to client: %f\n", result);
                close(events[i].data.fd);
            }
        }
    }

    close(server_fd);
    return 0;
}
