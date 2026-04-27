/*
 * ============================================================================
 * PIPE KAPASİTESİNİ ÖLÇME
 * ============================================================================
 * 
 * Bu program, pipe'ın tampon (buffer) kapasitesini ölçer.
 * Tipik olarak Linux'ta pipe kapasitesi 65536 byte (64 KB)'dır.
 * 
 * Derleme: gcc -o pipe_capacity pipe_capacity.c
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main() {
    int pipefd[2];
    char data[1024];  // Her seferinde 1 KB yaz
    ssize_t total_written = 0;
    int status;
    
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    // Non-blocking moda al (kapasite dolunca hata alsın)
    int flags = fcntl(pipefd[1], F_GETFL, 0);
    fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);
    
    printf("Pipe kapasitesi ölçülüyor...\n");
    printf("(Her blok 1 KB = 1024 byte)\n\n");
    
    while (1) {
        ssize_t written = write(pipefd[1], data, sizeof(data));
        
        if (written == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("\n!!! PIPE DOLDU !!!\n");
                printf("Toplam yazılan: %ld byte (%.2f KB)\n", 
                       total_written, total_written / 1024.0);
                break;
            } else {
                perror("write");
                break;
            }
        }
        
        total_written += written;
        
        if (total_written % (10 * 1024) == 0) {
            printf("Yazılan: %ld KB\n", total_written / 1024);
        }
    }
    
    close(pipefd[0]);
    close(pipefd[1]);
    
    return 0;
}