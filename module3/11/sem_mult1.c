#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>

#define FILE_NAME "file.txt"
#define LOG_NAME "log.txt"
#define SEM_NAME "/sem"
#define RW_SEM_NAME "/rw_sem"
#define WRITER_SEM_NAME "/writer_sem"
#define BUF_SIZE 512

sem_t *sem;          
sem_t *rw_sem;       
sem_t *writer_sem;  
int *sem_val;    

int log_fd = -1;

void write_log(const char *message) {
    if (log_fd != -1) {
        write(log_fd, message, strlen(message));
    }
}

void reader(int id, int num_iterations) {
    for (int i = 0; i < num_iterations; i++) {
        sem_wait(writer_sem);
        sem_wait(sem);
        (*sem_val)++;
        if (*sem_val == 1) {
            sem_wait(rw_sem);
        }
        sem_post(sem);
        sem_post(writer_sem);
        char log_msg[BUF_SIZE];
        int fd = open(FILE_NAME, O_RDONLY);
        if (fd == -1) {
            perror("Error reader: open");
            exit(EXIT_FAILURE);
        }
        char buf[BUF_SIZE];
        ssize_t bytes_read = read(fd, buf, sizeof(buf) - 1);
        close(fd);
        if (bytes_read > 0) {
            buf[bytes_read] = '\0';
            snprintf(log_msg, sizeof(log_msg), "\nReader %d read: \n%s\n", id, buf);
            write_log(log_msg);
            write_log ("-------------\n");
        }
        sem_wait(sem);
        (*sem_val)--;
        if (*sem_val == 0) {
            sem_post(rw_sem);
        }
        sem_post(sem);
        usleep(rand() % 500000);
    }
}

void writer(int num_iterations) {
    for (int i = 0; i < num_iterations; i++) {
        sem_wait(writer_sem);
        sem_wait(rw_sem);
        char log_msg[BUF_SIZE];
        int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1) {
            perror("Error writer: open");
            exit(EXIT_FAILURE);
        }
        char buf[64];
        int num = rand() % 100;
        int len = snprintf(buf, sizeof(buf), "Writer wrote: %d\n",num);
        write(fd, buf, len);
        close(fd);
        snprintf(log_msg, sizeof(log_msg), "Writer wrote: %d\n",num);
        write_log(log_msg);
        sem_post(rw_sem);
        sem_post(writer_sem);
        usleep(rand() % 500000);
    }
}

int main(int argc, char *argv[]) {
    pid_t pid;
    int status;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <readers> <iterations>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int num_readers = atoi(argv[1]);
    int iterations = atoi(argv[2]);
    if (num_readers <= 0 || iterations <= 0) {
        fprintf(stderr, "All arguments must be positive integers\n");
        exit(EXIT_FAILURE);
    }
    sem_val = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *sem_val = 0;
    sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    rw_sem = sem_open(RW_SEM_NAME, O_CREAT, 0644, 1);
    writer_sem = sem_open(WRITER_SEM_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED || rw_sem == SEM_FAILED || writer_sem == SEM_FAILED) {
        perror("Error sem_open");
        exit(EXIT_FAILURE);
    }
    log_fd = open(LOG_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (log_fd == -1) {
        perror("Error open log");
        exit(EXIT_FAILURE);
    }
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error open file");
        exit(EXIT_FAILURE);
    }
    close(fd);
    for (int i = 0; i < num_readers; i++) {
        pid = fork();
        if (pid < 0) {
                perror("Error fork");
                exit(EXIT_FAILURE);
        } else if (pid==0){
            srand(time(NULL) ^ getpid());
            reader(i + 1, iterations);
            exit(EXIT_SUCCESS);
        } 
    }
        if (pid > 0) {
        srand(time(NULL) ^ getpid());
        writer(iterations);
        wait(&status);
        printf("Parent: return status for Child: %d\n",WEXITSTATUS(status));
    }
    sem_close(sem);
    sem_close(rw_sem);
    sem_close(writer_sem);
    sem_unlink("/sem");
    sem_unlink("/rw_sem");
    sem_unlink("/writer_sem");
    munmap(sem_val, sizeof(int));
    close(log_fd);
}