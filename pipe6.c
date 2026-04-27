/*
 * ============================================================================
 * PIPE İLE VERİ AKIŞINI GÖRSELLEŞTİRME
 * ============================================================================
 * 
 * Bu program, ebeveyn ve çocuk arasındaki veri akışını
 * zaman damgalarıyla gösterir.
 * 
 * Derleme: gcc -o pipe_visual pipe_visual.c
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

void print_timestamp(const char *label) {
    time_t now;
    time(&now);
    printf("[%ld] %s\n", now, label);
}

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[100];
    
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    print_timestamp("Program başladı");
    
    pid = fork();
    
    if (pid == 0) {
        /* ÇOCUK */
        close(pipefd[1]);
        
        print_timestamp("[ÇOCUK] read() çağrısı yapıyorum (bloklanacağım)");
        
        ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);
        buffer[n] = '\0';
        
        print_timestamp("[ÇOCUK] Veri aldım!");
        printf("[ÇOCUK] Alınan mesaj: %s\n", buffer);
        
        close(pipefd[0]);
        exit(0);
        
    } else {
        /* EBEVEYN */
        close(pipefd[0]);
        
        print_timestamp("[EBEVEYN] 2 saniye bekleyip yazacağım");
        sleep(2);
        
        print_timestamp("[EBEVEYN] Pipe'a yazıyorum");
        write(pipefd[1], "Hello from Parent!", 18);
        
        close(pipefd[1]);
        wait(NULL);
        
        print_timestamp("[EBEVEYN] Tamamlandı");
    }
    
    return 0;
}