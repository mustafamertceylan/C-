/*
 * Program: nice_demo.c
 * Konu: nice(), getpriority(), setpriority() Sistem Çağrıları
 * 
 * Bu program, Unix/Linux sistemlerinde işlem önceliği (niceness)
 * yönetiminin tüm yönlerini göstermektedir.
 * 
 * Derleme: gcc -o nice_demo nice_demo.c
 * Çalıştır: ./nice_demo [seçenekler]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include<time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>

// ANSI renk kodları
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
    for (int i = 0; i < length; i++) putchar(c);
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
 * get_nice_value - Bir process'in nice değerini güvenli bir şekilde alır
 * 
 * @param pid: Process ID (0 = kendi process'imiz)
 * @return: Nice değeri, hata durumunda -999
 */
int get_nice_value(pid_t pid) {
    errno = 0;
    int nice_val = getpriority(PRIO_PROCESS, pid);
    
    if (nice_val == -1 && errno != 0) {
        perror("getpriority");
        return -999;
    }
    return nice_val;
}

/**
 * print_process_info - Process bilgilerini yazdırır
 */
void print_process_info(const char *label, pid_t pid) {
    int nice_val = get_nice_value(pid);
    if (nice_val != -999) {
        printf("%s PID: %d, Nice değeri: %d\n", label, pid, nice_val);
    }
}

/**
 * busy_work - CPU yoğun iş yapan fonksiyon (öncelik farkını görmek için)
 */
void busy_work(int seconds, const char *label) {
    printf(COLOR_YELLOW "%s: %d saniye boyunca çalışıyor...\n" COLOR_RESET, label, seconds);
    time_t start = time(NULL);
    volatile unsigned long count = 0;
    
    while (time(NULL) - start < seconds) {
        for (int i = 0; i < 1000000; i++) {
            count++;
        }
        printf("%s: İlerleme... (%lu)\n", label, count);
        break; // Sadece bir kere göster
    }
    printf(COLOR_GREEN "%s: Tamamlandı!\n" COLOR_RESET, label);
}

/* ============================================================================
 * BÖLÜM 1: nice() Fonksiyonu - Basit Kullanım
 * ============================================================================ */

void demo_nice_basic(void) {
    print_section("1. nice() - Basit Kullanım");
    
    // Mevcut nice değerini al
    int current = get_nice_value(0);
    printf("Başlangıç nice değeri: %d\n", current);
    
    // nice() ile değeri artır (daha nazik yap)
    printf("\nnice(5) çağrılıyor...\n");
    int new_val = nice(5);
    
    if (new_val == -1) {
        perror("nice");
    } else {
        printf("Yeni nice değeri: %d (5 artırıldı)\n", new_val);
    }
    
    // Tekrar artır
    printf("\nnice(10) çağrılıyor...\n");
    new_val = nice(10);
    if (new_val != -1) {
        printf("Yeni nice değeri: %d (10 artırıldı)\n", new_val);
    }
    
    printf(COLOR_YELLOW "\nNot: Normal kullanıcılar sadece nice değerini ARTIRABİLİR!\n" COLOR_RESET);
    printf(COLOR_RED "Not: nice değerini azaltmak (önceliği yükseltmek) root yetkisi gerektirir.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 2: setpriority() ile Doğrudan Nice Değeri Ayarlama
 * ============================================================================ */

void demo_setpriority(void) {
    print_section("2. setpriority() - Doğrudan Nice Değeri Ayarlama");
    
    int current = get_nice_value(0);
    printf("Mevcut nice değeri: %d\n", current);
    
    // setpriority() ile doğrudan 10 değerine ayarla
    printf("\nsetpriority(PRIO_PROCESS, 0, 10) çağrılıyor...\n");
    if (setpriority(PRIO_PROCESS, 0, 10) == -1) {
        perror("setpriority");
    } else {
        printf("Yeni nice değeri: %d\n", get_nice_value(0));
    }
    
    // setpriority() ile doğrudan 5 değerine ayarla (daha yüksek öncelik - root gerekir)
    printf("\nsetpriority(PRIO_PROCESS, 0, 5) çağrılıyor...\n");
    if (setpriority(PRIO_PROCESS, 0, 5) == -1) {
        perror("setpriority (muhtemelen yetki hatası)");
        printf(COLOR_RED "Normal kullanıcı nice değerini DÜŞÜREMEZ (önceliği yükseltemez)!\n" COLOR_RESET);
    } else {
        printf("Yeni nice değeri: %d\n", get_nice_value(0));
    }
}

/* ============================================================================
 * BÖLÜM 3: Farklı Process'lerin Nice Değerini Okuma
 * ============================================================================ */

void demo_read_other_process(void) {
    print_section("3. Farklı Process'lerin Nice Değerini Okuma");
    
    // Kendi process'imiz
    print_process_info("Kendi process'imiz", getpid());
    
    // Parent process (genellikle shell)
    print_process_info("Parent process (shell)", getppid());
    
    // init process (PID 1)
    print_process_info("init process", 1);
    
    printf(COLOR_YELLOW "\nNot: Diğer process'lerin nice değerini okumak için genellikle\n" COLOR_RESET);
    printf(COLOR_YELLOW "yetki gerekmez, ancak değiştirmek için yetki gerekebilir.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 4: fork() ile Çocuk Process'te Nice Değeri Değiştirme
 * ============================================================================ */

void demo_fork_and_nice(void) {
    print_section("4. fork() ile Çocuk Process'te Nice Değeri Değiştirme");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Çocuk process
        printf("Çocuk process (PID: %d) başladı\n", getpid());
        printf("Çocuk başlangıç nice değeri: %d\n", get_nice_value(0));
        
        // Çocuğun önceliğini düşür (daha nazik yap)
        printf("\nÇocuk: nice(10) çağrılıyor...\n");
        nice(10);
        printf("Çocuk yeni nice değeri: %d\n", get_nice_value(0));
        
        // Biraz iş yap
        busy_work(2, "Çocuk");
        exit(0);
        
    } else if (pid > 0) {
        // Ebeveyn process
        printf("Ebeveyn process (PID: %d)\n", getpid());
        printf("Ebeveyn nice değeri: %d\n", get_nice_value(0));
        
        // Ebeveynin önceliğini yükselt (root gerekir)
        printf("\nEbeveyn: setpriority(0, 0, -5) deneniyor...\n");
        if (setpriority(PRIO_PROCESS, 0, -5) == -1) {
            perror("setpriority (ebeveyn)");
            printf(COLOR_RED "Normal kullanıcı önceliği yükseltemez!\n" COLOR_RESET);
        }
        
        // Çocuğu bekle
        wait(NULL);
        printf("\nÇocuk process tamamlandı.\n");
    }
}

/* ============================================================================
 * BÖLÜM 5: Process Grubu ve Kullanıcı Bazlı Nice Değeri
 * ============================================================================ */

void demo_priority_scopes(void) {
    print_section("5. Process Grubu ve Kullanıcı Bazlı Nice Değeri");
    
    printf(COLOR_CYAN "getpriority() ve setpriority() üç farklı scope'da çalışabilir:\n\n" COLOR_RESET);
    
    printf("┌─────────────────┬────────────────────────────────────────────────┐\n");
    printf("│ Scope           │ Açıklama                                       │\n");
    printf("├─────────────────┼────────────────────────────────────────────────┤\n");
    printf("│ PRIO_PROCESS    │ Tek bir process'in nice değeri                 │\n");
    printf("│ PRIO_PGRP       │ Bir process grubundaki tüm process'ler         │\n");
    printf("│ PRIO_USER       │ Bir kullanıcının tüm process'leri              │\n");
    printf("└─────────────────┴────────────────────────────────────────────────┘\n");
    
    // Process grubu örneği
    printf(COLOR_GREEN "\nProcess grubu (PGRP) nice değeri:\n" COLOR_RESET);
    int pgrp_nice = getpriority(PRIO_PGRP, 0);
    if (pgrp_nice != -1 || errno == 0) {
        printf("  Process grubunun nice değeri: %d\n", pgrp_nice);
    }
    
    // Kullanıcı bazlı nice değeri
    printf(COLOR_GREEN "\nKullanıcı bazlı nice değeri:\n" COLOR_RESET);
    int user_nice = getpriority(PRIO_USER, getuid());
    if (user_nice != -1 || errno == 0) {
        printf("  Kullanıcının (UID: %d) nice değeri: %d\n", getuid(), user_nice);
    }
}

/* ============================================================================
 * BÖLÜM 6: Nice Değerinin Sınırları (-20 ile +19)
 * ============================================================================ */

void demo_nice_limits(void) {
    print_section("6. Nice Değerinin Sınırları (-20 ile +19)");
    
    printf(COLOR_CYAN "Nice değerleri -20 (en yüksek öncelik) ile +19 (en düşük öncelik) arasında değişir.\n\n" COLOR_RESET);
    
    // Maksimum nice değerini dene
    printf("Maksimum nice değeri (+19) deneniyor...\n");
    int max_nice = 19;
    int old_nice = get_nice_value(0);
    
    if (setpriority(PRIO_PROCESS, 0, max_nice) == -1) {
        perror("setpriority max");
    } else {
        printf("  Yeni nice değeri: %d\n", get_nice_value(0));
    }
    
    // Minimum nice değerini dene (root gerekir)
    printf("\nMinimum nice değeri (-20) deneniyor...\n");
    if (setpriority(PRIO_PROCESS, 0, -20) == -1) {
        perror("setpriority min");
        printf(COLOR_RED "  Not: -20 değeri genellikle root yetkisi gerektirir!\n" COLOR_RESET);
    } else {
        printf("  Yeni nice değeri: %d (root yetkisi ile!)\n", get_nice_value(0));
    }
    
    // Eski değere geri dön
    setpriority(PRIO_PROCESS, 0, old_nice);
    printf("\nEski nice değerine dönüldü: %d\n", get_nice_value(0));
}

/* ============================================================================
 * BÖLÜM 7: nice() ve getpriority() Hata Yönetimi
 * ============================================================================ */

void demo_error_handling(void) {
    print_section("7. Hata Yönetimi");
    
    printf(COLOR_CYAN "getpriority() ve nice() hata durumunda -1 döndürür.\n" COLOR_RESET);
    printf(COLOR_YELLOW "Ancak -1 geçerli bir nice değeri de olabilir!\n\n" COLOR_RESET);
    
    // Doğru hata kontrolü
    printf("Doğru hata kontrolü:\n");
    errno = 0;
    int nice_val = getpriority(PRIO_PROCESS, 0);
    
    if (nice_val == -1 && errno != 0) {
        perror("getpriority");
    } else {
        printf("  Nice değeri: %d (geçerli)\n", nice_val);
    }
    
    // Geçersiz PID ile çağrı
    printf("\nGeçersiz PID ile getpriority():\n");
    errno = 0;
    nice_val = getpriority(PRIO_PROCESS, 999999);
    
    if (nice_val == -1 && errno != 0) {
        perror("  getpriority");
        printf(COLOR_RED "  Hata kodu: %d (%s)\n" COLOR_RESET, errno, strerror(errno));
    }
}

/* ============================================================================
 * BÖLÜM 8: Performans Karşılaştırması (Farklı Nice Değerleri)
 * ============================================================================ */

void demo_performance_comparison(void) {
    print_section("8. Performans Karşılaştırması (Farklı Nice Değerleri)");
    
    printf(COLOR_YELLOW "Bu bölümde, farklı nice değerlerine sahip process'lerin\n" COLOR_RESET);
    printf(COLOR_YELLOW "CPU zamanı dağılımını gözlemleyebilirsiniz.\n\n" COLOR_RESET);
    
    pid_t pids[3];
    int nice_values[] = {19, 0, -20};  // Düşük, normal, yüksek öncelik
    
    for (int i = 0; i < 3; i++) {
        pids[i] = fork();
        
        if (pids[i] == 0) {
            // Çocuk process
            printf("Çocuk %d (PID: %d) nice değeri %d ile başlıyor\n", 
                   i, getpid(), nice_values[i]);
            
            // Nice değerini ayarla
            if (setpriority(PRIO_PROCESS, 0, nice_values[i]) == -1) {
                if (nice_values[i] < 0) {
                    printf(COLOR_RED "  Çocuk %d: Nice değeri %d ayarlanamadı (root gerekir)\n" COLOR_RESET, i, nice_values[i]);
                }
            }
            
            printf("  Çocuk %d yeni nice değeri: %d\n", i, get_nice_value(0));
            
            // Yoğun hesaplama yap
            busy_work(3, "Çocuk");
            exit(0);
        }
    }
    
    // Ebeveyn: tüm çocukları bekle
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }
    
    printf(COLOR_GREEN "\nNot: Aynı anda çalışan process'lerde, nice değeri DÜŞÜK olanlar\n" COLOR_RESET);
    printf(COLOR_GREEN "(-20) daha FAZLA CPU zamanı alır, yüksek olanlar (19) daha AZ alır.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 9: renice Komutunun C'de Taklidi
 * ============================================================================ */

void demo_renice(void) {
    print_section("9. renice Komutunun Taklidi");
    
    printf(COLOR_CYAN "renice, çalışan bir process'in nice değerini değiştirir.\n\n" COLOR_RESET);
    
    // Önce bir çocuk process oluştur
    pid_t pid = fork();
    
    if (pid == 0) {
        // Çocuk: uzun süre çalışan işlem
        printf("Çocuk process (PID: %d) başladı. 10 saniye çalışacak.\n", getpid());
        printf("Başlangıç nice değeri: %d\n", get_nice_value(0));
        
        // 10 saniye boyunca çalış
        for (int i = 0; i < 10; i++) {
            printf("  Çocuk: %d saniye geçti...\n", i + 1);
            sleep(1);
        }
        exit(0);
        
    } else {
        // Ebeveyn: çocuğun nice değerini değiştir
        sleep(2);  // Çocuğun başlamasını bekle
        
        printf("\nEbeveyn: Çocuğun (PID: %d) nice değerini değiştiriyor...\n", pid);
        printf("Çocuğun şu anki nice değeri: %d\n", get_nice_value(pid));
        
        // Çocuğun nice değerini 15 yap
        printf("setpriority(PRIO_PROCESS, %d, 15) çağrılıyor...\n", pid);
        if (setpriority(PRIO_PROCESS, pid, 15) == -1) {
            perror("setpriority");
        } else {
            printf("Çocuğun yeni nice değeri: %d\n", get_nice_value(pid));
        }
        
        // Çocuğu bekle
        wait(NULL);
    }
}

/* ============================================================================
 * BÖLÜM 10: Özet ve Bilgi Tablosu
 * ============================================================================ */

void demo_summary(void) {
    print_section("10. Özet: Nice Değeri ve İşlem Önceliği");
    
    printf(COLOR_CYAN "┌─────────────────────────────────────────────────────────────────────────┐\n");
    printf(COLOR_CYAN "│                         NICE DEĞERİ ÖZET TABLOSU                        │\n");
    printf(COLOR_CYAN "├─────────────────┬─────────────────────┬─────────────────────────────────┤\n");
    printf(COLOR_CYAN "│ Nice Değeri     │ Anlamı              │ Açıklama                        │\n");
    printf(COLOR_CYAN "├─────────────────┼─────────────────────┼─────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ -20             │ En yüksek öncelik  │ En kaba (root gerekir)            │\n");
    printf("│ -10             │ Yüksek öncelik     │ Çok kaba (root gerekir)            │\n");
    printf("│ 0               │ Normal öncelik     │ Varsayılan değer                  │\n");
    printf("│ 10              │ Düşük öncelik      │ Nazik                             │\n");
    printf("│ 19              │ En düşük öncelik   │ En nazik                          │\n");
    printf(COLOR_CYAN "├─────────────────┼─────────────────────┼─────────────────────────────────┤\n");
    printf(COLOR_CYAN "│ Fonksiyon       │ Açıklama            │ Prototip                        │\n");
    printf(COLOR_CYAN "├─────────────────┼─────────────────────┼─────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ nice()          │ Değeri artır       │ int nice(int inc);                │\n");
    printf("│ getpriority()   │ Değeri oku         │ int getpriority(int which, id_t who);│\n");
    printf("│ setpriority()   │ Değeri ayarla      │ int setpriority(int which, id_t who, int prio);│\n");
    printf(COLOR_CYAN "└─────────────────┴─────────────────────┴─────────────────────────────────┘\n" COLOR_RESET);
    
    printf(COLOR_GREEN "\n📌 Önemli Notlar:\n" COLOR_RESET);
    printf("  • Normal kullanıcılar nice değerini SADECE ARTIRABİLİR (daha nazik yapabilir)\n");
    printf("  • Nice değerini azaltmak (önceliği yükseltmek) için root yetkisi gerekir\n");
    printf("  • nice() ve getpriority() -1 döndürebilir, bu nedenle errno kontrolü ŞARTTIR\n");
    printf("  • fork() ile oluşturulan çocuk process, ebeveynin nice değerini miras alır\n");
    printf("  • nice değeri, process'in gerçek önceliğini doğrudan belirlemez, sadece etkiler\n");
}

/* ============================================================================
 * KOMUT SATIRI ARGÜMAN İŞLEME
 * ============================================================================ */

void print_usage(const char *progname) {
    printf("Kullanım: %s [seçenekler]\n\n", progname);
    printf("Seçenekler:\n");
    printf("  --all           Tüm demoları çalıştır (varsayılan)\n");
    printf("  --basic         Sadece nice() temel kullanımı\n");
    printf("  --set           setpriority() ile doğrudan ayarlama\n");
    printf("  --read          Diğer process'lerin nice değerini okuma\n");
    printf("  --fork          fork() ile çocuk process'te nice değeri\n");
    printf("  --scope         Process grubu ve kullanıcı bazlı nice değeri\n");
    printf("  --limits        Nice değerinin sınırları (-20 ile +19)\n");
    printf("  --errors        Hata yönetimi\n");
    printf("  --perf          Performans karşılaştırması\n");
    printf("  --renice        renice komutunun taklidi\n");
    printf("  --help          Bu yardım mesajını gösterir\n");
}

/* ============================================================================
 * ANA FONKSİYON
 * ============================================================================ */

int main(int argc, char *argv[]) {
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║              nice() ve setpriority() SİSTEM ÇAĞRILARI            ║\n");
    printf("║                     (İşlem Önceliği Yönetimi)                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    // Komut satırı argümanlarını kontrol et
    int run_all = 1;
    int run_basic = 0, run_set = 0, run_read = 0, run_fork = 0;
    int run_scope = 0, run_limits = 0, run_errors = 0, run_perf = 0, run_renice = 0;
    
    if (argc > 1) {
        run_all = 0;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--basic") == 0) run_basic = 1;
            else if (strcmp(argv[i], "--set") == 0) run_set = 1;
            else if (strcmp(argv[i], "--read") == 0) run_read = 1;
            else if (strcmp(argv[i], "--fork") == 0) run_fork = 1;
            else if (strcmp(argv[i], "--scope") == 0) run_scope = 1;
            else if (strcmp(argv[i], "--limits") == 0) run_limits = 1;
            else if (strcmp(argv[i], "--errors") == 0) run_errors = 1;
            else if (strcmp(argv[i], "--perf") == 0) run_perf = 1;
            else if (strcmp(argv[i], "--renice") == 0) run_renice = 1;
            else if (strcmp(argv[i], "--all") == 0) run_all = 1;
            else if (strcmp(argv[i], "--help") == 0) {
                print_usage(argv[0]);
                return 0;
            } else {
                printf(COLOR_RED "Bilinmeyen seçenek: %s\n" COLOR_RESET, argv[i]);
                print_usage(argv[0]);
                return 1;
            }
        }
    }
    
    if (run_all || run_basic) demo_nice_basic();
    if (run_all || run_set) demo_setpriority();
    if (run_all || run_read) demo_read_other_process();
    if (run_all || run_fork) demo_fork_and_nice();
    if (run_all || run_scope) demo_priority_scopes();
    if (run_all || run_limits) demo_nice_limits();
    if (run_all || run_errors) demo_error_handling();
    if (run_all || run_perf) demo_performance_comparison();
    if (run_all || run_renice) demo_renice();
    if (run_all) demo_summary();
    
    print_section("PROGRAM SONLANDI");
    printf(COLOR_GREEN "\nNot: Bu programdaki bazı işlemler root yetkisi gerektirebilir.\n" COLOR_RESET);
    printf(COLOR_YELLOW "Özellikle nice değerini düşürmek (önceliği yükseltmek) için sudo kullanın:\n" COLOR_RESET);
    printf("  sudo ./nice_demo --limits\n");
    
    return 0;
}