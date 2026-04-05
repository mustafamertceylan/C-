#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    char buffer[100];
    
    pipe(pipefd);  // Pipe oluştur
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // ÇOCUK: pipe'a yaz
        close(pipefd[0]);  // Okuma ucunu kapat
        write(pipefd[1], "Merhaba Parent!", 16);
        close(pipefd[1]);
    } else {
        // EBEVEYN: pipe'dan oku
        close(pipefd[1]);  // Yazma ucunu kapat
        read(pipefd[0], buffer, sizeof(buffer));
        printf("Parent şunu aldı: %s\n", buffer);
        close(pipefd[0]);
        wait(NULL);
    }
    
    return 0;
}