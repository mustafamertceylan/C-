/*
 * Öğrenci: Sistem Programlama Öğrencisi
 * Konu: open() Sistem Çağrısı ve umask İlişkisi - Detaylı Eğitim Kodu
 * 
 * Bu program, open() sistem çağrısının tüm inceliklerini, dosya izinlerini,
 * umask kavramını ve bunların etkileşimini göstermektedir.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

// ANSI renk kodları (görsel ayrım için)
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

/* ============================================================================
 * YARDIMCI FONKSİYONLAR
 * ============================================================================ */

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

/**
 * print_permissions - İzinleri okunabilir formatta yazdırır
 */
void print_permissions(mode_t mode) {
    printf(COLOR_CYAN "  İzinler: " COLOR_RESET);
    
    // Sahip (User)
    printf("%c", (mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');
    
    // Grup (Group)
    printf("%c", (mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');
    
    // Diğer (Other)
    printf("%c", (mode & S_IROTH) ? 'r' : '-');
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (mode & S_IXOTH) ? 'x' : '-');
    
    // Octal gösterim
    printf(COLOR_YELLOW " (%03o)" COLOR_RESET, mode & 0777);
    printf("\n");
}

/**
 * print_file_info - Dosya bilgilerini yazdırır
 */
void print_file_info(const char *filename) {
    struct stat st;
    
    if (stat(filename, &st) == 0) {
        printf("  Dosya: %s\n", filename);
        printf("  Boyut: %ld byte\n", st.st_size);
        print_permissions(st.st_mode);
    } else {
        printf(COLOR_RED "  Dosya bulunamadı: %s\n" COLOR_RESET, filename);
    }
}

/**
 * show_umask_effect - umask'ın etkisini gösteren görsel tablo
 */
void show_umask_effect(int requested_mode, int umask_value) {
    int result_mode = requested_mode & (~umask_value);
    
    printf("\n  " COLOR_CYAN "İstenen izin (mode):    " COLOR_RESET);
    print_permissions(requested_mode);
    
    printf("  " COLOR_RED "umask (KALDIRILACAK):   " COLOR_RESET);
    print_permissions(umask_value);
    
    printf("  " COLOR_GREEN "SONUÇ (mode & ~umask):  " COLOR_RESET);
    print_permissions(result_mode);
    
    printf("\n  " COLOR_YELLOW "Hesaplama: %03o & ~%03o = %03o\n" COLOR_RESET, 
           requested_mode, umask_value, result_mode);
}

/* ============================================================================
 * BÖLÜM 1: open() Sistem Çağrısının Temelleri
 * ============================================================================ */

void demo_open_basics(void) {
    print_section("1. open() Sistem Çağrısının Temelleri");
    
    printf(COLOR_CYAN "open() prototipi:\n" COLOR_RESET);
    printf("  int open(const char *pathname, int flags, mode_t mode);\n\n");
    
    printf(COLOR_CYAN "Dönüş değeri:\n" COLOR_RESET);
    printf("  • Başarılı → Dosya tanımlayıcı (3, 4, 5, ...)\n");
    printf("  • Başarısız → -1 (errno ayarlanır)\n\n");
    
    printf(COLOR_CYAN "Temel flag'ler:\n" COLOR_RESET);
    printf("  ┌─────────────────┬────────────────────────────────────────────┐\n");
    printf("  │ Flag            │ Açıklama                                   │\n");
    printf("  ├─────────────────┼────────────────────────────────────────────┤\n");
    printf("  │ O_RDONLY        │ Sadece okuma (Read Only)                   │\n");
    printf("  │ O_WRONLY        │ Sadece yazma (Write Only)                  │\n");
    printf("  │ O_RDWR          │ Okuma ve yazma (Read/Write)                │\n");
    printf("  │ O_CREAT         │ Dosya yoksa oluştur (Create)               │\n");
    printf("  │ O_TRUNC         │ Varsa dosyayı sıfırla (Truncate)           │\n");
    printf("  │ O_APPEND        │ Her yazmayı sona ekle (Append)             │\n");
    printf("  │ O_EXCL          │ O_CREAT ile: dosya varsa hata              │\n");
    printf("  │ O_SYNC          │ Senkron yazma (Sync)                       │\n");
    printf("  └─────────────────┴────────────────────────────────────────────┘\n");
    
    // Basit örnekler
    printf(COLOR_GREEN "\nÖrnek kullanımlar:\n" COLOR_RESET);
    printf("  int fd1 = open(\"file.txt\", O_RDONLY);\n");
    printf("  int fd2 = open(\"new.txt\", O_WRONLY | O_CREAT, 0644);\n");
    printf("  int fd3 = open(\"log.txt\", O_WRONLY | O_APPEND);\n");
}

/* ============================================================================
 * BÖLÜM 2: Dosya İzinleri (Permissions) Detaylı
 * ============================================================================ */

void demo_permissions(void) {
    print_section("2. Dosya İzinleri (Permissions) Detaylı");
    
    printf(COLOR_CYAN "Unix/Linux dosya izinleri 3 grup × 3 hak = 9 bit:\n\n" COLOR_RESET);
    
    printf("  ┌─────────────┬─────┬─────┬─────┬─────────────────────────────┐\n");
    printf("  │ Kullanıcı   │ r   │ w   │ x   │ Anlamı                      │\n");
    printf("  ├─────────────┼─────┼─────┼─────┼─────────────────────────────┤\n");
    printf("  │ Sahip (u)   │ 400 │ 200 │ 100 │ Dosya sahibi                │\n");
    printf("  │ Grup (g)    │ 040 │ 020 │ 010 │ Aynı gruptakiler            │\n");
    printf("  │ Diğer (o)   │ 004 │ 002 │ 001 │ Diğer tüm kullanıcılar      │\n");
    printf("  └─────────────┴─────┴─────┴─────┴─────────────────────────────┘\n");
    
    printf(COLOR_CYAN "\nYaygın izin kombinasyonları (octal):\n" COLOR_RESET);
    printf("  ┌─────────┬─────────┬─────────────────────────────────────────┐\n");
    printf("  │ Octal   │ Sembolik│ Açıklama                                │\n");
    printf("  ├─────────┼─────────┼─────────────────────────────────────────┤\n");
    printf("  │ 0644    │ rw-r--r--│ Normal dosya (sahip yazabilir)         │\n");
    printf("  │ 0755    │ rwxr-xr-x│ Program veya dizin                     │\n");
    printf("  │ 0600    │ rw-------│ Sadece sahip (gizli dosya)             │\n");
    printf("  │ 0666    │ rw-rw-rw-│ Herkes yazabilir (tehlikeli!)          │\n");
    printf("  │ 0700    │ rwx------│ Sadece sahip (özel dizin)              │\n");
    printf("  └─────────┴─────────┴─────────────────────────────────────────┘\n");
}

/* ============================================================================
 * BÖLÜM 3: umask Kavramı Detaylı
 * ============================================================================ */

void demo_umask_concept(void) {
    print_section("3. umask (User Mask) Kavramı");
    
    printf(COLOR_CYAN "umask nedir?\n" COLOR_RESET);
    printf("  • Yeni oluşturulan dosya ve dizinlerden HANGİ İZİNLERİN\n");
    printf("    KALDIRILACAĞINI belirten bir mask'dir.\n");
    printf("  • Bir güvenlik filtresidir.\n");
    printf("  • Her process'in kendi umask değeri vardır.\n\n");
    
    printf(COLOR_CYAN "Formül:\n" COLOR_RESET);
    printf(COLOR_YELLOW "  Nihai İzin = İstenen İzin & (~umask)\n" COLOR_RESET);
    printf("  veya: Nihai İzin = İstenen İzin - umask (bit bazında)\n\n");
    
    // Mevcut umask'ı göster
    mode_t current_umask = umask(0);
    umask(current_umask);  // Geri yükle
    
    printf(COLOR_GREEN "Mevcut shell umask değeri: %03o\n" COLOR_RESET, current_umask);
    printf("  Terminalde 'umask' komutu ile görebilirsiniz.\n\n");
    
    printf(COLOR_CYAN "Yaygın umask değerleri ve anlamları:\n" COLOR_RESET);
    printf("  ┌─────────┬─────────────────────────────────────────────────────┐\n");
    printf("  │ umask   │ Etkisi                                             │\n");
    printf("  ├─────────┼─────────────────────────────────────────────────────┤\n");
    printf("  │ 000     │ Hiçbir izin kaldırılmaz (çok tehlikeli!)           │\n");
    printf("  │ 002     │ Diğerlerinin yazma izni kaldırılır                  │\n");
    printf("  │ 022     │ Grup ve diğerlerinin yazma izni kaldırılır (Varsayılan)│\n");
    printf("  │ 027     │ Grup yazamaz, diğerleri hiçbir şey yapamaz          │\n");
    printf("  │ 077     │ Sadece sahip yazabilir (en güvenli)                 │\n");
    printf("  └─────────┴─────────────────────────────────────────────────────┘\n");
}

/* ============================================================================
 * BÖLÜM 4: umask ve open() Etkileşimi (ÖRNEKLER)
 * ============================================================================ */

void demo_umask_interaction(void) {
    print_section("4. umask ve open() Etkileşimi - Pratik Örnekler");
    
    // Mevcut umask'ı kaydet
    mode_t original_umask = umask(0);
    
    // Test dosyaları için dizin oluştur
    system("mkdir -p test_files");
    
    printf(COLOR_YELLOW "Test 1: Varsayılan umask (022) ile 0666 isteniyor\n" COLOR_RESET);
    umask(022);
    int fd1 = open("test_files/file_umask_022.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd1 != -1) {
        write(fd1, "Test", 4);
        close(fd1);
    }
    print_file_info("test_files/file_umask_022.txt");
    show_umask_effect(0666, 022);
    
    printf(COLOR_YELLOW "\nTest 2: umask 002 ile 0666 isteniyor\n" COLOR_RESET);
    umask(002);
    int fd2 = open("test_files/file_umask_002.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd2 != -1) {
        write(fd2, "Test", 4);
        close(fd2);
    }
    print_file_info("test_files/file_umask_002.txt");
    show_umask_effect(0666, 002);
    
    printf(COLOR_YELLOW "\nTest 3: umask 077 ile 0666 isteniyor\n" COLOR_RESET);
    umask(077);
    int fd3 = open("test_files/file_umask_077.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd3 != -1) {
        write(fd3, "Test", 4);
        close(fd3);
    }
    print_file_info("test_files/file_umask_077.txt");
    show_umask_effect(0666, 077);
    
    printf(COLOR_YELLOW "\nTest 4: umask 022 ile 0644 isteniyor (Sizin kodunuzdaki gibi)\n" COLOR_RESET);
    umask(022);
    int fd4 = open("test_files/file_umask_0644.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd4 != -1) {
        write(fd4, "Test", 4);
        close(fd4);
    }
    print_file_info("test_files/file_umask_0644.txt");
    show_umask_effect(0644, 022);
    
    printf(COLOR_YELLOW "\nTest 5: umask 027 ile 0666 isteniyor\n" COLOR_RESET);
    umask(027);
    int fd5 = open("test_files/file_umask_027.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd5 != -1) {
        write(fd5, "Test", 4);
        close(fd5);
    }
    print_file_info("test_files/file_umask_027.txt");
    show_umask_effect(0666, 027);
    
    // Eski umask'ı geri yükle
    umask(original_umask);
    
    printf(COLOR_GREEN "\n📁 Tüm test dosyaları 'test_files/' dizininde oluşturuldu.\n" COLOR_RESET);
    printf(COLOR_CYAN "  ls -la test_files/ komutu ile inceleyebilirsiniz.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 5: open() Flag'leri Detaylı
 * ============================================================================ */

void demo_open_flags(void) {
    print_section("5. open() Flag'leri Detaylı");
    
    char test_data[] = "Bu bir test satırıdır.\n";
    
    // Mevcut umask'ı kaydet
    mode_t original_umask = umask(022);
    
    // 5.1 O_CREAT - Dosya oluşturma
    printf(COLOR_CYAN "5.1. O_CREAT - Dosya oluşturma:\n" COLOR_RESET);
    int fd_create = open("test_files/created.txt", O_WRONLY | O_CREAT, 0644);
    if (fd_create != -1) {
        write(fd_create, test_data, strlen(test_data));
        close(fd_create);
        printf("  ✓ created.txt oluşturuldu\n");
    }
    
    // 5.2 O_EXCL - Exclusive create
    printf(COLOR_CYAN "\n5.2. O_EXCL - Exclusive create:\n" COLOR_RESET);
    int fd_excl = open("test_files/exclusive.txt", O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd_excl != -1) {
        write(fd_excl, test_data, strlen(test_data));
        close(fd_excl);
        printf("  ✓ exclusive.txt ilk kez oluşturuldu\n");
    }
    
    // Aynı dosyayı tekrar açmayı dene
    fd_excl = open("test_files/exclusive.txt", O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd_excl == -1) {
        printf(COLOR_RED "  ✗ exclusive.txt zaten var! O_EXCL hata verdi: %s\n" COLOR_RESET, strerror(errno));
    }
    
    // 5.3 O_TRUNC - Dosyayı sıfırlama
    printf(COLOR_CYAN "\n5.3. O_TRUNC - Dosyayı sıfırlama:\n" COLOR_RESET);
    
    // Önce bir dosya oluştur ve içine yaz
    int fd_trunc = open("test_files/trunc_test.txt", O_WRONLY | O_CREAT, 0644);
    write(fd_trunc, "Bu eski veridir. Silinecek.\n", 28);
    close(fd_trunc);
    printf("  Önce: "); print_file_info("test_files/trunc_test.txt");
    
    // Şimdi O_TRUNC ile aç
    fd_trunc = open("test_files/trunc_test.txt", O_WRONLY | O_TRUNC);
    write(fd_trunc, "Yeni veri\n", 10);
    close(fd_trunc);
    printf("  Sonra: "); print_file_info("test_files/trunc_test.txt");
    
    // 5.4 O_APPEND - Sona ekleme
    printf(COLOR_CYAN "\n5.4. O_APPEND - Sona ekleme:\n" COLOR_RESET);
    int fd_append = open("test_files/append_test.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    write(fd_append, "İlk satır\n", 10);
    write(fd_append, "İkinci satır\n", 13);
    write(fd_append, "Üçüncü satır\n", 13);
    close(fd_append);
    printf("  append_test.txt içeriği:\n");
    system("cat test_files/append_test.txt | sed 's/^/    /'");
    
    // 5.5 O_SYNC - Senkron yazma
    printf(COLOR_CYAN "\n5.5. O_SYNC - Senkron yazma:\n" COLOR_RESET);
    printf(COLOR_YELLOW "  O_SYNC ile yazma tamamlanana kadar beklenir (daha yavaş ama güvenli)\n" COLOR_RESET);
    int fd_sync = open("test_files/sync_test.txt", O_WRONLY | O_CREAT | O_SYNC, 0644);
    if (fd_sync != -1) {
        write(fd_sync, "Bu yazı diskte olduğundan emin olana kadar bekler\n", 48);
        close(fd_sync);
        printf("  ✓ sync_test.txt oluşturuldu\n");
    }
    
    umask(original_umask);
}

/* ============================================================================
 * BÖLÜM 6: umask() Sistem Çağrısı
 * ============================================================================ */

void demo_umask_syscall(void) {
    print_section("6. umask() Sistem Çağrısı");
    
    printf(COLOR_CYAN "umask() prototipi:\n" COLOR_RESET);
    printf("  mode_t umask(mode_t mask);\n\n");
    
    printf(COLOR_CYAN "Özellikler:\n" COLOR_RESET);
    printf("  • Eski umask değerini döndürür\n");
    printf("  • Sadece geçerli process için etkilidir\n");
    printf("  • Child process'ler (fork ile) ebeveynin umask'ını miras alır\n");
    printf("  • Root dahil her kullanıcı kendi umask'ını değiştirebilir\n\n");
    
    printf(COLOR_GREEN "Örnek kullanım:\n" COLOR_RESET);
    printf("  mode_t old = umask(0);    // umask'ı 0 yap, eski değeri al\n");
    printf("  // ... dosya oluşturma işlemleri ...\n");
    printf("  umask(old);              // Eski değeri geri yükle\n");
}

/* ============================================================================
 * BÖLÜM 7: Hata Yönetimi ve perror()
 * ============================================================================ */

void demo_error_handling(void) {
    print_section("7. Hata Yönetimi ve perror()");
    
    printf(COLOR_CYAN "open() başarısız olduğunda -1 döner ve errno ayarlanır.\n\n" COLOR_RESET);
    
    // 7.1. Var olmayan dosyayı açmaya çalış
    printf(COLOR_YELLOW "7.1. Var olmayan dosyayı okumaya çalış:\n" COLOR_RESET);
    int fd = open("olmayan_dosya.txt", O_RDONLY);
    if (fd == -1) {
        printf(COLOR_RED "  errno: %d (%s)\n" COLOR_RESET, errno, strerror(errno));
        perror("  perror çıktısı");
    }
    
    // 7.2. İzinsiz dosya oluşturmaya çalış (root değilsek)
    printf(COLOR_YELLOW "\n7.2. /etc/ dizininde dosya oluşturmaya çalış:\n" COLOR_RESET);
    fd = open("/etc/test_write.txt", O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        printf(COLOR_RED "  errno: %d (%s)\n" COLOR_RESET, errno, strerror(errno));
        perror("  perror çıktısı");
    }
    
    // 7.3. Geçersiz flag kombinasyonu
    printf(COLOR_YELLOW "\n7.3. Geçersiz flag (O_RDONLY ve O_WRONLY birlikte):\n" COLOR_RESET);
    printf(COLOR_CYAN "  Bu aslında geçersiz bir kombinasyon değil, O_RDWR kullanılmalı\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 8: Dosya Tanımlayıcıları (File Descriptors)
 * ============================================================================ */

void demo_file_descriptors(void) {
    print_section("8. Dosya Tanımlayıcıları (File Descriptors)");
    
    printf(COLOR_CYAN "Her program başladığında 3 standart dosya tanımlayıcı alır:\n\n" COLOR_RESET);
    
    printf("  ┌────────────┬───────────────────────┬─────────────────────────┐\n");
    printf("  │ fd         │ İsim                  │ Varsayılan Hedef        │\n");
    printf("  ├────────────┼───────────────────────┼─────────────────────────┤\n");
    printf("  │ 0          │ stdin (Standart Girdi)│ Klavye                  │\n");
    printf("  │ 1          │ stdout (Standart Çıktı)│ Ekran (Terminal)        │\n");
    printf("  │ 2          │ stderr (Standart Hata)│ Ekran (Terminal)        │\n");
    printf("  └────────────┴───────────────────────┴─────────────────────────┘\n");
    
    // Yeni dosyaların fd değerlerini göster
    printf(COLOR_GREEN "\nYeni açılan dosyaların fd değerleri:\n" COLOR_RESET);
    
    mode_t old_umask = umask(022);
    
    int fd1 = open("test_files/fd_test_1.txt", O_WRONLY | O_CREAT, 0644);
    int fd2 = open("test_files/fd_test_2.txt", O_WRONLY | O_CREAT, 0644);
    int fd3 = open("test_files/fd_test_3.txt", O_WRONLY | O_CREAT, 0644);
    
    printf("  İlk açılan dosya fd'si: %d\n", fd1);
    printf("  İkinci açılan dosya fd'si: %d\n", fd2);
    printf("  Üçüncü açılan dosya fd'si: %d\n", fd3);
    
    close(fd1);
    close(fd2);
    close(fd3);
    
    // Kapattıktan sonra yeniden aç
    int fd4 = open("test_files/fd_test_4.txt", O_WRONLY | O_CREAT, 0644);
    printf("\n  Kapattıktan sonra yeni açılan dosya fd'si: %d (en küçük kullanılmayan)\n", fd4);
    close(fd4);
    
    umask(old_umask);
    
    printf(COLOR_YELLOW "\n📝 Not: fd değerleri genellikle 3'ten başlar (0,1,2 zaten kullanılıyor)\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 9: Güvenlik ve En İyi Uygulamalar
 * ============================================================================ */

void demo_security_best_practices(void) {
    print_section("9. Güvenlik ve En İyi Uygulamalar");
    
    printf(COLOR_CYAN "1. Her zaman mode belirtin!\n" COLOR_RESET);
    printf(COLOR_RED "   open(\"file.txt\", O_WRONLY | O_CREAT);  // HATA! mode yok!\n" COLOR_RESET);
    printf(COLOR_GREEN "   open(\"file.txt\", O_WRONLY | O_CREAT, 0644);  // DOĞRU\n" COLOR_RESET);
    
    printf(COLOR_CYAN "\n2. Hassas dosyalar için en kısıtlayıcı izinleri kullanın:\n" COLOR_RESET);
    printf("   open(\"password.txt\", O_WRONLY | O_CREAT, 0600);  // Sadece sahip\n");
    
    printf(COLOR_CYAN "\n3. open() dönüş değerini HER ZAMAN kontrol edin:\n" COLOR_RESET);
    printf("   int fd = open(\"file.txt\", O_RDONLY);\n");
    printf("   if (fd == -1) {\n");
    printf("       perror(\"open\");\n");
    printf("       return -1;\n");
    printf("   }\n");
    
    printf(COLOR_CYAN "\n4. O_EXCL kullanarak race condition'ları önleyin:\n" COLOR_RESET);
    printf("   int fd = open(\"lockfile\", O_WRONLY | O_CREAT | O_EXCL, 0600);\n");
    
    printf(COLOR_CYAN "\n5. Program içinde umask'ı geçici olarak değiştirip geri yükleyin:\n" COLOR_RESET);
    printf("   mode_t old = umask(077);\n");
    printf("   int fd = open(\"secret.txt\", O_WRONLY | O_CREAT, 0600);\n");
    printf("   umask(old);\n");
}

/* ============================================================================
 * BÖLÜM 10: Özet ve Karşılaştırma Tablosu
 * ============================================================================ */

void demo_summary(void) {
    print_section("10. Özet: open() ve umask Karşılaştırması");
    
    printf(COLOR_CYAN "┌─────────────────────────────────────────────────────────────────────────┐\n");
    printf(COLOR_CYAN "│                        KARŞILAŞTIRMA TABLOSU                           │\n");
    printf(COLOR_CYAN "├─────────────────────┬───────────────────────────┬───────────────────────┤\n");
    printf(COLOR_CYAN "│ Özellik             │ open() mode               │ umask                 │\n");
    printf(COLOR_CYAN "├─────────────────────┼───────────────────────────┼───────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ Seviye              │ Sistem çağrısı parametresi  │ Process ayarı          │\n");
    printf("│ Amaç                │ İstenen izinleri belirtir   │ Kaldırılacak izinleri  │\n");
    printf("│                      │                           │ belirtir               │\n");
    printf("│ Etki                │ Sadece o dosya için        │ Tüm yeni dosyalar için │\n");
    printf("│ Kalıcılık           │ Sadece o open() çağrısı için│ Process sonlanana kadar│\n");
    printf("│ Kim değiştirebilir? │ Programcı (kod içinde)      │ Programcı veya kabuk   │\n");
    printf("│ Formül              │ -                          │ Nihai = mode & ~umask  │\n");
    printf(COLOR_CYAN "└─────────────────────┴───────────────────────────┴───────────────────────┘\n" COLOR_RESET);
    
    printf(COLOR_GREEN "\n📌 Altın Kural:\n" COLOR_RESET);
    printf("  open()'da belirtilen mode = \"NASIL OLSUN İSTİYORUM?\"\n");
    printf("  umask = \"HANGİ İZİNLERİ KESİNLİKLE VERMEK İSTEMİYORUM?\"\n");
    printf("  Sonuç = İkisinin birleşimi\n");
}

/* ============================================================================
 * BÖLÜM 11: Etkileşimli Test Alanı
 * ============================================================================ */

void demo_interactive(void) {
    print_section("11. Etkileşimli Test Alanı");
    
    printf(COLOR_YELLOW "Bu bölümde farklı mode ve umask kombinasyonlarını test edebilirsiniz.\n" COLOR_RESET);
    printf(COLOR_CYAN "Not: Bu bölüm etkileşimli olduğu için otomatik geçiliyor.\n" COLOR_RESET);
    printf("Kendiniz denemek için aşağıdaki gibi küçük bir program yazabilirsiniz:\n\n");
    
    printf(COLOR_GREEN "#include <stdio.h>\n");
    printf("#include <fcntl.h>\n");
    printf("#include <unistd.h>\n");
    printf("#include <sys/stat.h>\n\n");
    printf("int main() {\n");
    printf("    mode_t old = umask(0);  // Geçici olarak sıfırla\n");
    printf("    \n");
    printf("    // Farklı kombinasyonları dene\n");
    printf("    creat(\"test_0666.txt\", 0666);\n");
    printf("    creat(\"test_0644.txt\", 0644);\n");
    printf("    creat(\"test_0600.txt\", 0600);\n");
    printf("    \n");
    printf("    umask(old);  // Eski değeri geri yükle\n");
    printf("    return 0;\n");
    printf("}\n" COLOR_RESET);
}

/* ============================================================================
 * ANA FONKSİYON
 * ============================================================================ */

int main(int argc, char *argv[]) {
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║           open() SİSTEM ÇAĞRISI VE umask DETAYLI EĞİTİM KODU      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    // Test dosyaları için dizin oluştur
    system("mkdir -p test_files");
    
    // Tüm bölümleri çalıştır
    demo_open_basics();
    demo_permissions();
    demo_umask_concept();
    demo_umask_interaction();
    demo_open_flags();
    demo_umask_syscall();
    demo_error_handling();
    demo_file_descriptors();
    demo_security_best_practices();
    demo_summary();
    demo_interactive();
    
    print_section("SONUÇ");
    printf(COLOR_GREEN "\n  open() sistem çağrısı, dosya açmanın ve oluşturmanın temel yoludur.\n" COLOR_RESET);
    printf(COLOR_GREEN "  umask, yeni dosyaların izinlerini güvenli hale getiren bir filtredir.\n" COLOR_RESET);
    printf(COLOR_GREEN "  Nihai izin = mode & ~umask formülü ile hesaplanır.\n" COLOR_RESET);
    printf(COLOR_GREEN "  Her open() çağrısından sonra dönüş değerini kontrol etmeyi unutmayın!\n" COLOR_RESET);
    
    printf("\n");
    print_separator('=', 60);
    printf("Program başarıyla tamamlandı.\n");
    printf("Test dosyaları 'test_files/' dizininde oluşturuldu.\n");
    printf("'ls -la test_files/' ile inceleyebilirsiniz.\n");
    print_separator('=', 60);
    
    return 0;
}