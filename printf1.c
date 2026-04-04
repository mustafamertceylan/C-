/*
 * Öğrenci: Sistem Programlama Öğrencisi
 * Konu: printf() Fonksiyonunun Derinlemesine Analizi
 * 
 * Bu program, printf fonksiyonunun farklı kullanımlarını, davranışlarını
 * ve arka planda yaptığı işlemleri göstermektedir.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
 * BÖLÜM 1: printf'in Temel Kullanımları
 * ============================================================================ */

void demo_basic_printf(void) {
    print_section("1. printf'in Temel Kullanımları");
    
    // 1.1. Basit string yazdırma
    printf(COLOR_GREEN "1.1. Basit string: " COLOR_RESET);
    printf("Merhaba Dünya!\n");
    
    // 1.2. Format belirteçleri ile yazdırma
    printf(COLOR_GREEN "1.2. Format belirteçleri: " COLOR_RESET);
    int sayi = 42;
    double pi = 3.14159;
    char harf = 'A';
    printf("int: %d, double: %.2f, char: %c\n", sayi, pi, harf);
    
    // 1.3. Birden fazla argüman
    printf(COLOR_GREEN "1.3. Birden fazla argüman: " COLOR_RESET);
    printf("%d + %d = %d\n", 10, 20, 30);
    
    // 1.4. Sabit genişlik ve hizalama
    printf(COLOR_GREEN "1.4. Sabit genişlik: " COLOR_RESET);
    printf("|%10s| |%-10s|\n", "sağa", "sola");
    
    // 1.5. Özel karakterler
    printf(COLOR_GREEN "1.5. Özel karakterler: " COLOR_RESET);
    printf("Tab\tile\bayrılmış\n");
    printf("Yeni satır\n");
    printf("Geri\b\b\b\b\b silindi\n");
}

/* ============================================================================
 * BÖLÜM 2: printf'in Dönüş Değeri (Yazılan Karakter Sayısı)
 * ============================================================================ */

void demo_printf_return(void) {
    print_section("2. printf'in Dönüş Değeri");
    
    int count;
    
    // 2.1. Basit kullanım
    count = printf("Merhaba");
    printf(COLOR_GREEN " -> Yazılan karakter sayısı: %d\n" COLOR_RESET, count);
    
    // 2.2. Format belirteçli
    count = printf("%d + %d = %d", 5, 3, 8);
    printf(COLOR_GREEN " -> Yazılan karakter sayısı: %d\n" COLOR_RESET, count);
    
    // 2.3. Escape karakterler (\n bir karakterdir!)
    count = printf("Satır 1\nSatır 2\n");
    printf(COLOR_GREEN " -> Yazılan karakter sayısı: %d (\\n'ler dahil)\n" COLOR_RESET, count);
    
    // 2.4. Hata durumu (nadir)
    // printf başarısız olursa negatif değer döndürür
    printf(COLOR_YELLOW "Not: printf hata durumunda EOF (-1) döndürür.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 3: printf ve Tamponlama (Buffering)
 * ============================================================================ */

void demo_buffering(void) {
    print_section("3. printf ve Tamponlama (Buffering)");
    
    printf(COLOR_YELLOW "3.1. stdout (standart çıktı) LINE BUFFERED'dır.\n" COLOR_RESET);
    printf(COLOR_YELLOW "     Yani '\\n' görülene kadar çıktı ekranda GÖRÜNMEZ!\n\n" COLOR_RESET);
    
    // Bu mesaj hemen görünmez! (çünkü \n yok)
    printf(COLOR_MAGENTA "Bu mesaj hemen görünmeyecek");
    
    printf(COLOR_YELLOW "\n3.2. 3 saniye bekleniyor...\n" COLOR_RESET);
    sleep(3);
    
    // \n ekleyince buffer flush olur
    printf(COLOR_MAGENTA " (ŞİMDİ göründü!)\n" COLOR_RESET);
    
    // 3.3. fflush ile zorla flush
    printf(COLOR_YELLOW "\n3.3. fflush() ile zorla flush:\n" COLOR_RESET);
    printf(COLOR_MAGENTA "Bu mesaj hemen görünecek");
    fflush(stdout);  // Zorla flush
    printf(COLOR_MAGENTA " (fflush sayesinde!)\n" COLOR_RESET);
    
    // 3.4. stderr unbuffered'dır (hemen görünür)
    printf(COLOR_YELLOW "\n3.4. stderr UNBUFFERED'dır:\n" COLOR_RESET);
    fprintf(stderr, COLOR_RED "Bu hata mesajı HEMEN görünür (\\n olmasa bile)\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 4: printf'in Arka Plan Çalışması (Sistem Çağrıları)
 * ============================================================================ */

void demo_behind_the_scenes(void) {
    print_section("4. printf'in Arka Plan Çalışması");
    
    printf(COLOR_CYAN "printf() çağrısının katmanları:\n\n" COLOR_RESET);
    
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ 1. KULLANICI MODU (User Space)                                 │\n");
    printf("│    ┌─────────────────────────────────────────────────────────┐ │\n");
    printf("│    │ printf(\"Merhaba\\n\");                                   │ │\n");
    printf("│    │   ↓                                                     │ │\n");
    printf("│    │ C Standart Kütüphanesi (glibc)                          │ │\n");
    printf("│    │   - Format string parse edilir (%d, %s, etc.)           │ │\n");
    printf("│    │   - Değerler string'e dönüştürülür                      │ │\n");
    printf("│    │   - Buffer'da biriktirilir (stdout)                     │ │\n");
    printf("│    │   - '\\n' görülünce veya buffer dolunca flush            │ │\n");
    printf("│    └─────────────────────────────────────────────────────────┘ │\n");
    printf("│                         ↓                                      │\n");
    printf("│    ┌─────────────────────────────────────────────────────────┐ │\n");
    printf("│    │ write() sistem çağrısı (wrapper)                        │ │\n");
    printf("│    │   - Buffer'daki veriyi alır                            │ │\n");
    printf("│    │   - Sistem çağrısı numarasını kaydeder (rax=1)         │ │\n");
    printf("│    └─────────────────────────────────────────────────────────┘ │\n");
    printf("│                         ↓                                      │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ 2. SİSTEM ÇAĞRISI ARAYÜZÜ (System Call Interface)             │\n");
    printf("│    - 'syscall' talimatı (x86-64'te)                           │\n");
    printf("│    - Kullanıcı modundan çekirdek moduna geçiş (ring 3 -> 0)   │\n");
    printf("│                         ↓                                      │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ 3. ÇEKİRDEK MODU (Kernel Space)                               │\n");
    printf("│    ┌─────────────────────────────────────────────────────────┐ │\n");
    printf("│    │ sys_write() sistem çağrısı işleyicisi                   │ │\n");
    printf("│    │   - fd=1 (stdout) kontrol edilir                        │ │\n");
    printf("│    │   - Terminal veya dosya aygıt sürücüsüne yönlendirilir  │ │\n");
    printf("│    │   - Veri terminal buffer'ına veya dosyaya yazılır       │ │\n");
    printf("│    └─────────────────────────────────────────────────────────┘ │\n");
    printf("└─────────────────────────────────────────────────────────────────┘\n");
}

/* ============================================================================
 * BÖLÜM 5: printf ve write Sistem Çağrısı Karşılaştırması
 * ============================================================================ */

void demo_printf_vs_write(void) {
    print_section("5. printf() vs write() Sistem Çağrısı");
    
    printf(COLOR_CYAN "printf() bir KÜTÜPHANE fonksiyonudur.\n" COLOR_RESET);
    printf(COLOR_CYAN "write() bir SİSTEM ÇAĞRISIDIR.\n\n" COLOR_RESET);
    
    // printf ile yazdırma (buffered)
    printf(COLOR_GREEN "printf ile: " COLOR_RESET);
    printf("Bu mesaj printf ile yazıldı\n");
    
    // write ile doğrudan yazdırma (unbuffered)
    printf(COLOR_GREEN "write ile: " COLOR_RESET);
    write(1, "Bu mesaj write ile yazıldı\n", 26);
    
    printf(COLOR_YELLOW "\nFarklar:\n" COLOR_RESET);
    printf("  ┌─────────────────┬────────────────────────┬────────────────────────┐\n");
    printf("  │ Özellik         │ printf()               │ write()                │\n");
    printf("  ├─────────────────┼────────────────────────┼────────────────────────┤\n");
    printf("  │ Seviye          │ Kütüphane fonksiyonu   │ Sistem çağrısı         │\n");
    printf("  │ Tamponlama      │ Line buffered (stdout) │ Unbuffered             │\n");
    printf("  │ Format desteği  │ Evet (%%d, %%s, etc.)   │ Hayır (ham byte)       │\n");
    printf("  │ Mod geçişi      │ Sadece flush anında    │ Her çağrıda            │\n");
    printf("  │ Maliyet         │ Daha düşük (toplu)     │ Daha yüksek (her çağrı)│\n");
    printf("  └─────────────────┴────────────────────────┴────────────────────────┘\n");
}

/* ============================================================================
 * BÖLÜM 6: printf'in Bellek Düzeni (String Literal)
 * ============================================================================ */

void demo_memory_layout(void) {
    print_section("6. printf ve String Literal'ların Bellek Düzeni");
    
    // Dizi olarak tanımlanan string (stack'te kopya - değiştirilebilir)
    char str_array[] = "Merhaba";
    
    // Pointer olarak tanımlanan string (rodata'da - SALT OKUNUR)
    char *str_ptr = "Merhaba";
    
    printf(COLOR_CYAN "Dizi tanımı (char str_array[]):\n" COLOR_RESET);
    printf("  str_array adresi: %p\n", (void*)str_array);
    printf("  str_array içeriği: \"%s\"\n", str_array);
    printf("  sizeof(str_array): %zu byte\n", sizeof(str_array));
    printf("  DEĞİŞTİRİLEBİLİR: Evet (stack'te kopya)\n\n");
    
    printf(COLOR_CYAN "Pointer tanımı (char *str_ptr):\n" COLOR_RESET);
    printf("  str_ptr adresi: %p\n", (void*)&str_ptr);
    printf("  str_ptr'nin gösterdiği adres: %p\n", (void*)str_ptr);
    printf("  str_ptr içeriği: \"%s\"\n", str_ptr);
    printf("  sizeof(str_ptr): %zu byte (pointer boyutu)\n", sizeof(str_ptr));
    printf(COLOR_RED "  DEĞİŞTİRİLEBİLİR: Hayır! (rodata - salt okunur)\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "\nGörsel Bellek Düzeni:\n" COLOR_RESET);
    printf("  ┌─────────────────────────────────────────────────────────┐\n");
    printf("  │ STACK                                                   │\n");
    printf("  │ str_array[0]='M' str_array[1]='e' ... str_array[7]='\\0'│\n");
    printf("  │ str_ptr (8 byte) = 0x400624 ────┐                       │\n");
    printf("  └─────────────────────────────────┼───────────────────────┘\n");
    printf("  ┌─────────────────────────────────┼───────────────────────┐\n");
    printf("  │ .rodata (READ-ONLY)             ▼                       │\n");
    printf("  │ 0x400624: 'M' 'e' 'r' 'h' 'a' 'b' 'a' '\\0'             │\n");
    printf("  └─────────────────────────────────────────────────────────┘\n");
}

/* ============================================================================
 * BÖLÜM 7: printf ile Güvenlik Açıkları (Format String Vulnerability)
 * ============================================================================ */

void demo_security(void) {
    print_section("7. printf ve Güvenlik (Format String Vulnerability)");
    
    char user_input[] = "%x %x %x %x %x";
    
    printf(COLOR_RED "Güvenli olmayan kullanım:\n" COLOR_RESET);
    printf("  printf(user_input);  // Tehlikeli! Format string saldırısına açık\n");
    printf("  Bu durumda kullanıcı %%x göndererek stack'teki değerleri okuyabilir!\n\n");
    
    printf(COLOR_GREEN "Güvenli kullanım:\n" COLOR_RESET);
    printf("  printf(\"%%s\", user_input);  // Format string'i kontrol et\n\n");
    
    // Güvenli kullanım örneği
    char safe_input[] = "Kullanıcı girdisi";
    printf(COLOR_GREEN "Doğru kullanım: %s\n" COLOR_RESET, safe_input);
}

/* ============================================================================
 * BÖLÜM 8: printf'in Performansı (Sistem Çağrısı Sayısı)
 * ============================================================================ */

void demo_performance(void) {
    print_section("8. printf Performansı ve Sistem Çağrıları");
    
    printf(COLOR_CYAN "Tek bir printf, buffer dolana kadar sistem çağrısı YAPMAZ!\n\n" COLOR_RESET);
    
    // Bu 1000 printf sadece 1 sistem çağrısı yapar (buffer dolunca veya \n ile)
    printf(COLOR_YELLOW "1000 tane printf (\\n olmadan):\n" COLOR_RESET);
    for (int i = 0; i < 1000; i++) {
        printf("X");
    }
    printf(COLOR_GREEN " -> Sadece 1 sistem çağrısı (buffer doldu veya son flush)\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "\nHer bir printf (\\n ile):\n" COLOR_RESET);
    printf(COLOR_YELLOW "Not: Her '\\n' bir flush tetikler!\n" COLOR_RESET);
    
    // Bu 1000 printf 1000 sistem çağrısı yapar (her \n bir flush)
    for (int i = 0; i < 5; i++) {
        printf("Satır %d\n", i);
    }
    printf(COLOR_RED " -> 5 sistem çağrısı (her '\\n' bir write)\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 9: printf'in Tarihçesi ve Önemi
 * ============================================================================ */

void demo_history(void) {
    print_section("9. printf'in Tarihçesi ve Önemi");
    
    printf(COLOR_CYAN "printf fonksiyonu:\n" COLOR_RESET);
    printf("  • 1970'lerde Dennis Ritchie tarafından C diliyle birlikte geliştirildi\n");
    printf("  • İsmi: \"print formatted\" (biçimli yazdır)\n");
    printf("  • C standart kütüphanesinin (<stdio.h>) en önemli fonksiyonlarından biri\n");
    printf("  • Günümüzdeki birçok dil (Python, Java, Go, Rust) benzer formatlama kullanır\n");
    printf("\n");
    printf(COLOR_YELLOW "Neden bu kadar önemli?\n" COLOR_RESET);
    printf("  • Program çıktısının standart yoludur\n");
    printf("  • Hata ayıklama (debugging) için vazgeçilmezdir\n");
    printf("  • Formatlı çıktı sayesinde veriler okunabilir hale gelir\n");
    printf("  • Dosya yazma, log tutma, kullanıcı etkileşimi için temel araçtır\n");
}

/* ============================================================================
 * ANA FONKSİYON
 * ============================================================================ */

int main(int argc, char *argv[]) {
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║              printf() FONKSİYONUNUN DETAYLI ANALİZİ               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    // Tüm bölümleri çalıştır
    demo_basic_printf();
    demo_printf_return();
    demo_buffering();
    demo_behind_the_scenes();
    demo_printf_vs_write();
    demo_memory_layout();
    demo_security();
    demo_performance();
    demo_history();
    
    print_section("SONUÇ");
    printf(COLOR_GREEN "\n  printf() bir kütüphane fonksiyonudur, write() ise bir sistem çağrısıdır.\n" COLOR_RESET);
    printf(COLOR_GREEN "  printf formatlama ve tamponlama yapar, write ise ham byte'ları yazar.\n" COLOR_RESET);
    printf(COLOR_GREEN "  Her printf çağrısı write çağrısına dönüşmez - tamponlama sayesinde\n" COLOR_RESET);
    printf(COLOR_GREEN "  performans artar. '\\n' veya fflush() ile tampon boşaltılabilir.\n" COLOR_RESET);
    
    print_separator('=', 60);
    printf("Program başarıyla tamamlandı.\n");
    
    return 0;
}