/*
 * ============================================================================
 * dup2() SİSTEM ÇAĞRISI - DETAYLI ÖRNEK
 * ============================================================================
 * 
 * Bu program, dup2() ile standart çıktıyı (stdout, fd=1) bir dosyaya
 * yönlendirir. Böylece printf() gibi fonksiyonlar ekran yerine dosyaya yazar.
 * 
 * Derleme: gcc -o dup2_demo dup2_demo.c
 * Çalıştır: ./dup2_demo
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main() {
    int fd;
    int saved_stdout;
    
    /* ======================================================================
     * 1. Önce stdout'un bir yedeğini alalım (daha sonra geri yüklemek için)
     * ====================================================================== */
    saved_stdout = dup(1);  // stdout'un (fd=1) bir kopyasını al
    if (saved_stdout == -1) {
        perror("dup");
        exit(EXIT_FAILURE);
    }
    printf("stdout yedeği alındı. Yedek fd: %d\n", saved_stdout);
    
    /* ======================================================================
     * 2. Bir dosya aç (yoksa oluştur)
     * ====================================================================== */
    fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("Dosya açıldı. fd: %d\n", fd);
    
    /* ======================================================================
     * 3. dup2() ile stdout'u (fd=1) dosyaya yönlendir
     * ====================================================================== */
    printf("\n--- dup2(fd, 1) çağrılıyor... ---\n");
    if (dup2(fd, 1) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    printf("Artık stdout dosyaya yönlendirildi!\n");
    printf("(Bu mesaj EKRANA değil, DOSYAYA yazılacak!)\n");
    
    /* ======================================================================
     * 4. Şimdi yapılan tüm printf'ler output.txt dosyasına gidecek
     * ====================================================================== */
    printf("\n=== BU SATIRLAR output.txt DOSYASINA YAZILACAK ===\n");
    printf("dup2() ile stdout başarıyla yönlendirildi.\n");
    printf("Artık ekranda görmüyorsunuz, ama dosyada var!\n");
    printf("=================================================\n");
    
    /* ======================================================================
     * 5. Dosyayı kapat (ama stdout hala dosyayı gösteriyor!)
     * ====================================================================== */
    close(fd);  // fd'yi kapatmak, stdout'u (fd=1) etkilemez! Çünkü aynı dosyayı gösteriyorlar
    printf("Dosya tanımlayıcısı kapatıldı, ancak stdout hala dosyayı gösteriyor.\n");
    
    /* ======================================================================
     * 6. stdout'u eski haline döndür
     * ====================================================================== */
    printf("\n--- dup2(saved_stdout, 1) ile stdout geri yükleniyor... ---\n");
    if (dup2(saved_stdout, 1) == -1) {
        perror("dup2 restore");
        exit(EXIT_FAILURE);
    }
    close(saved_stdout);
    
    /* ======================================================================
     * 7. Artık çıktılar tekrar EKRANA gelecek
     * ====================================================================== */
    printf("\n=== BU SATIRLAR TEKRAR EKRANA YAZILACAK ===\n");
    printf("stdout başarıyla geri yüklendi!\n");
    printf("Şimdi 'output.txt' dosyasını kontrol edin.\n");
    printf("============================================\n");
    
    return 0;
}