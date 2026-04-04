/**
 * ============================================================================
 * BUFFERING DEMO - C I/O Tamponlama Politikaları Örneği
 * ============================================================================
 * 
 * Bu program, üç farklı tamponlama politikasını (unbuffered, line buffered,
 * fully buffered) ve bunların ekrana/dosyaya çıktı verme davranışlarını 
 * gösterir.
 * 
 * Derleme:  gcc -o buffering_demo buffering_demo.c
 * Çalıştır: ./buffering_demo
 * 
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* ANSI renk kodları (daha iyi görsel ayrım için) */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

/* Yardımcı fonksiyon: çizgi çiz */
void print_separator(const char *title) {
    printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════" COLOR_RESET "\n");
    if (title) {
        printf(COLOR_CYAN "  %s\n" COLOR_RESET, title);
        printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════" COLOR_RESET "\n");
    }
}

/* Yardımcı fonksiyon: bekleme (saniye cinsinden) */
void wait_seconds(int seconds) {
    printf(COLOR_YELLOW "  [%d saniye bekleniyor...]" COLOR_RESET "\n", seconds);
    fflush(stdout);  /* Beklemeden önce buffer'ı temizle */
    sleep(seconds);
}

/* ============================================================================
 * BÖLÜM 1: UNBUFFERED (Tamponlanmamış) - stderr örneği
 * ============================================================================
 * Unbuffered: Her yazma işlemi HEMEN sistem çağrısına dönüşür.
 *             Hata mesajları için idealdir (hemen görünmeli).
 * ============================================================================
 */
void demo_unbuffered(void) {
    print_separator("1. UNBUFFERED (Tamponlanmamış) - stderr");
    
    printf(COLOR_BLUE "  stderr (hata çıktısı) unbuffered'dır. Her karakter hemen ekrana yazılır.\n" COLOR_RESET);
    printf(COLOR_BLUE "  Aşağıdaki mesajlar yazıldıktan HEMEN sonra ekranda görünecektir:\n\n" COLOR_RESET);
    
    fprintf(stderr, COLOR_RED "  H>");
    wait_seconds(1);
    
    fprintf(stderr, "e");
    wait_seconds(1);
    
    fprintf(stderr, "l");
    wait_seconds(1);
    
    fprintf(stderr, "l");
    wait_seconds(1);
    
    fprintf(stderr, "o");
    wait_seconds(1);
    
    fprintf(stderr, "!");
    wait_seconds(1);
    
    fprintf(stderr, "\n" COLOR_RESET);
    
    printf(COLOR_GREEN "\n  ✓ Görüldüğü gibi, her karakter yazıldıktan hemen sonra ekranda belirdi.\n" COLOR_RESET);
    printf(COLOR_GREEN "  ✓ Bu davranış, hata mesajlarının hemen görünmesini sağlar.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 2: LINE BUFFERED (Satır Tamponlamalı) - stdout (terminal) örneği
 * ============================================================================
 * Line Buffered: Veriler, '\n' (newline) görülene kadar buffer'da bekler.
 *                Terminal çıktıları için varsayılan politikadır.
 * ============================================================================
 */
void demo_line_buffered(void) {
    print_separator("2. LINE BUFFERED (Satır Tamponlamalı) - stdout");
    
    printf(COLOR_BLUE "  stdout (normal çıktı) terminale yazarken line buffered'dır.\n" COLOR_RESET);
    printf(COLOR_BLUE "  Karakterler hemen görünmez, '\\n' görülene veya buffer dolana kadar bekler.\n\n" COLOR_RESET);
    
    printf(COLOR_MAGENTA "  Yazılıyor: " COLOR_RESET);
    fflush(stdout);  /* İlk mesajın hemen görünmesi için */
    
    printf("H");
    wait_seconds(1);
    printf("e");
    wait_seconds(1);
    printf("l");
    wait_seconds(1);
    printf("l");
    wait_seconds(1);
    printf("o");
    wait_seconds(1);
    
    printf(COLOR_YELLOW "\n\n  Şimdi '\\n' (newline) ekleniyor...\n" COLOR_RESET);
    wait_seconds(1);
    
    printf(" Dünya!\n");  /* <-- BU SATIRDA \n VAR! */
    
    printf(COLOR_GREEN "\n  ✓ '\\n' görüldüğünde buffer flush oldu ve tüm mesaj birden göründü!\n" COLOR_RESET);
    printf(COLOR_GREEN "  ✓ Line buffered, satır satır çıktı almak istediğimizde idealdir.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 3: FULLY BUFFERED (Tam Tamponlamalı) - Dosya yazma örneği
 * ============================================================================
 * Fully Buffered: Veriler, buffer TAMAMEN DOLANA kadar bekler.
 *                 Dosya I/O için varsayılan politikadır.
 * ============================================================================
 */
void demo_fully_buffered(void) {
    print_separator("3. FULLY BUFFERED (Tam Tamponlamalı) - Dosya Yazma");
    
    printf(COLOR_BLUE "  Dosyaya yazma işlemleri fully buffered'dır.\n" COLOR_RESET);
    printf(COLOR_BLUE "  Veriler, buffer dolana veya dosya kapatılana kadar diskte görünmez.\n\n" COLOR_RESET);
    
    const char *filename = "fully_buffered_output.txt";
    FILE *fp = fopen(filename, "w");
    
    if (!fp) {
        perror("Dosya açılamadı");
        return;
    }
    
    printf(COLOR_MAGENTA "  Dosyaya yazılıyor: " COLOR_RESET);
    
    fprintf(fp, "Bu yazı ");
    printf("1. yazı yapıldı (dosyada henüz yok!)");
    wait_seconds(1);
    
    fprintf(fp, "henüz ");
    printf(" -> 2. yazı yapıldı");
    wait_seconds(1);
    
    fprintf(fp, "diskette ");
    printf(" -> 3. yazı yapıldı");
    wait_seconds(1);
    
    fprintf(fp, "görünmüyor!");
    printf(" -> 4. yazı yapıldı\n");
    wait_seconds(1);
    
    printf(COLOR_YELLOW "\n  Şimdi 'cat' komutuyla dosyayı kontrol edelim:\n" COLOR_RESET);
    fflush(stdout);
    
    char command[256];
    snprintf(command, sizeof(command), "cat %s 2>/dev/null || echo '(Dosya boş veya henüz yazılmamış)'", filename);
    system(command);
    
    wait_seconds(1);
    
    printf(COLOR_YELLOW "\n  Şimdi dosyayı kapatıyoruz (fclose)...\n" COLOR_RESET);
    fclose(fp);  /* <-- BU SATIRDA buffer FLUSH olur! */
    
    printf(COLOR_YELLOW "\n  Dosyayı tekrar kontrol edelim:\n" COLOR_RESET);
    system(command);
    
    printf(COLOR_GREEN "\n  ✓ Veriler, fclose() yapılana kadar diskte görünmedi!\n" COLOR_RESET);
    printf(COLOR_GREEN "  ✓ Fully buffered, disk I/O sayısını azaltarak performansı artırır.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 4: fflush() ile Zorla Flush Etme
 * ============================================================================
 * fflush(): Buffer'ı zorla boşaltır (verileri hemen yazar).
 * ============================================================================
 */
void demo_fflush(void) {
    print_separator("4. fflush() ile Zorla Flush Etme");
    
    printf(COLOR_BLUE "  fflush() fonksiyonu, buffer'ı zorla boşaltır (flush).\n" COLOR_RESET);
    printf(COLOR_BLUE "  Normalde line buffered'da '\\n' olmadan çıktı görünmez, ancak fflush ile hemen görünür.\n\n" COLOR_RESET);
    
    printf(COLOR_MAGENTA "  Yazılıyor (\\n yok, fflush kullanılıyor): " COLOR_RESET);
    printf("Merhaba");
    fflush(stdout);  /* <-- BU SATIR ZORLA FLUSH YAPAR! */
    
    wait_seconds(2);
    
    printf(" Dünya!\n");
    
    printf(COLOR_GREEN "\n  ✓ fflush() sayesinde '\\n' olmamasına rağmen çıktı hemen göründü!\n" COLOR_RESET);
    printf(COLOR_GREEN "  ✓ fflush(), özellikle ilerleme göstergeleri (progress bar) için kullanılır.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 5: setvbuf() ile Politika Değiştirme
 * ============================================================================
 * setvbuf(): Bir akışın (stream) tamponlama politikasını değiştirir.
 * ============================================================================
 */
void demo_setvbuf(void) {
    print_separator("5. setvbuf() ile Politika Değiştirme");
    
    printf(COLOR_BLUE "  setvbuf() ile stdout'un tamponlama politikasını değiştirebiliriz.\n" COLOR_RESET);
    printf(COLOR_BLUE "  Önce stdout'u UNBUFFERED yapalım:\n\n" COLOR_RESET);
    
    /* stdout'u unbuffered yap */
    setvbuf(stdout, NULL, _IONBF, 0);
    
    printf(COLOR_MAGENTA "  Şimdi stdout UNBUFFERED! Her karakter hemen görünecek: " COLOR_RESET);
    
    printf("A");
    wait_seconds(1);
    printf("B");
    wait_seconds(1);
    printf("C");
    wait_seconds(1);
    printf("D");
    wait_seconds(1);
    printf("E\n");
    
    printf(COLOR_GREEN "\n  ✓ Her karakter yazıldıktan hemen sonra göründü!\n" COLOR_RESET);
    
    /* stdout'u tekrar line buffered yap */
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf(COLOR_BLUE "\n  stdout tekrar LINE BUFFERED yapıldı.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 6: fork() ve Buffer İlişkisi (Kritik!)
 * ============================================================================
 * fork() öncesi buffer'da veri varsa, bu buffer child process'e de kopyalanır!
 * ============================================================================
 */
void demo_fork_buffering(void) {
    print_separator("6. fork() ve Buffer İlişkisi (Kritik!)");
    
    printf(COLOR_BLUE "  fork() yapılmadan önce buffer'da veri varsa, bu buffer CHILD'a da KOPYALANIR!\n" COLOR_RESET);
    printf(COLOR_BLUE "  Bu, beklenmedik çıktılara neden olabilir.\n\n" COLOR_RESET);
    
    printf(COLOR_MAGENTA "  fork() öncesi buffer'a yazılıyor (\\n yok): " COLOR_RESET);
    printf("BUFFERDA_BEKLİYOR_");
    fflush(stdout);  /* İlk mesajın hemen görünmesi için */
    
    printf("DEVAM_EDİYOR");
    /* Burada \n yok! Buffer'da "BUFFERDA_BEKLİYOR_DEVAM_EDİYOR" bekliyor */
    
    printf(COLOR_YELLOW "\n\n  Şimdi fork() yapılıyor...\n" COLOR_RESET);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        /* Child process */
        printf(COLOR_RED "  [CHILD] Çıktı: %s", "CHILD YAZIYOR\n" COLOR_RESET);
    } else {
        /* Parent process */
        wait_seconds(1);
        printf(COLOR_GREEN "  [PARENT] Çıktı: %s", "PARENT YAZIYOR\n" COLOR_RESET);
    }
    
    printf(COLOR_GREEN "\n  ✓ Eğer buffer'da bekleyen veri varsa, hem parent hem child bu veriyi yazdırır!\n" COLOR_RESET);
    printf(COLOR_GREEN "  ✓ Bu nedenle fork() öncesi fflush() yapmak iyi bir pratiktir.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 7: Buffer Boyutunu Gösterme
 * ============================================================================
 */
void demo_buffer_size(void) {
    print_separator("7. Buffer Boyutları");
    
    printf(COLOR_BLUE "  Sistemdeki varsayılan buffer boyutları:\n\n" COLOR_RESET);
    printf("  BUFSIZ (stdio buffer boyutu): " COLOR_YELLOW "%d" COLOR_RESET " byte\n", BUFSIZ);
    printf("  stdout buffer boyutu: " COLOR_YELLOW "%zu" COLOR_RESET " byte\n", stdout->_IO_buf_end - stdout->_IO_buf_base);
    
    printf(COLOR_BLUE "\n  Not: Buffer boyutları sisteme ve derleyiciye göre değişebilir.\n" COLOR_RESET);
}

/* ============================================================================
 * ANA FONKSİYON
 * ============================================================================
 */
int main(int argc, char *argv[]) {
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║         C I/O TAMPONLAMA (BUFFERING) POLİTİKALARI DEMO            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    printf(COLOR_YELLOW "\nBu demo, üç temel tamponlama politikasını gösterir:\n");
    printf("  • " COLOR_RESET "Unbuffered    - Her yazma hemen sistem çağrısına dönüşür\n");
    printf("  • " COLOR_RESET "Line Buffered - '\\n' görüldüğünde veya buffer dolduğunda yazılır\n");
    printf("  • " COLOR_RESET "Fully Buffered- Sadece buffer dolduğunda yazılır\n\n");
    
    /* Tüm demoları çalıştır */
    demo_unbuffered();
    demo_line_buffered();
    demo_fully_buffered();
    demo_fflush();
    demo_setvbuf();
    demo_fork_buffering();
    demo_buffer_size();
    
    print_separator("DEMO SONU");
    printf(COLOR_GREEN "\n  Program başarıyla tamamlandı!\n" COLOR_RESET);
    printf(COLOR_YELLOW "  Not: Bazı çıktılar terminalinize ve sisteminize bağlı olarak farklılık gösterebilir.\n" COLOR_RESET);
    
    return 0;
}