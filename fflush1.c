#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Yükleniyor");
    fflush(stdout);     // HEMEN ekrana yaz!
    sleep(3);           
    printf(".\n");
    
    return 0;
}