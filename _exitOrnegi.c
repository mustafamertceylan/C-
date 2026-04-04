#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void cleanup() {
    printf("Bu mesaj child'da görünmeyecek ama parent'dan sonra gözükür\n");
}

int main() {
    atexit(cleanup);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Çocuk process
        printf("Child process çalışıyor\n");
        _exit(0);  // atexit() çağrılmaz! Temizlik yapılmaz
    } else {
        // Ebeveyn process
        wait(NULL);
        printf("Parent process bitti\n");
    }
    
    return 0;
}