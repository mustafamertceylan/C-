/*
 * ============================================================================
 * PIPE İLE TEK YÖNLÜ İLETİŞİM - TEMEL ÖRNEK
 * ============================================================================
 * 
 * Bu program, bir pipe oluşturup ebeveyn ve çocuk süreç arasında
 * tek yönlü veri iletişimini gösterir.
 * 
 * Ebeveyn → Pipe → Çocuk (veri akışı)
 * 
 * Derleme: gcc -o pipe_basic pipe_basic.c
 * Çalıştır: ./pipe_basic
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];          // pipefd[0] = okuma ucu, pipefd[1] = yazma ucu
    pid_t pid;
    char write_msg[] = "Merhaba Pipe! Bu mesaj ebeveynden geliyor.";
    char read_msg[100];
    
    // 1. Pipe oluştur
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    // 2. fork() ile yeni süreç oluştur
    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        /* ================================================================
         * ÇOCUK SÜREÇ (OKUYUCU)
         * ================================================================ */
        
        // Kullanılmayan yazma ucunu kapat
        close(pipefd[1]);
        
        // Pipe'dan oku
        ssize_t bytes_read = read(pipefd[0], read_msg, sizeof(read_msg) - 1);
        
        if (bytes_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        
        read_msg[bytes_read] = '\0';  // Null sonlandırma
        
        printf("[ÇOCUK] Ebeveynden gelen mesaj: %s\n", read_msg);
        printf("[ÇOCUK] Okunan byte sayısı: %ld\n", bytes_read);
        
        // Okuma ucunu kapat
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
        
    } else {
        /* ================================================================
         * EBEVEYN SÜREÇ (YAZICI)
         * ================================================================ */
        
        // Kullanılmayan okuma ucunu kapat
        close(pipefd[0]);
        
        // Pipe'a yaz
        ssize_t bytes_written = write(pipefd[1], write_msg, strlen(write_msg));
        
        if (bytes_written == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        
        printf("[EBEVEYN] Yazılan mesaj: %s\n", write_msg);
        printf("[EBEVEYN] Yazılan byte sayısı: %ld\n", bytes_written);
        
        // Yazma ucunu kapat (önemli!)
        close(pipefd[1]);
        
        // Çocuğun bitmesini bekle
        wait(NULL);
        
        printf("[EBEVEYN] Çocuk süreç tamamlandı.\n");
    }
    
    return 0;
}