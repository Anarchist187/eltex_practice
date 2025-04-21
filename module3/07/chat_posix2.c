#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <mqueue.h>

#define QUEUE_NAME "/my_queue"
#define MSG_SIZE 100
#define PRIOR 2
#define TERM_PRIOR 255


int main (int argc, char *argv[]){
    mqd_t qd;
    int prio;
    char text[MSG_SIZE];
    char newtext[MSG_SIZE];
    struct mq_attr attr, old_attr;
        if ((qd=mq_open(QUEUE_NAME, O_RDWR, 0600, NULL))==(mqd_t)-1){
        perror ("Error mq_open");
        exit(EXIT_FAILURE);
    }
    while (1){
        if (mq_receive(qd, newtext, MSG_SIZE, &prio) == -1){
            perror("Error mq_receive");
            exit(EXIT_FAILURE);
        }
        if (prio==TERM_PRIOR){
            printf("\nReceived termination message\n");
            break;
        }
        printf("Received message: %s\nPriority: %d\n",newtext,prio);
        fgets (text, MSG_SIZE, stdin);
        if (strncmp(text,"exit",4)==0){
            printf("Received termination message\n");
            if(mq_send(qd,text,strlen(text),TERM_PRIOR)){
                perror("Error msgsnd");
                exit(EXIT_FAILURE);
            }
            break;
        }
        if(mq_send(qd,text,strlen(text),PRIOR)){
            perror("Error msgsnd");
            exit(EXIT_FAILURE);
        }
        printf("Message sent: %s\nPriority: %d\n",text,PRIOR);
    }
    if (mq_close(qd) == -1)
        perror("Closing queue error");
    if (mq_unlink(QUEUE_NAME) == -1)
        perror("Removing queue error");
    return 0;
    
}