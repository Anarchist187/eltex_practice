#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define BUFFER_SIZE 65536
#define DUMP_FILE "udp_dump.txt"


void dump_data(int fd, const unsigned char *data, int length) {
    char log_msg[1024];
    for (int i = 0; i < length; i++) {
        snprintf(log_msg,sizeof(log_msg), "%02x ", data[i]);
        write(fd,log_msg,strlen(log_msg));
        if ((i + 1) % 16 == 0){
            snprintf(log_msg,sizeof(log_msg),"\n");
            write(fd,log_msg,strlen(log_msg));
        }
    }
    if (length % 16 != 0){
        snprintf(log_msg,sizeof(log_msg),"\n");
        write(fd,log_msg,strlen(log_msg));
    }
}

void dump_info(int dump_file, struct ip *ip_hdr, struct udphdr *udp_hdr, int udp_data_len, unsigned char *udp_data){
    char log_msg[1024];
    snprintf(log_msg,sizeof(log_msg), "----------------------------\n");
    write(dump_file,log_msg,strlen(log_msg));
    snprintf(log_msg,sizeof(log_msg), "Source IP: %s\n", inet_ntoa(ip_hdr->ip_src));
    write(dump_file,log_msg,strlen(log_msg));
    snprintf(log_msg,sizeof(log_msg), "Destination IP: %s\n", inet_ntoa(ip_hdr->ip_dst));
    write(dump_file,log_msg,strlen(log_msg));
    snprintf(log_msg,sizeof(log_msg), "Source Port: %d\n", ntohs(udp_hdr->source));
    write(dump_file,log_msg,strlen(log_msg));
    snprintf(log_msg,sizeof(log_msg), "Destination Port: %d\n", ntohs(udp_hdr->dest));
    write(dump_file,log_msg,strlen(log_msg));
    snprintf(log_msg,sizeof(log_msg), "UDP Payload length: %d\n", udp_data_len);
    write(dump_file,log_msg,strlen(log_msg));
    snprintf(log_msg,sizeof(log_msg), "Payload (hex):\n");
    write(dump_file,log_msg,strlen(log_msg));
    dump_data(dump_file, udp_data, udp_data_len);
    snprintf(log_msg,sizeof(log_msg), "\n");
    write(dump_file,log_msg,strlen(log_msg));
}

int main(int argc, char *argv[]) {
    int raw_socket;
    unsigned char buffer[BUFFER_SIZE];
    struct sockaddr_in source_addr;
    socklen_t addr_len = sizeof(source_addr);
    if (argc<4){
        printf("Usage: <server_ip (loopback)> <port_1> <port_2>");
        return 1;
    }
    char *server_ip = argv[1];
    int src_port=atoi(argv[2]);
    int dst_port=atoi(argv[3]);

    int dump_file = open(DUMP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dump_file<0) {
        perror("Error open");
        exit(EXIT_FAILURE);
    }

    // Создаем raw-сокет для IP протокола UDP
    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (raw_socket < 0) {
        perror("Error socket");
        close(dump_file);
        exit(EXIT_FAILURE);
    }

    printf("Сниффер UDP запущен, дамп будет записываться в %s\n", DUMP_FILE);

    while (1) {
        ssize_t data_size = recvfrom(raw_socket, buffer, BUFFER_SIZE, 0,(struct sockaddr *)&source_addr, &addr_len);
        if (data_size < 0) {
            perror("Error recvfrom");
            break;
        }
        // Парсим IP-заголовок
        struct ip *ip_hdr = (struct ip*)buffer;
        int ip_header_len = ip_hdr->ip_hl * 4;

        // Парсим UDP-заголовок
        struct udphdr *udp_hdr = (struct udphdr*)(buffer + ip_header_len);
        int udp_data_len = ntohs(udp_hdr->len) - sizeof(struct udphdr);
        unsigned char *udp_data = buffer + ip_header_len + sizeof(struct udphdr);

        // Записываем данные пакета в файл
        char dst_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ip_hdr->ip_dst),dst_ip,INET_ADDRSTRLEN);
        if (strcmp(dst_ip,server_ip)==0 && (ntohs(udp_hdr->dest)==src_port || ntohs(udp_hdr->dest)==dst_port)){
            dump_info(dump_file, ip_hdr, udp_hdr, udp_data_len, udp_data);
        }
    }

    close(raw_socket);
    close(dump_file);
    return 0;
}
