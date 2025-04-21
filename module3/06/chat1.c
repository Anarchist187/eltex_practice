#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define MSG_SIZE 100
#define TERM_PRIOR 100

struct message{
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main(int argc, char *argv[]){
    int i;
    char rcvmsg[MSG_SIZE];
    ssize_t bytes;
    key_t key=ftok("myqueue",187);
    int msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1){
        perror ("Error msgget");
        exit(EXIT_FAILURE);
    }
    struct message msg;
    msg.msg_type = 1;
    while (1){
        printf("User1. Введите сообщение: ");
        fgets (msg.msg_text, MSG_SIZE, stdin);

        if (strncmp(msg.msg_text,"exit",4)==0){
            printf("Received termination message\n");
            msg.msg_type=TERM_PRIOR;
            if(msgsnd(msqid,&msg,sizeof(msg.msg_text),0)==-1){
                perror("Error msgsnd");
                exit(EXIT_FAILURE);
            }
            break;
        }

        if(msgsnd(msqid,&msg,sizeof(msg.msg_text),0)==-1){
            perror("Error msgsnd");
            exit(EXIT_FAILURE);
        }
        printf("\nMessage sent: %s",msg.msg_text);
        bytes=msgrcv(msqid,&msg,sizeof(msg.msg_text),0,0);

        if(bytes==-1){
            perror("Error msgrcv");
            exit(EXIT_FAILURE);
        }

        printf("Got message: %s",msg.msg_text);
        if (strncmp(msg.msg_text,"exit",4)==0){
            printf("Received termination message\n");
            break;
        }
    } 
    msgctl(msqid,IPC_RMID,NULL);
}