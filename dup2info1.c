/*
 * ============================================================================
 * dup2() ile PIPE VE FORK ÖRNEĞİ
 * ============================================================================
 * 
 * Bu örnekte, ebeveyn ve çocuk süreç arasında pipe ile iletişim kurulurken
 * dup2() kullanılarak standart giriş/çıkış yönlendirilir.
 * 
 * Derleme: gcc -o dup2_pipe dup2_pipe.c
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int pipefd[2];
    pid_t pid;
    
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
        /* ====================================================================
         * ÇOCUK SÜREÇ: stdout'u pipe'ın yazma ucuna yönlendir
         * ==================================================================== */
        
        // Okuma ucunu kapat (çocuk sadece yazacak)
        close(pipefd[0]);
        
        // stdout'u (fd=1) pipe'ın yazma ucuna yönlendir
        if (dup2(pipefd[1], 1) == -1) {
            perror("dup2 child");
            exit(EXIT_FAILURE);
        }
        
        // pipe'ın yazma ucunu da kapat (artık stdout onu gösteriyor)
        close(pipefd[1]);
        
        // Artık printf()'ler pipe'a yazılacak!
        printf("Bu mesaj ebeveyne pipe üzerinden gidecek!\n");
        printf("Çocuk süreç tamamlandı.\n");
        
        exit(EXIT_SUCCESS);
        
    } else {
        /* ====================================================================
         * EBEVEYN SÜREÇ: stdin'i pipe'ın okuma ucuna yönlendir
         * ==================================================================== */
        
        int status;
        
        // Yazma ucunu kapat (ebeveyn sadece okuyacak)
        close(pipefd[1]);
        
        // stdin'i (fd=0) pipe'ın okuma ucuna yönlendir
        if (dup2(pipefd[0], 0) == -1) {
            perror("dup2 parent");
            exit(EXIT_FAILURE);
        }
        
        // pipe'ın okuma ucunu da kapat (artık stdin onu gösteriyor)
        close(pipefd[0]);
        
        // Artık scanf() veya fgets() pipe'dan okuyacak!
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            printf("Ebeveyn okudu: %s", buffer);
        }
        
        wait(&status);
    }
    
    return 0;
}