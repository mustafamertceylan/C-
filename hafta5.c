#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Program çalışıyor (PID: %d)\n", getpid());
    
    pid_t pid = fork();
    
    if (pid == 0) {
        printf("Çocuk process (PID: %d)\n", getpid());
    } else {
        printf("Ebeveyn process (PID: %d), çocuk PID: %d\n", getpid(), pid);
    }
    
    return 0;
}