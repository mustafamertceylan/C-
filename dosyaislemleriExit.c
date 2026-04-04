#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void cleanup() {
    printf("Program temizleniyor...\n");
    // Geçici dosyaları sil, log yaz, vs.
}

int main(int argc, char *argv[]) {
    atexit(cleanup);
    
    if (argc < 2) {
        fprintf(stderr, "Hata: Dosya adı belirtilmedi!\n");
        exit(EXIT_FAILURE);  // Program burada sonlanır, cleanup çalışır
    }
    
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);  // Hata durumunda sonlan
    }
    
    // Dosya ile işlemler...
    
    close(fd);
    return EXIT_SUCCESS;
}