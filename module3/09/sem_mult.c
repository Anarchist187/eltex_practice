#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define READERS_MAX_COUNT 4
#define FILE_NAME "file.txt"
#define LOG_NAME "log.txt"
#define SEM_NAME "my_sem"

void sem_lock(int semid, int semnum) {
    struct sembuf op = {semnum, -1, SEM_UNDO};
    if (semop(semid, &op, 1) == -1) {
        perror("semop wait");
        exit(EXIT_FAILURE);
    }
}
void sem_unlock(int semid, int semnum) {
    struct sembuf op = {semnum, 1, SEM_UNDO};
    if (semop(semid, &op, 1) == -1) {
        perror("semop wait");
        exit(EXIT_FAILURE);
    }
}

void reader_lock(int semid) {
    sem_lock(semid, 1);
    sem_unlock(semid, 1);
    sem_lock(semid, 0);
    if (semctl(semid, 2, GETVAL) == 0) {
        sem_lock(semid, 1);
    }
    sem_unlock(semid, 2); 
    sem_unlock(semid, 0);
}

void reader_unlock(int semid) {
    sem_lock(semid, 0);
    sem_lock(semid, 2);
    if (semctl(semid, 2, GETVAL) == 0) {
        sem_unlock(semid, 1);
    }

    sem_unlock(semid, 0);
}

int main (int argc, char *argv[]){
    int fd,fd_log, num, semid,status,num_count,reader_count; 
    pid_t pid;
    int pipe_fd[2];
    char buf[300];
    char log_ms[300];

    fd_log = open(LOG_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_log==-1){
        perror("Cannot open log");
        exit(EXIT_FAILURE);
    }
    fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd==-1){
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }
    close(fd);

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <reader_count> <amount_of_numbers>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (atoi(argv[1]) > READERS_MAX_COUNT){
        fprintf(stderr, "Too many readers, max readers: %d\n", READERS_MAX_COUNT);
        exit(EXIT_FAILURE);
    }
    reader_count = atoi(argv[1]);
    num_count = atoi(argv[2]);
    if (reader_count <= 0 || num_count <= 0) {
        fprintf(stderr, "Counts must be positive\n");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipe_fd)==-1){
        perror("Error creating pipe");
        close (fd_log);
        exit(EXIT_FAILURE);
    }
    key_t key = ftok("mysem",187);
    semid=semget(key, 3, 0666 | IPC_CREAT);
    if (semid==-1){
        perror("Error semget");
        exit(EXIT_FAILURE);
    }
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1 || semctl(semid, 1, SETVAL, arg) == -1) {
        perror("semctl SETVAL");
        close(fd_log);
        exit(EXIT_FAILURE);
    }
    arg.val = 0;
    if (semctl(semid, 2, SETVAL, arg) == -1) {
        perror("semctl SETVAL");
        close(fd_log);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < reader_count; i++) {
        pid=fork();
        if (pid==-1){
            perror("fork reader");
            continue;
        }
        if(pid==0){
            close(pipe_fd[0]);
            srand(time(NULL) ^ getpid());
            for (int j = 0; j < num_count/reader_count+1; j++) {
                sleep(rand() % 3 + 1);
                reader_lock(semid);
                snprintf(log_ms,sizeof(log_ms),"Reader %d (PID=%d) blocked access\n",i+1,getpid());
                write(fd_log,log_ms,strlen(log_ms));
                fd = open(FILE_NAME, O_RDONLY);
                if (fd == -1) {
                    perror("reader open");
                    exit(EXIT_FAILURE);
                }
                ssize_t bytes_read = read(fd, buf, sizeof(buf) - 1);
                close(fd);
                if (bytes_read > 0) {
                    buf[bytes_read] = '\0';
                    snprintf(log_ms, sizeof(log_ms), "Reader %d (PID=%d) read: \n%s\n", i+1, getpid(), buf);
                    write(fd_log,log_ms,strlen(log_ms));
                }
                snprintf(log_ms, sizeof(log_ms), "Reader %d (PID=%d) unlocked access\n", i+1, getpid());
                write(fd_log,log_ms,strlen(log_ms));
                reader_unlock(semid);
                num=rand()%10000;
                write(pipe_fd[1],&num,sizeof(num));
                snprintf(log_ms,sizeof(log_ms),"Reader %d (PID=%d) sent: %d\n",i+1, getpid(), num);
                write(fd_log,log_ms,strlen(log_ms));
            }
            close(fd_log);
            close(pipe_fd[1]);
            exit(EXIT_SUCCESS);
        }
    }
        if(pid>0){
            close (pipe_fd[1]);
            fd=open(FILE_NAME,O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd==-1){
                perror("Error opening file");
                close(fd_log);
                exit(EXIT_FAILURE);
            }
            for (int i=0;i<num_count;i++){
                read(pipe_fd[0],&num,sizeof(num));
                snprintf(log_ms,sizeof(log_ms),"Parent received: %d\n",num);
                write(fd_log,log_ms,strlen(log_ms));
                sem_lock(semid,1);
                snprintf(log_ms,sizeof(log_ms),"Parent blocked file access\n");
                write(fd_log,log_ms,strlen(log_ms));
                int msg_len=snprintf(buf,sizeof(buf),"%d\n",num);
                write(fd,buf,msg_len);
                sem_unlock(semid,1);
                snprintf(log_ms,sizeof(log_ms),"Parent unblocked file access\n");
                write(fd_log,log_ms,strlen(log_ms));
            }
            close(pipe_fd[0]);
            close(fd);
            wait(&status);
            printf("Parent: return status for Child: %d\n",WEXITSTATUS(status));
        }
}
        
        

