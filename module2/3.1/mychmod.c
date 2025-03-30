#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

void int_to_bin(unsigned int in, int count, int* out) {
    unsigned int mask = 1U << (count - 1);
    int i;
    for (i = 0; i < count; i++) {
        out[i] = (in & mask) ? 1 : 0;
        in <<= 1;
    }
}

void countRightsBin(int str, char* out) {
    int bin[3];
    int cnt=0;
    for (int i = 0; i < 3; i++) {
        int_to_bin(str % 10, 3, bin);
        for (int j = 0; j < 3; j++)
        {
            out[cnt] = bin[j] ? '1' : '0';
            cnt++;
        }
        str /= 10;
    }
}

void countBinRightsFromChar(char* str, char* out){
    const char *template="rwx";
    for (int i=0;i<9;i++)
        out[i]=(strchr(template,str[i])) ? '1' : '0';
}

void statToBinSymb (mode_t rights, char* outBin, char* outSymb)
{
    if (rights & S_IRUSR)  {outSymb[0]='r'; outBin[0]='1';} else {outSymb[0]='-'; outBin[0]='0';} 
    if (rights & S_IWUSR)  {outSymb[1]='w'; outBin[1]='1';} else {outSymb[1]='-'; outBin[1]='0';}
    if (rights & S_IXUSR)  {outSymb[2]='x'; outBin[2]='1';} else {outSymb[2]='-'; outBin[2]='0';}
    if (rights & S_IRGRP)  {outSymb[3]='r'; outBin[3]='1';} else {outSymb[3]='-'; outBin[3]='0';}
    if (rights & S_IWGRP)  {outSymb[4]='w'; outBin[4]='1';} else {outSymb[4]='-'; outBin[4]='0';}
    if (rights & S_IXGRP)  {outSymb[5]='x'; outBin[5]='1';} else {outSymb[5]='-'; outBin[5]='0';}
    if (rights & S_IROTH)  {outSymb[6]='r'; outBin[6]='1';} else {outSymb[6]='-'; outBin[6]='0';}
    if (rights & S_IWOTH)  {outSymb[7]='w'; outBin[7]='1';} else {outSymb[7]='-'; outBin[7]='0';}
    if (rights & S_IXOTH)  {outSymb[8]='x'; outBin[8]='1';} else {outSymb[8]='-'; outBin[8]='0';}
    outSymb[9]=outBin[9]='\0';
}
int statToDec(mode_t rights)
{
    int out=0;
    int res=0;
    res+=(rights & S_IRUSR) ? 4 : 0;
    res+=(rights & S_IWUSR) ? 2 : 0;
    res+=(rights & S_IXUSR) ? 1 : 0;
    out+=res*100; res=0;
    res+=(rights & S_IRGRP) ? 4 : 0;
    res+=(rights & S_IWGRP) ? 2 : 0;
    res+=(rights & S_IXGRP) ? 1 : 0;
    out+=res*10; res=0;
    res+=(rights & S_IROTH) ? 4 : 0;
    res+=(rights & S_IWOTH) ? 2 : 0;
    res+=(rights & S_IXOTH) ? 1 : 0;
    return out+=res;
}

mode_t modifyPerm(mode_t rights,const char* mode){
    if (mode[1]=='+'){
        if (mode[0]=='u' || mode[0]=='a'){
            for (int i=2;mode[i];i++){
                switch (mode[i]){
                    case 'r': rights |= S_IRUSR; break;
                    case 'w': rights |= S_IWUSR; break;
                    case 'x': rights |= S_IXUSR; break;
                }
            }
        }
        if (mode[0]=='g' || mode[0]=='a'){
            for (int i=2;mode[i];i++){
                switch (mode[i]){
                    case 'r': rights |= S_IRGRP; break;
                    case 'w': rights |= S_IWGRP; break;
                    case 'x': rights |= S_IXGRP; break;
                }
            }
        }
        if (mode[0]=='o' || mode[0]=='a'){
            for (int i=2;mode[i];i++){
                switch (mode[i]){
                    case 'r': rights |= S_IROTH; break;
                    case 'w': rights |= S_IWOTH; break;
                    case 'x': rights |= S_IXOTH; break;
                }
            }
        }

    }
    if (mode[1]=='-'){
        if (mode[0]=='u' || mode[0]=='a'){
            for (int i=2;mode[i];i++){
                switch (mode[i]){
                    case 'r': rights &= ~S_IRUSR; break;
                    case 'w': rights &= ~S_IWUSR; break;
                    case 'x': rights &= ~S_IXUSR; break;
                }
            }
        }
        if (mode[0]=='g' || mode[0]=='a'){
            for (int i=2;mode[i];i++){
                switch (mode[i]){
                    case 'r': rights &= ~S_IRGRP; break;
                    case 'w': rights &= ~S_IWGRP; break;
                    case 'x': rights &= ~S_IXGRP; break;
                }
            }
        }
        if (mode[0]=='o' || mode[0]=='a'){
            for (int i=2;mode[i];i++){
                switch (mode[i]){
                    case 'r': rights &= ~S_IROTH; break;
                    case 'w': rights &= ~S_IWOTH; break;
                    case 'x': rights &= ~S_IXOTH; break;
                }
            }
        }

    }
    return rights;
}

int main(int argc, char* argv[]){
    char permBin[10];
    char permSymb[10];
    char *ptr = argv[1];
    char binary_int[10];
    long num;
    num = strtol(ptr, &ptr, 10);
    if (argc == 2){
        if (*ptr == '\0'){
            int UB = atoi(argv[1]);
            countRightsBin(UB, binary_int);
            for (int i=0;i<9;i++)
                printf("%d", binary_int[i]);
        } 
        else {
            if (strlen(argv[1])!=9) {
                struct stat buffer;
                const char* filename = argv[1];
                stat(filename, &buffer);
                statToBinSymb(buffer.st_mode, permBin,permSymb);
                printf("Binary permissions: %s\n", permBin);
                printf("String permissions: %s\n", permSymb);
                printf("Decimal permissions: %d",statToDec(buffer.st_mode));
            }
                else {
                    char binary_char[10];
                    char *perm_ch = argv[1];
                    countBinRightsFromChar(perm_ch, binary_char);
                    for (int i=0;i<9;i++)
                        printf("%d", binary_char[i]);
                }
        }
    }
    char mode[10];
    if (argc == 3){
        struct stat buffer;
        mode_t rights;
        const char* filename = argv[2];
        stat(filename, &buffer);
        statToBinSymb(buffer.st_mode, permBin,permSymb);
        printf("Default string permissions: %s\n", permSymb);
        printf("Default binary permissions: %s\n", permBin);
        printf("Default decimal permissions: %d\n\n",statToDec(buffer.st_mode));
        const char* mode = argv[1];
        rights = modifyPerm(buffer.st_mode, mode);
        statToBinSymb(rights, permBin,permSymb);
        printf("Modified string permissions: %s\n", permSymb);
        printf("Modified binary permissions: %s\n", permBin);
        printf("Modified decimal permissions: %d",statToDec(rights)); 
        }
}