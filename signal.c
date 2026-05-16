/*
 * ============================================================================
 * signal() FONKSİYONU - DETAYLI EĞİTİM KODU
 * ============================================================================
 * 
 * Bu program, signal() fonksiyonunun tüm yönlerini gösterir:
 *   - Sinyal yakalama (catching)
 *   - Sinyal yok sayma (ignoring)
 *   - Varsayılan davranışa dönme
 *   - Farklı sinyal türleri (SIGINT, SIGTERM, SIGUSR1, vb.)
 *   - signal()'ın sınırlamaları ve sigaction() ile karşılaştırması
 * 
 * Derleme: gcc -o signal_demo signal_demo.c
 * Çalıştır: ./signal_demo
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/* ANSI renk kodları */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

/* ============================================================================
 * GLOBAL DEĞİŞKENLER
 * ============================================================================ */

volatile sig_atomic_t sigint_count = 0;      // SIGINT kaç kez geldi
volatile sig_atomic_t sigusr1_received = 0;  // SIGUSR1 alındı mı?
volatile sig_atomic_t last_signal = 0;       // Son alınan sinyal

/* ============================================================================
 * YARDIMCI FONKSİYONLAR
 * ============================================================================ */

void print_separator(char c, int length) {
    for (int i = 0; i < length; i++) putchar(c);
    putchar('\n');
}

void print_header(const char *title) {
    printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_YELLOW "  %s\n" COLOR_RESET, title);
    printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
}

void print_signal_name(int sig) {
    switch (sig) {
        case SIGINT:  printf("SIGINT (2)"); break;
        case SIGTERM: printf("SIGTERM (15)"); break;
        case SIGUSR1: printf("SIGUSR1 (10)"); break;
        case SIGUSR2: printf("SIGUSR2 (12)"); break;
        case SIGALRM: printf("SIGALRM (14)"); break;
        default:      printf("Sinyal %d", sig); break;
    }
}

/* ============================================================================
 * SİNYAL İŞLEYİCİLERİ (HANDLERS)
 * ============================================================================ */

/**
 * SIGINT işleyicisi - Ctrl+C ile tetiklenir
 */
void sigint_handler(int sig) {
    sigint_count++;
    last_signal = sig;
    printf(COLOR_YELLOW "\n[!] SIGINT (Ctrl+C) yakalandı! (Sayı: %d)\n" COLOR_RESET, sigint_count);
    printf(COLOR_CYAN "    Bu mesaj signal() handler'ından geliyor.\n" COLOR_RESET);
    fflush(stdout);
    
    // 3 kez SIGINT alınırsa varsayılan davranışa dön
    if (sigint_count >= 3) {
        printf(COLOR_RED "\n[!] 3 kez SIGINT alındı. Varsayılan davranışa dönülüyor...\n" COLOR_RESET);
        signal(SIGINT, SIG_DFL);
    }
}

/**
 * SIGUSR1 işleyicisi - Kullanıcı tanımlı sinyal
 */
void sigusr1_handler(int sig) {
    sigusr1_received = 1;
    last_signal = sig;
    printf(COLOR_GREEN "\n[✓] SIGUSR1 alındı! Bu kullanıcı tanımlı bir sinyaldir.\n" COLOR_RESET);
    printf(COLOR_CYAN "    Özel amaçlı iletişim için kullanılabilir.\n" COLOR_RESET);
    fflush(stdout);
}

/**
 * SIGUSR2 işleyicisi
 */
void sigusr2_handler(int sig) {
    last_signal = sig;
    printf(COLOR_MAGENTA "\n[★] SIGUSR2 alındı! İkinci kullanıcı sinyali.\n" COLOR_RESET);
    fflush(stdout);
}

/**
 * SIGALRM işleyicisi - alarm() süresi dolduğunda
 */
void sigalrm_handler(int sig) {
    last_signal = sig;
    printf(COLOR_YELLOW "\n[⏰] SIGALRM (Alarm) çaldı! alarm() süresi doldu.\n" COLOR_RESET);
    fflush(stdout);
}

/**
 * SIGTERM işleyicisi - Kapatma sinyali (nazik sonlandırma)
 */
void sigterm_handler(int sig) {
    last_signal = sig;
    printf(COLOR_RED "\n[⚠] SIGTERM alındı! Bu bir sonlandırma sinyalidir.\n" COLOR_RESET);
    printf(COLOR_YELLOW "    5 saniye içinde program sonlanacak...\n" COLOR_RESET);
    
    // Temizlik yap (örnek)
    for (int i = 5; i > 0; i--) {
        printf("  %d saniye kaldı...\r", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
    
    printf(COLOR_GREEN "Temizlik tamamlandı. Program sonlanıyor.\n" COLOR_RESET);
    exit(0);
}

/* ============================================================================
 * BÖLÜM 1: signal() ile Sinyal Yakalama (Catching)
 * ============================================================================ */

void demo_signal_catch(void) {
    print_header("1. signal() ile Sinyal Yakalama (Catching)");
    
    printf(COLOR_CYAN "signal() fonksiyonu bir sinyale işleyici (handler) atar.\n");
    printf(COLOR_CYAN "Prototip: void (*signal(int sig, void (*func)(int)))(int);\n\n");
    
    // SIGINT için işleyici kur (Ctrl+C)
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    printf(COLOR_GREEN "✓ SIGINT işleyici kuruldu. Şimdi Ctrl+C tuşlarına basabilirsiniz.\n" COLOR_RESET);
    
    // SIGUSR1 için işleyici kur
    if (signal(SIGUSR1, sigusr1_handler) == SIG_ERR) {
        perror("signal");
    } else {
        printf(COLOR_GREEN "✓ SIGUSR1 işleyici kuruldu. 'kill -USR1 %d' ile test edin.\n" COLOR_RESET, getpid());
    }
    
    // SIGALRM için işleyici kur
    if (signal(SIGALRM, sigalrm_handler) == SIG_ERR) {
        perror("signal");
    } else {
        printf(COLOR_GREEN "✓ SIGALRM işleyici kuruldu. 5 saniye sonra alarm çalacak.\n" COLOR_RESET);
    }
    
    printf(COLOR_YELLOW "\n10 saniye bekleniyor... Sinyalleri göndermek için başka bir terminal kullanın.\n" COLOR_RESET);
    printf(COLOR_CYAN "  kill -USR1 %d\n", getpid());
    printf(COLOR_CYAN "  kill -ALRM %d\n", getpid());
    printf(COLOR_CYAN "  kill -INT %d (veya Ctrl+C)\n\n", getpid());
    
    // 5 saniye sonra alarm kur
    alarm(5);
    
    // 10 saniye bekle (sinyalleri yakalamak için)
    for (int i = 10; i > 0; i--) {
        printf("  %d saniye kaldı...\r", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
    
    printf(COLOR_GREEN "\n✓ Toplam SIGINT sayısı: %d\n" COLOR_RESET, sigint_count);
    if (sigusr1_received) {
        printf(COLOR_GREEN "✓ SIGUSR1 alındı.\n" COLOR_RESET);
    }
}

/* ============================================================================
 * BÖLÜM 2: Sinyal Yok Sayma (Ignoring)
 * ============================================================================ */

void demo_signal_ignore(void) {
    print_header("2. Sinyal Yok Sayma (Ignoring)");
    
    printf(COLOR_CYAN "SIG_IGN ile bir sinyali yok sayabiliriz (ignore).\n");
    printf(COLOR_CYAN "Bu durumda sinyal geldiğinde hiçbir şey olmaz.\n\n");
    
    // SIGINT'i yok say (Ctrl+C işe yaramayacak)
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
        perror("signal");
    } else {
        printf(COLOR_YELLOW "⚠ SIGINT artık YOK SAYILIYOR! Ctrl+C işe yaramayacak.\n" COLOR_RESET);
    }
    
    printf(COLOR_CYAN "\n10 saniye boyunca Ctrl+C işe yaramayacak. Deneyebilirsiniz.\n" COLOR_RESET);
    
    for (int i = 10; i > 0; i--) {
        printf("  %d saniye kaldı (Ctrl+C çalışmaz)...\r", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
    
    // Varsayılan davranışa geri dön
    if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
        perror("signal");
    } else {
        printf(COLOR_GREEN "\n✓ SIGINT varsayılan davranışa geri döndü. Artık Ctrl+C çalışır.\n" COLOR_RESET);
    }
}

/* ============================================================================
 * BÖLÜM 3: Varsayılan Davranışa Dönme (Default)
 * ============================================================================ */

void demo_signal_default(void) {
    print_header("3. Varsayılan Davranışa Dönme (SIG_DFL)");
    
    printf(COLOR_CYAN "SIG_DFL ile bir sinyalin varsayılan davranışını geri yükleyebiliriz.\n\n");
    
    printf("Sinyallerin varsayılan davranışları:\n");
    printf("  - SIGINT  (2)  : Programı sonlandırır\n");
    printf("  - SIGTERM (15) : Programı sonlandırır\n");
    printf("  - SIGCHLD (17) : Yok sayar (ignore)\n");
    printf("  - SIGSTOP (19) : Programı durdurur (yakalanamaz)\n\n");
    
    // Önce SIGINT'i yakala
    signal(SIGINT, sigint_handler);
    printf(COLOR_GREEN "✓ SIGINT işleyici kuruldu. Şimdi Ctrl+C mesaj gösterecek.\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "\n5 saniye boyunca Ctrl+C mesaj gösterecek...\n" COLOR_RESET);
    sleep(5);
    
    // Varsayılan davranışa dön
    signal(SIGINT, SIG_DFL);
    printf(COLOR_RED "\n✓ SIGINT varsayılan davranışa döndü. Şimdi Ctrl+C programı sonlandıracak!\n" COLOR_RESET);
    printf(COLOR_YELLOW "Dikkat: 5 saniye sonra program sonlanacak. Ctrl+C ile sonlandırabilirsiniz.\n" COLOR_RESET);
    
    sleep(5);
    printf(COLOR_GREEN "Program normal şekilde sonlandı.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 4: Farklı Sinyal Türleri
 * ============================================================================ */

void demo_signal_types(void) {
    print_header("4. Farklı Sinyal Türleri");
    
    printf(COLOR_CYAN "signal() ile farklı sinyal türlerine işleyici atayabiliriz.\n\n");
    
    // SIGUSR2 işleyicisi
    if (signal(SIGUSR2, sigusr2_handler) == SIG_ERR) {
        perror("signal");
    } else {
        printf(COLOR_GREEN "✓ SIGUSR2 işleyici kuruldu.\n" COLOR_RESET);
    }
    
    // SIGTERM işleyicisi (nazik sonlandırma)
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR) {
        perror("signal");
    } else {
        printf(COLOR_GREEN "✓ SIGTERM işleyici kuruldu.\n" COLOR_RESET);
    }
    
    printf(COLOR_YELLOW "\nFarklı sinyalleri test etmek için başka bir terminalde:\n" COLOR_RESET);
    printf(COLOR_CYAN "  kill -USR2 %d\n", getpid());
    printf(COLOR_CYAN "  kill -TERM %d  (nazik sonlandırma)\n", getpid());
    printf(COLOR_CYAN "  kill -INT %d   (veya Ctrl+C)\n\n", getpid());
    
    printf(COLOR_CYAN "15 saniye bekleniyor...\n" COLOR_RESET);
    for (int i = 15; i > 0; i--) {
        printf("  %d saniye kaldı...\r", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
    
    if (last_signal) {
        printf(COLOR_GREEN "\n✓ Son alınan sinyal: ");
        print_signal_name(last_signal);
        printf(COLOR_RESET "\n");
    }
}

/* ============================================================================
 * BÖLÜM 5: signal()'ın Sınırlamaları
 * ============================================================================ */

void demo_signal_limitations(void) {
    print_header("5. signal()'ın Sınırlamaları");
    
    printf(COLOR_RED "signal() fonksiyonunun bazı sınırlamaları vardır:\n\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "1. Taşınabilirlik (Portability):\n" COLOR_RESET);
    printf("   - signal()'ın davranışı farklı Unix sistemlerinde değişebilir.\n");
    printf("   - Bazı sistemlerde sinyal işleyicisi çağrıldıktan sonra otomatik olarak\n");
    printf("     varsayılan davranışa sıfırlanır.\n\n");
    
    printf(COLOR_YELLOW "2. Sinyal Maskesi:\n" COLOR_RESET);
    printf("   - signal() işleyici çalışırken hangi sinyallerin bloklanacağını\n");
    printf("     belirlememize izin vermez.\n\n");
    
    printf(COLOR_YELLOW "3. SA_RESTART:\n" COLOR_RESET);
    printf("   - signal() ile kesilen sistem çağrılarının otomatik yeniden başlatılmasını\n");
    printf("     kontrol edemeyiz.\n\n");
    
    printf(COLOR_YELLOW "4. Ek Bilgiler:\n" COLOR_RESET);
    printf("   - signal() işleyicisine sinyal hakkında ek bilgi (siginfo_t) gönderilemez.\n\n");
    
    printf(COLOR_CYAN "Bu nedenlerle, modern programlarda signal() yerine sigaction() tercih edilir.\n" COLOR_RESET);
    printf(COLOR_GREEN "sigaction() daha güçlü, taşınabilir ve esnektir.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 6: signal() vs sigaction() Karşılaştırması
 * ============================================================================ */

void demo_signal_vs_sigaction(void) {
    print_header("6. signal() vs sigaction() Karşılaştırması");
    
    printf(COLOR_CYAN "┌─────────────────┬─────────────────────────────────────────────────────┐\n");
    printf(COLOR_CYAN "│ signal()        │ sigaction()                                         │\n");
    printf(COLOR_CYAN "├─────────────────┼─────────────────────────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ Basit, eski API  │ Gelişmiş, modern API                                     │\n");
    printf("│ Taşınabilir değil│ Taşınabilir (POSIX)                                      │\n");
    printf("│ Sinyal maskesi   │ Sinyal maskesi belirlenebilir                           │\n");
    printf("│ yok              │                                                           │\n");
    printf("│ SA_RESTART       │ SA_RESTART ayarlanabilir                                │\n");
    printf("│ yok              │                                                           │\n");
    printf("│ Ek bilgi yok     │ siginfo_t ile ek bilgi alınabilir                       │\n");
    printf("│ Handler          │ Handler her seferinde yeniden kurulur (bazı sistemlerde)│\n");
    printf("│ tekrar kurulabilir│ SA_RESETHAND ile kontrol edilebilir                     │\n");
    printf(COLOR_CYAN "└─────────────────┴─────────────────────────────────────────────────────┘\n" COLOR_RESET);
    
    printf(COLOR_GREEN "\nÖneri: Yeni programlarda her zaman sigaction() kullanın!\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 7: Yakalanamayan Sinyaller
 * ============================================================================ */

void demo_uncatchable_signals(void) {
    print_header("7. Yakalanamayan Sinyaller");
    
    printf(COLOR_RED "Bazı sinyaller signal() ile yakalanamaz:\n\n" COLOR_RESET);
    
    // SIGKILL (9) - Yakalanamaz
    if (signal(SIGKILL, sigint_handler) == SIG_ERR) {
        printf(COLOR_YELLOW "  ✗ SIGKILL (9) : Yakalanamaz!\n" COLOR_RESET);
    }
    
    // SIGSTOP (19) - Yakalanamaz
    if (signal(SIGSTOP, sigint_handler) == SIG_ERR) {
        printf(COLOR_YELLOW "  ✗ SIGSTOP (19): Yakalanamaz!\n" COLOR_RESET);
    }
    
    printf(COLOR_CYAN "\nBu sinyaller her zaman varsayılan davranışlarını gösterir:\n" COLOR_RESET);
    printf("  - SIGKILL: Process'i anında sonlandırır\n");
    printf("  - SIGSTOP: Process'i anında durdurur\n");
}

/* ============================================================================
 * BÖLÜM 8: Özet Tablo
 * ============================================================================ */

void demo_summary(void) {
    print_header("8. Özet: signal() Kullanım Tablosu");
    
    printf(COLOR_CYAN "┌─────────────────────────────────────────────────────────────────────────┐\n");
    printf(COLOR_CYAN "│                      signal() KULLANIM TABLOSU                          │\n");
    printf(COLOR_CYAN "├─────────────────────────────┬───────────────────────────────────────────┤\n");
    printf(COLOR_CYAN "│ Kullanım                    │ Açıklama                                  │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ signal(SIGINT, SIG_DFL)      │ SIGINT'in varsayılan davranışını kullan          │\n");
    printf("│ signal(SIGINT, SIG_IGN)      │ SIGINT'i yok say (ignore)                         │\n");
    printf("│ signal(SIGINT, handler)      │ SIGINT geldiğinde handler fonksiyonunu çağır     │\n");
    printf("│ signal(SIGKILL, handler)     │ ❌ HATA! SIGKILL yakalanamaz                      │\n");
    printf("│ signal(SIGSTOP, handler)     │ ❌ HATA! SIGSTOP yakalanamaz                      │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────────────────────────────┤\n");
    printf(COLOR_CYAN "│ Dönüş Değeri               │ Açıklama                                  │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ Önceki işleyici              │ signal() önceki işleyicinin adresini döndürür   │\n");
    printf("│ SIG_ERR (-1)                 │ Hata durumunda SIG_ERR döndürür                  │\n");
    printf(COLOR_CYAN "└─────────────────────────────┴───────────────────────────────────────────┘\n" COLOR_RESET);
}

/* ============================================================================
 * ANA FONKSİYON - MENÜ SİSTEMİ
 * ============================================================================ */

int main(int argc, char *argv[]) {
    int choice = 0;
    
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    signal() FONKSİYONU EĞİTİM KODU                ║\n");
    printf("║              Sinyal İşleme Mekanizması Detaylı Gösterim           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    printf(COLOR_YELLOW "\nBu program PID: %d\n" COLOR_RESET, getpid());
    printf(COLOR_CYAN "Başka bir terminalde 'kill -Sinyal %d' komutlarını deneyebilirsiniz.\n" COLOR_RESET, getpid());
    
    if (argc > 1) {
        // Komut satırından direkt test modu
        if (strcmp(argv[1], "--test") == 0) {
            demo_signal_catch();
            return 0;
        }
    }
    
    while (choice != 9) {
        printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
        printf("MENÜ:\n");
        printf("  1. signal() ile sinyal yakalama (SIGINT, SIGUSR1, SIGALRM)\n");
        printf("  2. Sinyal yok sayma (SIG_IGN)\n");
        printf("  3. Varsayılan davranışa dönme (SIG_DFL)\n");
        printf("  4. Farklı sinyal türleri (SIGUSR2, SIGTERM)\n");
        printf("  5. signal()'ın sınırlamaları\n");
        printf("  6. signal() vs sigaction() karşılaştırması\n");
        printf("  7. Yakalanamayan sinyaller (SIGKILL, SIGSTOP)\n");
        printf("  8. Özet tablo\n");
        printf("  9. Çıkış\n");
        printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
        printf("Seçiminiz (1-9): ");
        fflush(stdout);
        
        if (scanf("%d", &choice) != 1) {
            printf(COLOR_RED "Geçersiz giriş!\n" COLOR_RESET);
            while (getchar() != '\n');
            choice = 0;
            continue;
        }
        getchar();
        
        switch (choice) {
            case 1:
                sigint_count = 0;
                sigusr1_received = 0;
                demo_signal_catch();
                break;
            case 2:
                demo_signal_ignore();
                break;
            case 3:
                demo_signal_default();
                break;
            case 4:
                last_signal = 0;
                demo_signal_types();
                break;
            case 5:
                demo_signal_limitations();
                break;
            case 6:
                demo_signal_vs_sigaction();
                break;
            case 7:
                demo_uncatchable_signals();
                break;
            case 8:
                demo_summary();
                break;
            case 9:
                printf(COLOR_GREEN "\nProgram sonlandırılıyor...\n" COLOR_RESET);
                break;
            default:
                printf(COLOR_RED "Geçersiz seçim! (1-9 arası)\n" COLOR_RESET);
        }
    }
    
    return 0;
}