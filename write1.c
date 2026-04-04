/*
 * Öğrenci: Sistem Programlama Öğrencisi
 * Konu: write() Sistem Çağrısının Derinlemesine Analizi
 * 
 * Bu program, write sistem çağrısının farklı kullanımlarını,
 * davranışlarını ve arka planda yaptığı işlemleri göstermektedir.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

// ANSI renk kodları (görsel ayrım için)
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

/**
 * print_separator - Ayırıcı çizgi yazdırır
 */
void print_separator(char c, int length) {
    for (int i = 0; i < length; i++) {
        putchar(c);
    }
    putchar('\n');
}

/**
 * print_section - Bölüm başlığı yazdırır
 */
void print_section(const char *title) {
    printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_YELLOW "  %s\n" COLOR_RESET, title);
    printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 1: write() Sistem Çağrısının Temel Kullanımları
 * ============================================================================ */

void demo_basic_write(void) {
    print_section("1. write() Sistem Çağrısının Temel Kullanımları");
    
    // 1.1. write ile stdout'a yazma (fd = 1)
    printf(COLOR_GREEN "1.1. write ile stdout'a yazma:\n" COLOR_RESET);
    write(1, "  write() ile stdout'a yazılan mesaj\n", 36);
    
    // 1.2. write ile stderr'e yazma (fd = 2)
    printf(COLOR_GREEN "1.2. write ile stderr'e yazma:\n" COLOR_RESET);
    write(2, "  write() ile stderr'e yazılan hata mesajı\n", 42);
    
    // 1.3. write ile dosyaya yazma
    printf(COLOR_GREEN "1.3. write ile dosyaya yazma:\n" COLOR_RESET);
    int fd = open("write_test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
    } else {
        char data[] = "Bu veri dosyaya write() ile yazıldı\n";
        write(fd, data, sizeof(data) - 1);  // -1: null terminator yazma
        close(fd);
        printf("  'write_test.txt' dosyasına yazıldı.\n");
    }
    
    // 1.4. write ile ham byte yazma (string olmayan veri)
    printf(COLOR_GREEN "\n1.4. write ile ham byte yazma:\n" COLOR_RESET);
    unsigned char bytes[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x0A};  // "Hello\n"
    write(1, bytes, sizeof(bytes));
}

/* ============================================================================
 * BÖLÜM 2: write() Dönüş Değeri (Yazılan Byte Sayısı)
 * ============================================================================ */

void demo_write_return(void) {
    print_section("2. write() Dönüş Değeri");
    
    ssize_t bytes_written;
    char buffer[] = "Merhaba Dünya\n";
    
    // 2.1. Başarılı write
    bytes_written = write(1, buffer, sizeof(buffer) - 1);
    printf(COLOR_GREEN "  Yazılan byte sayısı: %ld\n" COLOR_RESET, bytes_written);
    
    // 2.2. write hata durumu (geçersiz fd)
    printf(COLOR_YELLOW "\n2.2. Hata durumu (geçersiz fd):\n" COLOR_RESET);
    errno = 0;
    bytes_written = write(-1, buffer, 10);
    printf(COLOR_RED "  Dönüş değeri: %ld\n" COLOR_RESET, bytes_written);
    printf(COLOR_RED "  errno: %d (%s)\n" COLOR_RESET, errno, strerror(errno));
    
    // 2.3. Kısmi yazma durumu (sinyal ile kesilme)
    printf(COLOR_YELLOW "\n2.3. Kısmi yazma:\n" COLOR_RESET);
    printf(COLOR_CYAN "  Not: write, her zaman tüm veriyi yazamayabilir!\n" COLOR_RESET);
    printf(COLOR_CYAN "  Örneğin: Sinyal ile kesilirse (EINTR) veya disk dolduysa.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 3: write ve Tamponlama (Buffering - TAMAMEN UNBUFFERED!)
 * ============================================================================ */

void demo_buffering(void) {
    print_section("3. write() ve Tamponlama (UNBUFFERED!)");
    
    printf(COLOR_YELLOW "write() TAMAMEN UNBUFFERED'dır!\n" COLOR_RESET);
    printf(COLOR_YELLOW "Her çağrı HEMEN sistem çağrısına dönüşür.\n\n" COLOR_RESET);
    
    printf(COLOR_GREEN "write ile yazma (her karakter hemen yazılır):\n" COLOR_RESET);
    write(1, "H", 1);
    sleep(1);
    write(1, "e", 1);
    sleep(1);
    write(1, "l", 1);
    sleep(1);
    write(1, "l", 1);
    sleep(1);
    write(1, "o", 1);
    sleep(1);
    write(1, "\n", 1);
    
    printf(COLOR_YELLOW "\nHer karakter arasında 1 saniye beklendi,\n" COLOR_RESET);
    printf(COLOR_YELLOW "her biri hemen ekranda göründü!\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 4: write() ile Dosya Tanımlayıcıları (File Descriptors)
 * ============================================================================ */

void demo_file_descriptors(void) {
    print_section("4. write() ve Dosya Tanımlayıcıları");
    
    printf(COLOR_CYAN "Her program çalıştırıldığında 3 standart dosya tanımlayıcı alır:\n\n" COLOR_RESET);
    printf("  ┌────────────┬───────────────────────┬─────────┐\n");
    printf("  │ Dosya No   │ İsim                  │ Sembol  │\n");
    printf("  ├────────────┼───────────────────────┼─────────┤\n");
    printf("  │ 0          │ Standart Girdi        │ STDIN   │\n");
    printf("  │ 1          │ Standart Çıktı        │ STDOUT  │\n");
    printf("  │ 2          │ Standart Hata         │ STDERR  │\n");
    printf("  └────────────┴───────────────────────┴─────────┘\n");
    
    printf(COLOR_GREEN "\nFarklı dosya tanımlayıcılara yazma:\n" COLOR_RESET);
    write(1, "  STDOUT (fd=1): Bu normal çıktı\n", 31);
    write(2, "  STDERR (fd=2): Bu hata çıktısı\n", 31);
    
    printf(COLOR_YELLOW "\nNot: STDOUT ve STDERR aynı terminale yazsa da,\n" COLOR_RESET);
    printf(COLOR_YELLOW "yönlendirme yaparken farklı davranırlar.\n" COLOR_RESET);
    printf(COLOR_CYAN "  ./program > output.txt 2> error.txt\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 5: write() vs printf() Karşılaştırması
 * ============================================================================ */

void demo_write_vs_printf(void) {
    print_section("5. write() vs printf() Karşılaştırması");
    
    printf(COLOR_CYAN "printf() bir KÜTÜPHANE fonksiyonu, write() bir SİSTEM ÇAĞRISI'dır.\n\n" COLOR_RESET);
    
    printf("┌─────────────────┬────────────────────────┬────────────────────────┐\n");
    printf("│ Özellik         │ printf()               │ write()                │\n");
    printf("├─────────────────┼────────────────────────┼────────────────────────┤\n");
    printf("│ Seviye          │ Kütüphane fonksiyonu   │ Sistem çağrısı         │\n");
    printf("│ Tamponlama      │ Line buffered (stdout) │ UNBUFFERED             │\n");
    printf("│ Format desteği  │ Evet (%%d, %%s, etc.)   │ Hayır (ham byte)       │\n");
    printf("│ Mod geçişi      │ Sadece flush anında    │ Her çağrıda            │\n");
    printf("│ Maliyet         │ Daha düşük (toplu)     │ Daha yüksek (her çağrı)│\n");
    printf("│ Async-signal-safe│ Hayır                  │ Evet                   │\n");
    printf("└─────────────────┴────────────────────────┴────────────────────────┘\n");
    
    // Performans testi
    printf(COLOR_YELLOW "\nPerformans testi (1000 yazma):\n" COLOR_RESET);
    
    // printf ile
    printf(COLOR_CYAN "  printf ile (\\n ile): " COLOR_RESET);
    for (int i = 0; i < 1000; i++) {
        printf("X");
    }
    printf(" -> 1 sistem çağrısı (buffer flush)\n");
    
    // write ile
    printf(COLOR_CYAN "  write ile: " COLOR_RESET);
    for (int i = 0; i < 1000; i++) {
        write(1, "X", 1);
    }
    printf(COLOR_RED " -> 1000 sistem çağrısı! (çok daha yavaş)\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 6: write() ve NULL Karakter ('\0')
 * ============================================================================ */

void demo_null_character(void) {
    print_section("6. write() ve NULL Karakter ('\\0')");
    
    printf(COLOR_CYAN "write() NULL karakteri ('\\0') normal bir byte olarak işler!\n" COLOR_RESET);
    printf(COLOR_CYAN "printf() ise '\\0' gördüğünde durur.\n\n" COLOR_RESET);
    
    // İçinde NULL karakter olan veri
    char data[] = {'A', 'B', 'C', '\0', 'D', 'E', 'F', '\n'};
    
    printf(COLOR_GREEN "write ile yazma: " COLOR_RESET);
    write(1, data, sizeof(data));  // 8 byte yazar (NULL dahil)
    
    printf(COLOR_GREEN "\nprintf ile yazma: " COLOR_RESET);
    printf("%s", data);  // Sadece "ABC" yazar (NULL'da durur)
    printf(COLOR_RED " (NULL'da durdu!)\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 7: write()'ın Arka Plan Çalışması (Sistem Çağrısı Detayları)
 * ============================================================================ */

void demo_behind_the_scenes(void) {
    print_section("7. write()'ın Arka Plan Çalışması");
    
    printf(COLOR_CYAN "write() sistem çağrısının katmanları:\n\n" COLOR_RESET);
    
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ 1. KULLANICI MODU (User Space)                                 │\n");
    printf("│    ┌─────────────────────────────────────────────────────────┐ │\n");
    printf("│    │ write(1, \"Merhaba\\n\", 8);                              │ │\n");
    printf("│    │   ↓                                                     │ │\n");
    printf("│    │ C Kütüphanesi wrapper (glibc)                           │ │\n");
    printf("│    │   - Argümanları kayıtlara yükler                        │ │\n");
    printf("│    │   - Sistem çağrı numarasını ayarlar (rax=1)             │ │\n");
    printf("│    └─────────────────────────────────────────────────────────┘ │\n");
    printf("│                         ↓                                      │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ 2. SİSTEM ÇAĞRISI ARAYÜZÜ (System Call Interface)             │\n");
    printf("│    - 'syscall' talimatı (x86-64)                              │\n");
    printf("│    - Kullanıcı modundan çekirdek moduna geçiş (ring 3 -> 0)   │\n");
    printf("│    - İşlemci, sistem çağrısı işleyicisine atlar              │\n");
    printf("│                         ↓                                      │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ 3. ÇEKİRDEK MODU (Kernel Space)                               │\n");
    printf("│    ┌─────────────────────────────────────────────────────────┐ │\n");
    printf("│    │ sys_write() işleyicisi                                  │ │\n");
    printf("│    │   - fd=1 (stdout) kontrol edilir                        │ │\n");
    printf("│    │   - Kullanıcı buffer'ından veri okunur (copy_from_user)│ │\n");
    printf("│    │   - Dosya veya aygıt sürücüsüne yönlendirilir           │ │\n");
    printf("│    │   - Terminal veya dosyaya yazılır                       │ │\n");
    printf("│    │   - Yazılan byte sayısı döndürülür                      │ │\n");
    printf("│    └─────────────────────────────────────────────────────────┘ │\n");
    printf("│                         ↓                                      │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ 4. DONANIM SEVİYESİ (Hardware)                                │\n");
    printf("│    - Terminal: Karakterler ekranda görünür                    │\n");
    printf("│    - Dosya: Veri diskte saklanır                              │\n");
    printf("└─────────────────────────────────────────────────────────────────┘\n");
}

/* ============================================================================
 * BÖLÜM 8: write() ve Sinyal Güvenliği (Async-Signal-Safe)
 * ============================================================================ */

void demo_signal_safety(void) {
    print_section("8. write() ve Sinyal Güvenliği (Async-Signal-Safe)");
    
    printf(COLOR_CYAN "write() ASYNC-SIGNAL-SAFE'dir!\n" COLOR_RESET);
    printf(COLOR_CYAN "Sinyal işleyicileri (signal handlers) içinde kullanılabilir.\n\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "printf() ise async-signal-safe DEĞİLDİR!\n" COLOR_RESET);
    printf(COLOR_YELLOW "Sinyal işleyicilerinde printf kullanmak undefined behavior'a yol açar.\n\n" COLOR_RESET);
    
    printf(COLOR_GREEN "Örnek: Sinyal işleyicisinde güvenli kullanım\n" COLOR_RESET);
    printf("  void handler(int sig) {\n");
    printf("      write(2, \"Sinyal alındı!\\n\", 15);  // Güvenli\n");
    printf("      // printf(\"Sinyal alındı!\\n\");    // GÜVENSİZ!\n");
    printf("  }\n");
}

/* ============================================================================
 * BÖLÜM 9: write() ve Non-Blocking I/O
 * ============================================================================ */

void demo_non_blocking(void) {
    print_section("9. write() ve Non-Blocking I/O");
    
    printf(COLOR_CYAN "write() varsayılan olarak BLOCKING'dir.\n" COLOR_RESET);
    printf(COLOR_CYAN "Ancak O_NONBLOCK flag'i ile non-blocking yapılabilir.\n\n" COLOR_RESET);
    
    // Non-blocking pipe örneği
    int pipefd[2];
    if (pipe(pipefd) == 0) {
        // Okuma ucunu non-blocking yap
        fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
        
        // Pipe buffer'ını doldurmaya çalış (pipe boyutu genelde 65536 byte)
        char buffer[70000];
        ssize_t written = write(pipefd[1], buffer, sizeof(buffer));
        
        printf(COLOR_GREEN "  Yazılan byte: %ld\n" COLOR_RESET, written);
        
        // Non-blocking okuma dener
        char read_buf[100];
        ssize_t read_bytes = read(pipefd[0], read_buf, sizeof(read_buf));
        
        if (read_bytes == -1 && errno == EAGAIN) {
            printf(COLOR_YELLOW "  Henüz veri yok! (EAGAIN)\n" COLOR_RESET);
        }
        
        close(pipefd[0]);
        close(pipefd[1]);
    }
}

/* ============================================================================
 * BÖLÜM 10: write()'ın x86-64 Assembly Seviyesi
 * ============================================================================ */

void demo_assembly(void) {
    print_section("10. write()'ın x86-64 Assembly Seviyesi");
    
    printf(COLOR_CYAN "write(1, \"Merhaba\\n\", 8) çağrısının assembly karşılığı:\n\n" COLOR_RESET);
    
    printf("  .data\n");
    printf("  msg: db \"Merhaba\\n\", 0\n\n");
    printf("  .text\n");
    printf("  mov rax, 1          ; sistem çağrı numarası (write = 1)\n");
    printf("  mov rdi, 1          ; fd = 1 (stdout)\n");
    printf("  mov rsi, msg        ; buffer adresi\n");
    printf("  mov rdx, 8          ; yazılacak byte sayısı\n");
    printf("  syscall             ; çekirdek moduna geçiş\n");
}

/* ============================================================================
 * BÖLÜM 11: write() Hata Kodları
 * ============================================================================ */

void demo_errors(void) {
    print_section("11. write() Hata Kodları (errno)");
    
    printf(COLOR_CYAN "write() başarısız olduğunda -1 döner ve errno ayarlanır:\n\n" COLOR_RESET);
    
    printf("┌─────────────────┬────────────────────────────────────────────────┐\n");
    printf("│ errno           │ Anlamı                                         │\n");
    printf("├─────────────────┼────────────────────────────────────────────────┤\n");
    printf("│ EBADF           │ fd geçersiz veya yazma için açık değil         │\n");
    printf("│ EINTR           │ Sinyal ile kesildi                             │\n");
    printf("│ EAGAIN          │ Non-blocking modda hemen yazılamaz             │\n");
    printf("│ ENOSPC          │ Disk dolu                                      │\n");
    printf("│ EPIPE           │ Okuma ucu kapalı pipe (SIGPIPE)                │\n");
    printf("│ EFAULT          │ Buffer adresi geçersiz                         │\n");
    printf("│ EIO             │ G/Ç hatası                                     │\n");
    printf("└─────────────────┴────────────────────────────────────────────────┘\n");
}

/* ============================================================================
 * BÖLÜM 12: write() ile İleri Düzey Kullanımlar
 * ============================================================================ */

void demo_advanced(void) {
    print_section("12. write() ile İleri Düzey Kullanımlar");
    
    // 12.1. writev (scatter/gather I/O)
    printf(COLOR_GREEN "12.1. writev() - Scatter/Gather I/O:\n" COLOR_RESET);
    printf(COLOR_CYAN "  writev, birden fazla buffer'ı TEK sistem çağrısıyla yazabilir.\n" COLOR_RESET);
    
    // 12.2. pwrite (offset ile yazma)
    printf(COLOR_GREEN "\n12.2. pwrite() - Offset ile yazma:\n" COLOR_RESET);
    printf(COLOR_CYAN "  pwrite, dosya offset'ini değiştirmeden belirli bir konuma yazar.\n" COLOR_RESET);
    
    // 12.3. write ve lseek
    printf(COLOR_GREEN "\n12.3. write + lseek:\n" COLOR_RESET);
    int fd = open("seek_test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd != -1) {
        lseek(fd, 100, SEEK_SET);  // 100. byte'a git
        write(fd, "X", 1);          // 100. byte'a 'X' yaz
        close(fd);
        printf("  'seek_test.txt' dosyasına 100. byte'a 'X' yazıldı.\n");
        printf(COLOR_YELLOW "  Dosya boyutu: 101 byte (sparse file)\n" COLOR_RESET);
    }
}

/* ============================================================================
 * ANA FONKSİYON
 * ============================================================================ */

int main(int argc, char *argv[]) {
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║              write() SİSTEM ÇAĞRISININ DETAYLI ANALİZİ            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    // Tüm bölümleri çalıştır
    demo_basic_write();
    demo_write_return();
    demo_buffering();
    demo_file_descriptors();
    demo_write_vs_printf();
    demo_null_character();
    demo_behind_the_scenes();
    demo_signal_safety();
    demo_non_blocking();
    demo_assembly();
    demo_errors();
    demo_advanced();
    
    print_section("SONUÇ");
    printf(COLOR_GREEN "\n  write() bir sistem çağrısıdır, printf() ise bir kütüphane fonksiyonudur.\n" COLOR_RESET);
    printf(COLOR_GREEN "  write() UNBUFFERED'dır - her çağrı HEMEN sistem çağrısına dönüşür.\n" COLOR_RESET);
    printf(COLOR_GREEN "  write() async-signal-safe'dir - sinyal işleyicilerinde kullanılabilir.\n" COLOR_RESET);
    printf(COLOR_GREEN "  write() formatlama YAPMAZ - sadece ham byte yazar.\n" COLOR_RESET);
    
    print_separator('=', 60);
    printf("Program başarıyla tamamlandı.\n");
    
    return 0;
}