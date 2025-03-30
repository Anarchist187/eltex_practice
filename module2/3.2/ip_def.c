#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

uint32_t ip_to_uint32(const char *ip){
    uint32_t result = 0;
    uint8_t octet;
    char *ptr=strdup(ip);
    char *byte=strtok(ptr,".");
    for (int i =0; i<4;i++){
        octet=(uint8_t)atoi(byte);
        result=(result<<8) | octet;
        byte=strtok(NULL,".");
    }
    free(ptr);
    return result;
}

int isInSubnet(uint32_t ip_address, uint32_t subnet_ip_address, uint32_t subnet_mask){
    return ((ip_address & subnet_mask)==(subnet_ip_address & subnet_mask));
}

int main(int argc, char *argv[]){
    printf("asas");
    char* ip_address = argv[1];
    char* subnet_mask=argv[2];
    int generate_number = atoi(argv[3]);
    float isInSubnetCount=0.0;
    srand(time(NULL));
    uint32_t ip;
    uint32_t uintip=ip_to_uint32(ip_address);
    uint32_t uintmask=ip_to_uint32(subnet_mask);
    for (int i=0;i<generate_number;i++){
        for (int j=0;j<4;j++){
            uint8_t octet = (uint8_t)(rand()%170);
            ip=(ip<<8)|octet;
        }
        isInSubnetCount+=isInSubnet(ip,uintip,uintmask);
    }
    printf("Amount of ip addresses in subnet: %f\n",isInSubnetCount);
    printf("Percentage of ip addresses in subnet: %f\n",isInSubnetCount/generate_number*100);
    printf("Amount of ip addresses not in subnet: %f\n",generate_number-isInSubnetCount);
    printf("Percentage of ip addresses not in subnet: %f",(generate_number-isInSubnetCount)/generate_number*100);
}