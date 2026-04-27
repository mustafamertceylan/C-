/*
 * ============================================================================
 * NON-BLOCKING PIPE (O_NONBLOCK ile)
 * ============================================================================
 * 
 * Bu program, pipe'ı non-blocking moda alarak read()'in
 * bloklanmadan hemen dönmesini sağlar.
 * 
 * Derleme: gcc -o pipe_nonblocking pipe_nonblocking.c
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main() {
    int pipefd[2];
    char buffer[10];
    
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    // Pipe'ın okuma ucunu NON-BLOCKING yap
    int flags = fcntl(pipefd[0], F_GETFL, 0);
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
    
    printf("Pipe non-blocking moda alındı.\n");
    printf("Pipe'tan okumaya çalışıyorum (pipe boş)...\n");
    
    // Bu read() bloklanmaz, hemen -1 döner
    ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
    
    if (bytes_read == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("Pipe boş! (errno: %s)\n", strerror(errno));
        } else {
            perror("read");
        }
    }
    
    // Şimdi pipe'a yaz
    printf("\nPipe'a yazıyorum...\n");
    write(pipefd[1], "VERI", 4);
    
    // Non-blocking modda tekrar oku - şimdi veri var
    bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
    buffer[bytes_read] = '\0';
    printf("Okunan veri: %s\n", buffer);
    
    close(pipefd[0]);
    close(pipefd[1]);
    
    return 0;
}