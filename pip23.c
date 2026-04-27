/*
 * ============================================================================
 * PIPE İLE BLOKLAMA DAVRANIŞI (BLOCKING I/O)
 * ============================================================================
 * 
 * Bu program, pipe'ın bloklama (blocking) davranışını gösterir:
 * - Boş bir pipe'tan read() yapmak BLOKLAR (veri gelene kadar bekler)
 * - Dolu bir pipe'a write() yapmak BLOKLAR (okuma yapılana kadar bekler)
 * 
 * Derleme: gcc -o pipe_blocking pipe_blocking.c
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[10];
    
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        /* ================================================================
         * ÇOCUK SÜREÇ - 3 saniye sonra veri yazacak
         * ================================================================ */
        
        close(pipefd[0]);  // Okuma ucunu kapat
        
        printf("[ÇOCUK] Başladım. 3 saniye sonra pipe'a yazacağım...\n");
        sleep(3);
        
        printf("[ÇOCUK] Pipe'a yazıyorum...\n");
        write(pipefd[1], "VERI", 4);
        printf("[ÇOCUK] Yazma tamamlandı.\n");
        
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
        
    } else {
        /* ================================================================
         * EBEVEYN SÜREÇ - HEMEN read() yapacak ve BLOKLANACAK
         * ================================================================ */
        
        close(pipefd[1]);  // Yazma ucunu kapat
        
        time_t start = time(NULL);
        
        printf("[EBEVEYN] Başladım. Pipe'tan okumaya çalışıyorum...\n");
        printf("[EBEVEYN] (Pipe boş olduğu için BLOKLANACAĞIM)\n");
        
        // Bu read() çağrısı, çocuk veri yazana kadar BLOKLANIR!
        ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
        
        time_t end = time(NULL);
        
        buffer[bytes_read] = '\0';
        printf("[EBEVEYN] Okunan veri: %s\n", buffer);
        printf("[EBEVEYN] Bekleme süresi: %ld saniye\n", end - start);
        
        close(pipefd[0]);
        wait(NULL);
    }
    
    return 0;
}