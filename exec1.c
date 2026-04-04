#include <unistd.h>
#include <stdio.h>

int main() {
    printf("Bu mesaj exec() ÖNCESİ\n");
    
    execl("/bin/ls", "ls", "-l", NULL);
    
    // Bu satırlar sadece exec() BAŞARISIZ olursa çalışır!
    printf("Bu mesaj exec() SONRASI - HATA durumunda!\n");
    perror("exec");
    
    return 1;
}