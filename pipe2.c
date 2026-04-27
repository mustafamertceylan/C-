/*
 * ============================================================================
 * ÇİFT YÖNLÜ İLETİŞİM - İKİ PIPE İLE
 * ============================================================================
 * 
 * Bu program, iki pipe kullanarak ebeveyn ve çocuk arasında
 * çift yönlü (bidirectional) iletişim kurar.
 * 
 * Ebeveyn → Pipe1 → Çocuk
 * Ebeveyn ← Pipe2 ← Çocuk
 * 
 * Derleme: gcc -o pipe_bidirectional pipe_bidirectional.c
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int parent_to_child[2];   // Ebeveynden çocuğa giden pipe
    int child_to_parent[2];   // Çocuktan ebeveyne giden pipe
    pid_t pid;
    
    // İki pipe oluştur
    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
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
         * ÇOCUK SÜREÇ
         * ================================================================ */
        
        // Kullanılmayan uçları kapat
        close(parent_to_child[1]);   // Ebeveyn → Çocuk pipe'ının yazma ucunu kapat
        close(child_to_parent[0]);   // Çocuk → Ebeveyn pipe'ının okuma ucunu kapat
        
        char buffer[100];
        
        // 1. Ebeveyn'den mesaj al
        ssize_t bytes_read = read(parent_to_child[0], buffer, sizeof(buffer) - 1);
        buffer[bytes_read] = '\0';
        printf("[ÇOCUK] Ebeveynden alınan: %s\n", buffer);
        
        close(parent_to_child[0]);   // Okuma işlemi bitti
        
        // 2. Cevap gönder
        char reply[] = "Merhaba Ebeveyn! Ben çocuğum, mesajını aldım.";
        write(child_to_parent[1], reply, strlen(reply));
        printf("[ÇOCUK] Cevapları gönderdim.\n");
        
        close(child_to_parent[1]);   // Yazma işlemi bitti
        
        exit(EXIT_SUCCESS);
        
    } else {
        /* ================================================================
         * EBEVEYN SÜREÇ
         * ================================================================ */
        
        // Kullanılmayan uçları kapat
        close(parent_to_child[0]);   // Ebeveyn → Çocuk pipe'ının okuma ucunu kapat
        close(child_to_parent[1]);   // Çocuk → Ebeveyn pipe'ının yazma ucunu kapat
        
        // 1. Çocuğa mesaj gönder
        char message[] = "Selam Çocuk! Bu mesajı alabiliyor musun?";
        write(parent_to_child[1], message, strlen(message));
        printf("[EBEVEYN] Çocuğa mesaj gönderildi: %s\n", message);
        
        close(parent_to_child[1]);   // Yazma işlemi bitti
        
        // 2. Çocuktan cevap bekle
        char reply_buffer[200];
        ssize_t bytes_read = read(child_to_parent[0], reply_buffer, sizeof(reply_buffer) - 1);
        reply_buffer[bytes_read] = '\0';
        printf("[EBEVEYN] Çocuktan gelen cevap: %s\n", reply_buffer);
        
        close(child_to_parent[0]);   // Okuma işlemi bitti
        
        // Çocuğun bitmesini bekle
        wait(NULL);
        printf("[EBEVEYN] İletişim tamamlandı.\n");
    }
    
    return 0;
}