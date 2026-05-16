/*
 * ============================================================================
 * SİNYALLER (SIGNALS) - DETAYLI EĞİTİM KODU
 * ============================================================================
 * 
 * Bu program, Unix/Linux sinyal mekanizmasını tüm yönleriyle gösterir:
 *   - Sinyal yakalama (signal catching)
 *   - Sinyal gönderme (kill, raise)
 *   - Alarm sinyali (SIGALRM)
 *   - Çocuk süreç sinyali (SIGCHLD)
 *   - Sinyal bloklama (sigprocmask)
 *   - Sinyal bekletme (sigsuspend)
 *   - Kullanıcı tanımlı sinyaller (SIGUSR1, SIGUSR2)
 * 
 * Derleme: gcc -o signals_demo signals_demo.c
 * Çalıştır: ./signals_demo
 * ============================================================================
 */
#include <sys/time.h>   // itimerval, setitimer, ITIMER_REAL için
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

/* ANSI renk kodları (daha iyi görsel ayrım için) */
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

volatile sig_atomic_t sigint_count = 0;      // SIGINT kaç kez geldi (volatile atomic)
volatile sig_atomic_t sigusr1_received = 0;  // SIGUSR1 alındı mı?
volatile sig_atomic_t alarm_triggered = 0;   // Alarm çaldı mı?
volatile sig_atomic_t child_terminated = 0;  // Çocuk süreç bitti mi?

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
        case SIGCHLD: printf("SIGCHLD (17)"); break;
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
    printf(COLOR_YELLOW "\n[!] SIGINT (Ctrl+C) yakalandı! (sayı: %d)\n" COLOR_RESET, sigint_count);
    printf(COLOR_CYAN "    Not: signal() veya sigaction() ile yakalanabilir.\n" COLOR_RESET);
    
    if (sigint_count >= 3) {
        printf(COLOR_RED "\n[!] 3 kez SIGINT alındı. Varsayılan davranışa dönülüyor...\n" COLOR_RESET);
        // Eski davranışı geri yükle (default)
        signal(SIGINT, SIG_DFL);
    }
    fflush(stdout);
}

/**
 * SIGUSR1 işleyicisi - Kullanıcı tanımlı sinyal
 */
void sigusr1_handler(int sig) {
    sigusr1_received = 1;
    printf(COLOR_GREEN "\n[✓] SIGUSR1 alındı! Bu kullanıcı tanımlı bir sinyaldir.\n" COLOR_RESET);
    printf(COLOR_CYAN "    Özel amaçlı iletişim için kullanılabilir.\n" COLOR_RESET);
    fflush(stdout);
}

/**
 * SIGALRM işleyicisi - alarm() süresi dolduğunda
 */
void sigalrm_handler(int sig) {
    alarm_triggered = 1;
    printf(COLOR_YELLOW "\n[⏰] SIGALRM (Alarm) çaldı! alarm() süresi doldu.\n" COLOR_RESET);
    printf(COLOR_CYAN "    Bu sinyal, zamanlayıcı işlemleri için kullanılır.\n" COLOR_RESET);
    fflush(stdout);
}

/**
 * SIGCHLD işleyicisi - Çocuk süreç durum değiştirdiğinde
 */
void sigchld_handler(int sig) {
    child_terminated = 1;
    printf(COLOR_GREEN "\n[👶] SIGCHLD alındı! Bir çocuk süreç sonlandı.\n" COLOR_RESET);
    
    // Zombi oluşmasını önlemek için waitpid ile temizle (non-blocking)
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
    
    fflush(stdout);
}

/**
 * SIGTERM işleyicisi - Kapatma sinyali
 */
void sigterm_handler(int sig) {
    printf(COLOR_RED "\n[⚠] SIGTERM alındı! Program sonlandırılıyor...\n" COLOR_RESET);
    printf(COLOR_CYAN "    Bu nazik bir sonlandırma sinyalidir.\n" COLOR_RESET);
    exit(0);
}

/* ============================================================================
 * BÖLÜM 1: signal() ile Basit Sinyal Yakalama
 * ============================================================================ */

void demo_signal_basic(void) {
    print_header("1. signal() ile Basit Sinyal Yakalama");
    
    printf(COLOR_CYAN "signal() fonksiyonu, bir sinyale işleyici (handler) atar.\n" COLOR_RESET);
    printf(COLOR_CYAN "Prototip: void (*signal(int sig, void (*func)(int)))(int);\n\n" COLOR_RESET);
    
    // SIGINT için işleyici kur (Ctrl+C)
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    printf(COLOR_GREEN "✓ SIGINT işleyici kuruldu. Şimdi Ctrl+C tuşlarına basabilirsiniz.\n" COLOR_RESET);
    
    // SIGTERM için işleyici kur
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    printf(COLOR_GREEN "✓ SIGTERM işleyici kuruldu. 'kill -TERM %d' ile test edebilirsiniz.\n" COLOR_RESET, getpid());
    
    printf(COLOR_YELLOW "\n10 saniye bekleniyor... Ctrl+C veya kill deneyin.\n" COLOR_RESET);
    
    // 10 saniye bekle (sinyalleri yakalamak için)
    for (int i = 10; i > 0; i--) {
        printf("  %d saniye kaldı...\r", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
    
    printf(COLOR_GREEN "\n✓ Toplam SIGINT sayısı: %d\n" COLOR_RESET, sigint_count);
}

/* ============================================================================
 * BÖLÜM 2: sigaction() ile Gelişmiş Sinyal Yakalama
 * ============================================================================ */

void demo_sigaction_advanced(void) {
    print_header("2. sigaction() ile Gelişmiş Sinyal Yakalama");
    
    printf(COLOR_CYAN "sigaction() signal()'dan daha gelişmiştir ve tercih edilmelidir.\n");
    printf(COLOR_CYAN "Özellikleri:\n");
    printf("  - Sinyal maskesi belirleyebiliriz\n");
    printf("  - SA_RESTART ile kesilen sistem çağrılarını otomatik yeniden başlatabiliriz\n");
    printf("  - Sinyal hakkında ek bilgi alabiliriz (siginfo_t)\n\n");
    
    struct sigaction sa;
    
    // SIGUSR1 için işleyici kur
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);      // Maske boş (başka sinyal bloklanmasın)
    sa.sa_flags = SA_RESTART;      // Kesilen sistem çağrılarını yeniden başlat
    
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    printf(COLOR_GREEN "✓ SIGUSR1 işleyici kuruldu.\n" COLOR_RESET);
    
    // SIGALRM için işleyici kur
    sa.sa_handler = sigalrm_handler;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    printf(COLOR_GREEN "✓ SIGALRM işleyici kuruldu.\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "\nTest için:\n" COLOR_RESET);
    printf("  - Başka bir terminalde: kill -USR1 %d\n", getpid());
    printf("  - Bu program 5 saniye sonra alarm() gönderecek.\n");
    
    // 5 saniye sonra alarm kur
    printf(COLOR_CYAN "\n5 saniye sonra alarm çalacak...\n" COLOR_RESET);
    alarm(5);
    
    // Bekle (sinyalleri almak için)
    for (int i = 0; i < 6; i++) {
        sleep(1);
        if (sigusr1_received) {
            printf(COLOR_GREEN "  ✓ SIGUSR1 alındı! (sayfa: %d)\n" COLOR_RESET, sigusr1_received);
        }
        if (alarm_triggered) {
            printf(COLOR_YELLOW "  ⏰ Alarm çaldı!\n" COLOR_RESET);
        }
    }
}

/* ============================================================================
 * BÖLÜM 3: Sinyal Gönderme (kill, raise)
 * ============================================================================ */

void demo_signal_sending(void) {
    print_header("3. Sinyal Gönderme (kill, raise)");
    
    printf(COLOR_CYAN "Sinyal gönderme fonksiyonları:\n");
    printf("  - kill(pid, sig) : Belirli bir sürece sinyal gönderir\n");
    printf("  - raise(sig)     : Kendi sürecimize sinyal gönderir\n");
    printf("  - kill(-pgid, sig): Bir process grubuna sinyal gönderir\n\n");
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        /* ÇOCUK SÜREÇ - Kendi kendine sinyal gönderecek */
        printf("[ÇOCUK] PID: %d başladı. Kendime SIGUSR2 göndereceğim...\n", getpid());
        sleep(1);
        
        // raise() ile kendimize sinyal gönder
        printf("[ÇOCUK] raise(SIGUSR2) çağrılıyor...\n");
        raise(SIGUSR2);
        printf("[ÇOCUK] Sinyal gönderildi.\n");
        
        sleep(1);
        exit(0);
        
    } else {
        /* EBEVEYN SÜREÇ - Çocuğa sinyal gönderecek */
        printf("[EBEVEYN] PID: %d\n", getpid());
        printf("[EBEVEYN] Çocuk sürece (PID: %d) SIGUSR1 göndereceğim...\n", pid);
        sleep(1);
        
        // kill() ile çocuğa sinyal gönder
        if (kill(pid, SIGUSR1) == -1) {
            perror("kill");
        } else {
            printf("[EBEVEYN] SIGUSR1 gönderildi.\n");
        }
        
        wait(NULL);
        printf("[EBEVEYN] Çocuk süreç tamamlandı.\n");
    }
}

/* ============================================================================
 * BÖLÜM 4: SIGCHLD ve Zombi Process Önleme
 * ============================================================================ */

void demo_sigchld_handler(void) {
    print_header("4. SIGCHLD ile Zombi Process Önleme");
    
    printf(COLOR_CYAN "SIGCHLD, bir çocuk süreç sonlandığında veya durduğunda ebeveyne gönderilir.\n");
    printf(COLOR_CYAN "Bu sinyali yakalayarak zombi process'leri önleyebiliriz.\n\n");
    
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;  // Sadece sonlanma, durma değil
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return;
    }
    printf(COLOR_GREEN "✓ SIGCHLD işleyici kuruldu.\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "\n3 tane çocuk süreç oluşturuluyor...\n" COLOR_RESET);
    
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // Çocuk süreç
            printf("  [Çocuk %d] PID: %d başladı, 2 saniye sonra bitecek.\n", i, getpid());
            sleep(2);
            printf("  [Çocuk %d] Bitiuyor.\n", i);
            exit(i);
        }
    }
    
    printf(COLOR_CYAN "\nEbeveyn çalışmaya devam ediyor...\n" COLOR_RESET);
    
    // Çocukların bitmesini bekle (sinyal handler temizleyecek)
    for (int i = 0; i < 5; i++) {
        printf("  Bekleniyor... %d/5\r", i + 1);
        fflush(stdout);
        sleep(1);
        if (child_terminated) {
            printf("\n");
        }
    }
    printf("\n");
    
    printf(COLOR_GREEN "✓ Tüm çocuklar temizlendi, zombi yok!\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 5: Sinyal Bloklama (sigprocmask)
 * ============================================================================ */

void demo_signal_blocking(void) {
    print_header("5. Sinyal Bloklama (sigprocmask)");
    
    printf(COLOR_CYAN "sigprocmask() ile sinyalleri geçici olarak bloklayabiliriz.\n");
    printf(COLOR_CYAN "Bloklanan sinyaller, maske kaldırılana kadar bekletilir.\n\n");
    
    sigset_t newmask, oldmask, pending;
    
    // SIGINT'i blokla (Ctrl+C etkisiz hale gelsin)
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);
    
    printf(COLOR_YELLOW "SIGINT (Ctrl+C) bloklanıyor... 10 saniye boyunca Ctrl+C işe yaramayacak!\n" COLOR_RESET);
    
    // Mevcut maskeyi al ve yeni maskeyi ayarla
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) == -1) {
        perror("sigprocmask");
        return;
    }
    
    // 10 saniye bekle (bu sırada gelen SIGINT'ler bekletilecek)
    for (int i = 10; i > 0; i--) {
        printf("  %d saniye kaldı (Ctrl+C çalışmaz)...\r", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
    
    // Bekleyen sinyalleri kontrol et
    if (sigpending(&pending) == -1) {
        perror("sigpending");
    } else if (sigismember(&pending, SIGINT)) {
        printf(COLOR_YELLOW "⚠ Bekleyen SIGINT sinyali var!\n" COLOR_RESET);
    }
    
    // Eski maskeyi geri yükle (bloklamayı kaldır)
    printf(COLOR_GREEN "\nSIGINT bloklaması kaldırılıyor... Bekleyen sinyaller şimdi işlenecek.\n" COLOR_RESET);
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) == -1) {
        perror("sigprocmask");
        return;
    }
    
    printf(COLOR_GREEN "✓ Bloklama kaldırıldı. Bekleyen sinyaller işlendi.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 6: Sinyal Bekletme (sigsuspend)
 * ============================================================================ */

void demo_sigsuspend(void) {
    print_header("6. Sinyal Bekletme (sigsuspend)");
    
    printf(COLOR_CYAN "sigsuspend(), bir sinyal gelene kadar süreci bekletir.\n");
    printf(COLOR_CYAN "Aynı anda geçici bir sinyal maskesi de uygulayabiliriz.\n\n");
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        /* ÇOCUK - 3 saniye sonra sinyal gönder */
        sleep(3);
        printf("[ÇOCUK] Ebeveyne SIGUSR1 gönderiyorum...\n");
        kill(getppid(), SIGUSR1);
        exit(0);
        
    } else {
        /* EBEVEYN - sigsuspend ile bekle */
        sigset_t mask, oldmask;
        
        // Tüm sinyalleri blokla
        sigfillset(&mask);
        sigprocmask(SIG_SETMASK, &mask, &oldmask);
        
        printf("[EBEVEYN] Sinyal bekleniyor (sigsuspend)...\n");
        printf("[EBEVEYN] Çocuk 3 saniye sonra SIGUSR1 gönderecek.\n");
        
        // Sadece SIGUSR1'i bekle (diğer sinyaller bloklu)
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        
        // sigsuspend: bekle ve geçici maske uygula
        if (sigsuspend(&mask) == -1 && errno == EINTR) {
            printf("[EBEVEYN] Sinyal alındı, devam ediyorum.\n");
        }
        
        // Eski maskeyi geri yükle
        sigprocmask(SIG_SETMASK, &oldmask, NULL);
        
        wait(NULL);
    }
}

/* ============================================================================
 * BÖLÜM 7: Alarm ve Zamanlayıcı Sinyalleri
 * ============================================================================ */

void demo_alarm_timer(void) {
    print_header("7. Alarm ve Zamanlayıcı Sinyalleri (SIGALRM)");
    
    printf(COLOR_CYAN "alarm() fonksiyonu, belirtilen saniye sonra SIGALRM gönderir.\n");
    printf(COLOR_CYAN "setitimer() daha hassas zamanlayıcılar için kullanılabilir.\n\n");
    
    struct sigaction sa;
    sa.sa_handler = sigalrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        return;
    }
    
    printf(COLOR_YELLOW "Zamanlayıcı başlatılıyor:\n" COLOR_RESET);
    printf("  - 2 saniye sonra 1. alarm\n");
    printf("  - 4 saniye sonra 2. alarm\n");
    printf("  - 6 saniye sonra 3. alarm (tekrarlayan)\n\n");
    
    // Tek seferlik alarm
    alarm(2);
    sleep(3);
    
    // Tekrar alarm kur
    alarm(4);
    sleep(5);
    
    // Tekrarlayan alarm için setitimer örneği
    printf(COLOR_CYAN "\nsetitimer() ile tekrarlayan alarm:\n" COLOR_RESET);
    
    struct itimerval timer;
    timer.it_value.tv_sec = 1;      // 1 saniye sonra başla
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;   // Her 1 saniyede bir tekrarla
    timer.it_interval.tv_usec = 0;
    
    alarm_triggered = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    
    printf("Her saniye alarm çalacak, 5 saniye devam edecek...\n");
    for (int i = 0; i < 5; i++) {
        sleep(1);
        printf("  %d. saniye geçti\n", i + 1);
    }
    
    // Zamanlayıcıyı durdur
    timer.it_value.tv_sec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
}

/* ============================================================================
 * BÖLÜM 8: Sinyal Güvenliği (Async-Signal-Safe)
 * ============================================================================ */

void demo_signal_safety(void) {
    print_header("8. Sinyal Güvenliği (Async-Signal-Safe)");
    
    printf(COLOR_CYAN "Sinyal işleyicilerinde (handlers) kullanılabilecek fonksiyonlar sınırlıdır.\n");
    printf(COLOR_CYAN "Async-signal-safe fonksiyonlar sinyal işleyicilerinde güvenle çağrılabilir.\n\n");
    
    printf(COLOR_YELLOW "Güvenli (Async-signal-safe) fonksiyonlar:\n" COLOR_RESET);
    printf("  - write(), read(), open(), close()\n");
    printf("  - fork(), exec(), wait(), waitpid()\n");
    printf("  - kill(), raise(), signal(), sigaction()\n");
    printf("  - _exit(), sleep(), usleep()\n\n");
    
    printf(COLOR_RED "Güvensiz (Async-signal-unsafe) fonksiyonlar:\n" COLOR_RESET);
    printf("  - printf(), fprintf(), sprintf()\n");
    printf("  - malloc(), free(), calloc()\n");
    printf("  - fopen(), fclose()\n");
    printf("  - strtok(), rand(), system()\n\n");
    
    printf(COLOR_CYAN "Bu nedenle sinyal işleyicilerinde write() kullanmak daha güvenlidir.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 9: Yakalanamayan Sinyaller (SIGKILL, SIGSTOP)
 * ============================================================================ */

void demo_uncatchable_signals(void) {
    print_header("9. Yakalanamayan Sinyaller (SIGKILL, SIGSTOP)");
    
    printf(COLOR_CYAN "Bazı sinyaller yakalanamaz (catch) ve engellenemez (block):\n\n" COLOR_RESET);
    printf("  - SIGKILL (9)  : Process'i anında sonlandırır. 'kill -9 PID'\n");
    printf("  - SIGSTOP (19) : Process'i anında durdurur. 'kill -STOP PID'\n\n");
    
    printf(COLOR_YELLOW "Bu sinyaller için handler kurmaya çalışırsak:\n" COLOR_RESET);
    
    if (signal(SIGKILL, sigint_handler) == SIG_ERR) {
        printf(COLOR_RED "  ✗ SIGKILL için handler kurulamaz! (SIG_ERR döndü)\n" COLOR_RESET);
    }
    
    if (signal(SIGSTOP, sigint_handler) == SIG_ERR) {
        printf(COLOR_RED "  ✗ SIGSTOP için handler kurulamaz! (SIG_ERR döndü)\n" COLOR_RESET);
    }
    
    printf(COLOR_GREEN "\n✓ SIGKILL ve SIGSTOP her zaman varsayılan davranışlarını gösterir.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 10: Özet ve Karşılaştırma Tablosu
 * ============================================================================ */

void demo_summary(void) {
    print_header("10. Özet: Sinyal Fonksiyonları Karşılaştırması");
    
    printf(COLOR_CYAN "┌─────────────────┬─────────────────────────────────────────────────────┐\n");
    printf(COLOR_CYAN "│ Fonksiyon       │ Açıklama                                            │\n");
    printf(COLOR_CYAN "├─────────────────┼─────────────────────────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ signal()        │ Basit sinyal işleyici kurma (eski, taşınabilir değil)        │\n");
    printf("│ sigaction()     │ Gelişmiş sinyal işleyici kurma (ÖNERİLEN)                    │\n");
    printf("│ kill()          │ Bir sürece sinyal gönderir                                    │\n");
    printf("│ raise()         │ Kendi sürecine sinyal gönderir                                │\n");
    printf("│ alarm()         │ Belirtilen saniye sonra SIGALRM gönderir                     │\n");
    printf("│ pause()         │ Sinyal gelene kadar süreci bekletir                          │\n");
    printf("│ sigprocmask()   │ Sinyal maskesini (bloklama) ayarlar                          │\n");
    printf("│ sigpending()    │ Bekleyen sinyalleri kontrol eder                             │\n");
    printf("│ sigsuspend()    │ Sinyal bekler ve geçici maske uygular                       │\n");
    printf("│ setitimer()     │ Daha hassas zamanlayıcı (mikrosaniye)                       │\n");
    printf(COLOR_CYAN "└─────────────────┴─────────────────────────────────────────────────────┘\n" COLOR_RESET);
}

/* ============================================================================
 * ANA FONKSİYON - MENÜ SİSTEMİ
 * ============================================================================ */

int main(int argc, char *argv[]) {
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    SİNYALLER (SIGNALS) EĞİTİM KODU                 ║\n");
    printf("║            Unix/Linux Sinyal Mekanizması Detaylı Gösterim         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    printf(COLOR_YELLOW "\nBu program PID: %d\n" COLOR_RESET, getpid());
    printf(COLOR_CYAN "Başka bir terminalde 'kill -Sinyal %d' komutlarını deneyebilirsiniz.\n" COLOR_RESET, getpid());
    
    int choice = 0;
    while (choice != 11) {
        printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
        printf("MENÜ:\n");
        printf("  1. signal() ile basit sinyal yakalama (SIGINT, SIGTERM)\n");
        printf("  2. sigaction() ile gelişmiş sinyal yakalama (SIGUSR1, SIGALRM)\n");
        printf("  3. Sinyal gönderme (kill, raise)\n");
        printf("  4. SIGCHLD ile zombi process önleme\n");
        printf("  5. Sinyal bloklama (sigprocmask)\n");
        printf("  6. Sinyal bekletme (sigsuspend)\n");
        printf("  7. Alarm ve zamanlayıcı sinyalleri (SIGALRM)\n");
        printf("  8. Sinyal güvenliği (Async-Signal-Safe)\n");
        printf("  9. Yakalanamayan sinyaller (SIGKILL, SIGSTOP)\n");
        printf(" 10. Özet tablo\n");
        printf(" 11. Çıkış\n");
        printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
        printf("Seçiminiz (1-11): ");
        fflush(stdout);
        
        if (scanf("%d", &choice) != 1) {
            printf(COLOR_RED "Geçersiz giriş!\n" COLOR_RESET);
            while (getchar() != '\n');
            choice = 0;
            continue;
        }
        getchar();
        
        switch (choice) {
            case 1: demo_signal_basic(); break;
            case 2: demo_sigaction_advanced(); break;
            case 3: demo_signal_sending(); break;
            case 4: demo_sigchld_handler(); break;
            case 5: demo_signal_blocking(); break;
            case 6: demo_sigsuspend(); break;
            case 7: demo_alarm_timer(); break;
            case 8: demo_signal_safety(); break;
            case 9: demo_uncatchable_signals(); break;
            case 10: demo_summary(); break;
            case 11: printf(COLOR_GREEN "\nProgram sonlandırılıyor...\n" COLOR_RESET); break;
            default: printf(COLOR_RED "Geçersiz seçim! (1-11 arası)\n" COLOR_RESET);
        }
    }
    
    return 0;
}