#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <mqueue.h>

#define QUEUE_NAME "/my_queue"
#define MSG_SIZE 100
#define PRIOR 1
#define TERM_PRIOR 255


int main (int argc, char *argv[]){
    mqd_t qd;
    int prio;
    char text[MSG_SIZE];
    char newtext[MSG_SIZE];
    struct mq_attr queue_attr;
    queue_attr.mq_maxmsg = 32;
    queue_attr.mq_msgsize=MSG_SIZE;
    if ((qd=mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0600, &queue_attr))==(mqd_t)-1){
        perror ("Error mq_open");
        exit(EXIT_FAILURE);
    }
    while (1){
        fgets (text, MSG_SIZE, stdin);
        if (strncmp(text,"exit",4)==0){
            printf("Received termination message\n");
            if(mq_send(qd,text,strlen(text),TERM_PRIOR)){
                perror("Error msgsnd");
                exit(EXIT_FAILURE);
            }
            break;
        }
        if (mq_send(qd,text,strlen(text),PRIOR)){
            perror ("Error mq_send");
            exit(EXIT_FAILURE);
        }
        printf("Message sent: %s\nPriority: %d\n",text,PRIOR);
        if (mq_receive(qd, newtext, MSG_SIZE, &prio) == -1){
            perror("Error mq_receive");
            return -1;
        }
        if (prio==TERM_PRIOR){
            printf("\nReceived termination message\n");
            break;
        }
        printf("Received: %s\nPriority: %d\n",newtext,prio);
    }
    if (mq_close(qd) == -1)
        perror("Error mq_close");
  
    return 0;
}
