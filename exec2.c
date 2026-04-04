#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Çocuk process
        printf("Çocuk: exec() öncesi (PID: %d)\n", getpid());
        
        execlp("ls", "ls", "-l", NULL);
        
        // exec başarısız olursa
        perror("execlp");
        return 1;
        
    } else if (pid > 0) {
        // Ebeveyn process
        printf("Ebeveyn: Çocuk bekleniyor...\n");
        wait(NULL);
        printf("Ebeveyn: Çocuk bitti\n");
        
    } else {
        perror("fork");
        return 1;
    }
    
    return 0;
}