/*
 * ============================================================================
 * kill() FONKSİYONU - DETAYLI EĞİTİM KODU
 * ============================================================================
 * 
 * Bu program, kill() fonksiyonunun tüm yönlerini gösterir:
 *   - PID ile sinyal gönderme
 *   - Süreç grubuna sinyal gönderme
 *   - Tüm süreçlere sinyal gönderme
 *   - Sinyal 0 ile süreç varlığını kontrol etme
 *   - Farklı sinyal türleri (SIGINT, SIGTERM, SIGKILL, SIGUSR1, vb.)
 * 
 * Derleme: gcc -o kill_demo kill_demo.c
 * Çalıştır: ./kill_demo
 * 
 * NOT: Bu program bir kabuk simülatörü gibi çalışır.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

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

volatile sig_atomic_t signal_received = 0;
volatile sig_atomic_t last_signal = 0;

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
        case SIGHUP:  printf("SIGHUP (1)"); break;
        case SIGINT:  printf("SIGINT (2)"); break;
        case SIGQUIT: printf("SIGQUIT (3)"); break;
        case SIGKILL: printf("SIGKILL (9)"); break;
        case SIGUSR1: printf("SIGUSR1 (10)"); break;
        case SIGUSR2: printf("SIGUSR2 (12)"); break;
        case SIGTERM: printf("SIGTERM (15)"); break;
        case SIGCHLD: printf("SIGCHLD (17)"); break;
        case SIGCONT: printf("SIGCONT (18)"); break;
        case SIGSTOP: printf("SIGSTOP (19)"); break;
        default:      printf("Sinyal %d", sig); break;
    }
}

/* ============================================================================
 * SİNYAL İŞLEYİCİLERİ (HANDLERS)
 * ============================================================================ */

void generic_handler(int sig) {
    signal_received = 1;
    last_signal = sig;
    printf(COLOR_YELLOW "\n[!] Sinyal alındı: ");
    print_signal_name(sig);
    printf(COLOR_RESET "\n");
    fflush(stdout);
}

void sigint_handler(int sig) {
    signal_received = 1;
    last_signal = sig;
    printf(COLOR_RED "\n[⚠] SIGINT (Ctrl+C) alındı! Program sonlanıyor...\n" COLOR_RESET);
    fflush(stdout);
    exit(0);
}

/* ============================================================================
 * BÖLÜM 1: PID ile Sinyal Gönderme
 * ============================================================================ */

void demo_kill_by_pid(void) {
    print_header("1. PID ile Sinyal Gönderme");
    
    printf(COLOR_CYAN "kill(pid, sig) ile belirli bir PID'ye sinyal gönderebiliriz.\n\n" COLOR_RESET);
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        /* ÇOCUK SÜREÇ - Sinyalleri bekleyen process */
        printf("[ÇOCUK] PID: %d başladı. Sinyal bekliyorum...\n", getpid());
        
        // SIGUSR1 için işleyici kur
        signal(SIGUSR1, generic_handler);
        signal(SIGUSR2, generic_handler);
        signal(SIGTERM, generic_handler);
        
        // Sinyal gelene kadar bekle
        while (1) {
            sleep(1);
            printf("[ÇOCUK] Bekliyorum...\n");
            if (signal_received) {
                printf("[ÇOCUK] Sinyal alındı, çıkıyorum.\n");
                break;
            }
        }
        exit(0);
        
    } else {
        /* EBEVEYN SÜREÇ - Çocuğa sinyal gönderecek */
        sleep(2);
        
        printf("[EBEVEYN] Çocuğa (PID: %d) SIGUSR1 gönderiyorum...\n", pid);
        if (kill(pid, SIGUSR1) == -1) {
            perror("kill");
        }
        sleep(1);
        
        printf("[EBEVEYN] Çocuğa (PID: %d) SIGTERM gönderiyorum...\n", pid);
        if (kill(pid, SIGTERM) == -1) {
            perror("kill");
        }
        
        wait(NULL);
        printf("[EBEVEYN] Çocuk süreç tamamlandı.\n");
    }
}

/* ============================================================================
 * BÖLÜM 2: Süreç Grubuna Sinyal Gönderme (pid == 0)
 * ============================================================================ */

void demo_kill_process_group(void) {
    print_header("2. Süreç Grubuna Sinyal Gönderme (pid = 0)");
    
    printf(COLOR_CYAN "kill(0, sig) ile aynı süreç grubundaki TÜM süreçlere sinyal gönderebiliriz.\n\n" COLOR_RESET);
    printf(COLOR_YELLOW "Mevcut süreç grubu PGID: %d\n" COLOR_RESET, getpgrp());
    
    // 3 tane çocuk süreç oluştur
    pid_t children[3];
    
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        
        if (pid == -1) {
            perror("fork");
            return;
        }
        
        if (pid == 0) {
            /* ÇOCUK SÜREÇLER */
            printf("[Çocuk %d] PID: %d, PGID: %d başladı.\n", i+1, getpid(), getpgrp());
            signal(SIGUSR1, generic_handler);
            
            // Sinyal gelene kadar bekle
            while (!signal_received) {
                sleep(1);
                printf("[Çocuk %d] Bekliyorum...\n", i+1);
            }
            exit(0);
        } else {
            children[i] = pid;
        }
    }
    
    // Ebeveyn - tüm çocukların başlamasını bekle
    sleep(2);
    
    printf(COLOR_YELLOW "\n[EBEVEYN] Aynı süreç grubundaki (PGID: %d) tüm süreçlere sinyal gönderiyorum...\n" COLOR_RESET, getpgrp());
    
    // kill(0, SIGUSR1) - aynı gruptaki TÜM süreçlere sinyal gönder
    if (kill(0, SIGUSR1) == -1) {
        perror("kill");
    }
    
    // Tüm çocukları bekle
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }
    
    printf(COLOR_GREEN "\n✓ Tüm çocuk süreçler sonlandırıldı.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 3: Tüm Süreçlere Sinyal Gönderme (pid == -1)
 * ============================================================================ */

void demo_kill_all_processes(void) {
    print_header("3. Tüm Süreçlere Sinyal Gönderme (pid = -1)");
    
    printf(COLOR_RED "⚠ UYARI: kill(-1, sig) tüm süreçlere sinyal gönderir!\n" COLOR_RESET);
    printf(COLOR_RED "Bu çok tehlikelidir! Sadece root yetkisi olan süreçler hariç tüm süreçlere gönderilir.\n\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "kill(-1, 0) ile sadece süreçlerin var olup olmadığını kontrol edebiliriz (sinyal göndermez).\n" COLOR_RESET);
    
    // Sinyal göndermeden sadece kontrol
    if (kill(-1, 0) == -1) {
        perror("kill");
    } else {
        printf(COLOR_GREEN "✓ kill(-1, 0) başarılı (en az bir süreç var).\n" COLOR_RESET);
    }
    
    printf(COLOR_CYAN "\nNot: Gerçek sinyal gönderme (kill(-1, SIGTERM)) çok tehlikeli olduğu için\n");
    printf("bu örnekte gösterilmemektedir.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 4: Süreç Grubuna Negatif PID ile Sinyal Gönderme (pid < -1)
 * ============================================================================ */

void demo_kill_by_pgid(void) {
    print_header("4. Süreç Grubuna Negatif PID ile Sinyal Gönderme (pid < -1)");
    
    printf(COLOR_CYAN "kill(-pgid, sig) ile belirli bir süreç grubundaki (PGID) tüm süreçlere sinyal gönderebiliriz.\n\n" COLOR_RESET);
    
    // Yeni bir süreç grubu oluştur
    pid_t group_leader = fork();
    
    if (group_leader == -1) {
        perror("fork");
        return;
    }
    
    if (group_leader == 0) {
        /* GRUP LİDERİ */
        setpgid(0, 0);  // Yeni süreç grubu oluştur
        printf("[Grup Lideri] PID: %d, PGID: %d\n", getpid(), getpgrp());
        
        // 2 tane daha çocuk oluştur (aynı grupta olacaklar)
        for (int i = 0; i < 2; i++) {
            pid_t child = fork();
            
            if (child == 0) {
                /* GRUP ÜYESİ */
                printf("[Grup Üyesi %d] PID: %d, PGID: %d\n", i+1, getpid(), getpgrp());
                signal(SIGUSR1, generic_handler);
                while (!signal_received) {
                    sleep(1);
                }
                exit(0);
            }
        }
        
        // Grup lideri sinyal bekle
        signal(SIGUSR1, generic_handler);
        while (!signal_received) {
            sleep(1);
        }
        exit(0);
    }
    
    // Ebeveyn - grup liderinin PGID'sini al
    sleep(1);
    pid_t pgid = group_leader;
    
    printf(COLOR_YELLOW "\n[EBEVEYN] PGID %d'ye (süreç grubuna) SIGUSR1 gönderiyorum...\n" COLOR_RESET, pgid);
    
    // Negatif PID ile süreç grubuna sinyal gönder
    if (kill(-pgid, SIGUSR1) == -1) {
        perror("kill");
    }
    
    // Tüm süreçleri bekle
    wait(NULL);
    wait(NULL);
    wait(NULL);
    
    printf(COLOR_GREEN "\n✓ Tüm grup üyeleri sonlandırıldı.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 5: Sinyal 0 ile Süreç Varlığını Kontrol Etme
 * ============================================================================ */

void demo_kill_check_existence(void) {
    print_header("5. Sinyal 0 ile Süreç Varlığını Kontrol Etme");
    
    printf(COLOR_CYAN "kill(pid, 0) sinyal GÖNDERMEZ, sadece sürecin var olup olmadığını kontrol eder.\n\n" COLOR_RESET);
    
    // Var olan bir PID (kendimiz)
    printf("Kendi PID'imiz (%d) için: ", getpid());
    if (kill(getpid(), 0) == -1) {
        printf(COLOR_RED "❌ Süreç yok veya sinyal gönderme iznimiz yok\n" COLOR_RESET);
    } else {
        printf(COLOR_GREEN "✓ Süreç var\n" COLOR_RESET);
    }
    
    // Var olmayan bir PID
    pid_t fake_pid = 999999;
    printf("Var olmayan PID (%d) için: ", fake_pid);
    if (kill(fake_pid, 0) == -1) {
        printf(COLOR_RED "❌ Süreç yok (errno: %s)\n" COLOR_RESET, strerror(errno));
    } else {
        printf(COLOR_GREEN "✓ Süreç var\n" COLOR_RESET);
    }
    
    // Fork ile oluşturulmuş bir çocuk
    pid_t child = fork();
    
    if (child == 0) {
        // Çocuk süreç - 5 saniye yaşa
        sleep(5);
        exit(0);
    } else {
        printf("Yeni oluşturulan çocuk PID (%d) için: ", child);
        if (kill(child, 0) == -1) {
            printf(COLOR_RED "❌ Süreç yok\n" COLOR_RESET);
        } else {
            printf(COLOR_GREEN "✓ Süreç var (çocuk çalışıyor)\n" COLOR_RESET);
        }
        
        sleep(6);  // Çocuğun bitmesini bekle
        
        printf("Çocuk bittikten sonra (%d) için: ", child);
        if (kill(child, 0) == -1) {
            printf(COLOR_RED "❌ Süreç yok (sonlandı)\n" COLOR_RESET);
        }
        
        wait(NULL);
    }
}

/* ============================================================================
 * BÖLÜM 6: Farklı Sinyal Türleri ile kill()
 * ============================================================================ */

void demo_kill_different_signals(void) {
    print_header("6. Farklı Sinyal Türleri ile kill()");
    
    printf(COLOR_CYAN "kill() ile her türlü sinyali gönderebiliriz:\n\n" COLOR_RESET);
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        /* ÇOCUK SÜREÇ - Farklı sinyallere tepki verecek */
        printf("[ÇOCUK] PID: %d başladı.\n", getpid());
        
        // Farklı sinyaller için işleyiciler kur
        signal(SIGUSR1, generic_handler);
        signal(SIGUSR2, generic_handler);
        signal(SIGTERM, generic_handler);
        signal(SIGINT, generic_handler);
        
        printf("[ÇOCUK] Sinyal bekliyorum...\n");
        while (1) {
            sleep(1);
            if (signal_received) {
                printf("[ÇOCUK] Sinyal alındı, devam ediyorum.\n");
                signal_received = 0;
            }
        }
        exit(0);
        
    } else {
        /* EBEVEYN SÜREÇ - Farklı sinyaller gönderecek */
        sleep(1);
        
        printf("\n[EBEVEYN] SIGUSR1 gönderiliyor...\n");
        kill(pid, SIGUSR1);
        sleep(1);
        
        printf("[EBEVEYN] SIGUSR2 gönderiliyor...\n");
        kill(pid, SIGUSR2);
        sleep(1);
        
        printf("[EBEVEYN] SIGTERM gönderiliyor...\n");
        kill(pid, SIGTERM);
        sleep(1);
        
        printf("[EBEVEYN] SIGINT gönderiliyor...\n");
        kill(pid, SIGINT);
        sleep(1);
        
        // Çocuğu sonlandır
        kill(pid, SIGKILL);
        wait(NULL);
        
        printf(COLOR_GREEN "\n✓ Tüm sinyaller gönderildi.\n" COLOR_RESET);
    }
}

/* ============================================================================
 * BÖLÜM 7: kill() Yetki Kontrolleri
 * ============================================================================ */

void demo_kill_permissions(void) {
    print_header("7. kill() Yetki Kontrolleri");
    
    printf(COLOR_CYAN "kill() ile sinyal göndermek için belirli yetkiler gerekir:\n\n" COLOR_RESET);
    
    printf("1. Süper kullanıcı (root): Herhangi bir sürece sinyal gönderebilir\n");
    printf("2. Normal kullanıcı: Sadece kendi süreçlerine sinyal gönderebilir\n");
    printf("3. SIGKILL ve SIGSTOP göndermek için özel yetkiler gerekebilir\n\n");
    
    // init process (PID 1) - genellikle sadece root sinyal gönderebilir
    printf("init process (PID 1)'e SIGUSR1 göndermeyi deniyorum: ");
    if (kill(1, SIGUSR1) == -1) {
        printf(COLOR_RED "❌ Başarısız: %s\n" COLOR_RESET, strerror(errno));
    } else {
        printf(COLOR_GREEN "✓ Başarılı (root yetkisiyle)\n" COLOR_RESET);
    }
    
    // Kendi sürecimize sinyal gönderme
    printf("Kendi sürecime (PID: %d) SIGUSR1 göndermeyi deniyorum: ", getpid());
    if (kill(getpid(), SIGUSR1) == -1) {
        printf(COLOR_RED "❌ Başarısız\n" COLOR_RESET);
    } else {
        printf(COLOR_GREEN "✓ Başarılı\n" COLOR_RESET);
    }
}

/* ============================================================================
 * BÖLÜM 8: kill() Hata Kodları
 * ============================================================================ */

void demo_kill_errors(void) {
    print_header("8. kill() Hata Kodları");
    
    printf(COLOR_CYAN "kill() başarısız olduğunda errno değerleri:\n\n" COLOR_RESET);
    
    // EINVAL - Geçersiz sinyal
    printf("Geçersiz sinyal (1000) ile deneme: ");
    errno = 0;
    if (kill(getpid(), 1000) == -1) {
        printf(COLOR_RED "EINVAL: %s\n" COLOR_RESET, strerror(errno));
    }
    
    // ESRCH - Süreç yok
    printf("Var olmayan PID (999999) ile deneme: ");
    errno = 0;
    if (kill(999999, SIGUSR1) == -1) {
        printf(COLOR_RED "ESRCH: %s\n" COLOR_RESET, strerror(errno));
    }
    
    // EPERM - Yetki yok (init process'e sinyal göndermeyi dene)
    printf("init process (PID 1)'e SIGUSR1 göndermeyi deneme: ");
    errno = 0;
    if (kill(1, SIGUSR1) == -1) {
        printf(COLOR_RED "EPERM: %s\n" COLOR_RESET, strerror(errno));
    }
}

/* ============================================================================
 * BÖLÜM 9: İnteraktif kill Simülatörü
 * ============================================================================ */

void interactive_kill_simulator(void) {
    print_header("9. İnteraktif kill Simülatörü");
    
    printf(COLOR_CYAN "Bu bölümde, arka planda çalışan bir sürece kill komutları gönderebilirsiniz.\n\n" COLOR_RESET);
    
    // Arka plan süreci oluştur
    pid_t target_pid = fork();
    
    if (target_pid == -1) {
        perror("fork");
        return;
    }
    
    if (target_pid == 0) {
        /* HEDEF SÜREÇ - Sinyalleri bekleyen süreç */
        printf(COLOR_GREEN "[Hedef Süreç] PID: %d\n" COLOR_RESET, getpid());
        printf("[Hedef Süreç] Sinyal bekliyorum. Ctrl+C ile sonlandırabilirsiniz.\n");
        
        signal(SIGUSR1, generic_handler);
        signal(SIGUSR2, generic_handler);
        signal(SIGTERM, generic_handler);
        signal(SIGINT, sigint_handler);
        
        while (1) {
            sleep(1);
            printf("[Hedef Süreç] Çalışıyor...\n");
        }
        exit(0);
        
    } else {
        /* KULLANICI ARAYÜZÜ */
        printf(COLOR_YELLOW "\nHedef süreç PID: %d\n" COLOR_RESET, target_pid);
        printf(COLOR_CYAN "Kullanılabilir komutlar:\n" COLOR_RESET);
        printf("  kill USR1    - SIGUSR1 gönder\n");
        printf("  kill USR2    - SIGUSR2 gönder\n");
        printf("  kill TERM    - SIGTERM gönder (nazik sonlandırma)\n");
        printf("  kill KILL    - SIGKILL gönder (sert sonlandırma)\n");
        printf("  check        - Sürecin var olup olmadığını kontrol et\n");
        printf("  exit         - Bu programdan çık (arka plan süreci devam eder)\n");
        printf("  stop         - Hedef süreci sonlandır ve çık\n\n");
        
        char input[100];
        while (1) {
            printf("kill> ");
            fflush(stdout);
            
            if (fgets(input, sizeof(input), stdin) == NULL) {
                break;
            }
            
            input[strcspn(input, "\n")] = '\0';
            
            if (strcmp(input, "kill USR1") == 0) {
                kill(target_pid, SIGUSR1);
            } else if (strcmp(input, "kill USR2") == 0) {
                kill(target_pid, SIGUSR2);
            } else if (strcmp(input, "kill TERM") == 0) {
                kill(target_pid, SIGTERM);
            } else if (strcmp(input, "kill KILL") == 0) {
                kill(target_pid, SIGKILL);
                wait(NULL);
                printf(COLOR_RED "Hedef süreç sonlandırıldı.\n" COLOR_RESET);
                break;
            } else if (strcmp(input, "check") == 0) {
                if (kill(target_pid, 0) == -1) {
                    printf(COLOR_RED "Hedef süreç artık yok.\n" COLOR_RESET);
                    break;
                } else {
                    printf(COLOR_GREEN "Hedef süreç hala çalışıyor.\n" COLOR_RESET);
                }
            } else if (strcmp(input, "exit") == 0) {
                printf(COLOR_YELLOW "Çıkılıyor. Hedef süreç arka planda devam ediyor.\n" COLOR_RESET);
                break;
            } else if (strcmp(input, "stop") == 0) {
                kill(target_pid, SIGTERM);
                wait(NULL);
                printf(COLOR_GREEN "Hedef süreç sonlandırıldı.\n" COLOR_RESET);
                break;
            } else {
                printf(COLOR_RED "Bilinmeyen komut: %s\n" COLOR_RESET, input);
            }
        }
    }
}

/* ============================================================================
 * ÖZET TABLO
 * ============================================================================ */

void demo_summary(void) {
    print_header("10. Özet: kill() Kullanım Tablosu");
    
    printf(COLOR_CYAN "┌─────────────────────────────────────────────────────────────────────────┐\n");
    printf(COLOR_CYAN "│                        kill() KULLANIM TABLOSU                          │\n");
    printf(COLOR_CYAN "├─────────────────────────────┬───────────────────────────────────────────┤\n");
    printf(COLOR_CYAN "│ Kullanım                    │ Açıklama                                  │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ kill(pid, sig)               │ Belirtilen PID'li sürece sinyal gönder          │\n");
    printf("│ kill(0, sig)                 │ Aynı süreç grubundaki tüm süreçlere gönder      │\n");
    printf("│ kill(-1, sig)                │ Tüm süreçlere sinyal gönder (çok tehlikeli!)    │\n");
    printf("│ kill(-pgid, sig)             │ Belirtilen PGID'li süreç grubuna gönder         │\n");
    printf("│ kill(pid, 0)                 │ Sinyal göndermez, sadece süreç varlığını kontrol│\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────────────────────────────┤\n");
    printf(COLOR_CYAN "│ Dönüş Değeri                │ Açıklama                                  │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ 0                           │ Başarılı                                     │\n");
    printf("│ -1                          │ Hata (errno kontrol edilmeli)                │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────────────────────────────┤\n");
    printf(COLOR_CYAN "│ Hata Kodları                │ Açıklama                                  │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ EINVAL                      │ Geçersiz sinyal numarası                     │\n");
    printf("│ ESRCH                       │ Belirtilen PID'de süreç yok                   │\n");
    printf("│ EPERM                       │ Sinyal gönderme izni yok                      │\n");
    printf(COLOR_CYAN "└─────────────────────────────┴───────────────────────────────────────────┘\n" COLOR_RESET);
}

/* ============================================================================
 * ANA FONKSİYON - MENÜ SİSTEMİ
 * ============================================================================ */

int main(int argc, char *argv[]) {
    int choice = 0;
    
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    kill() FONKSİYONU EĞİTİM KODU                  ║\n");
    printf("║                 Sinyal Gönderme Mekanizması                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    printf(COLOR_YELLOW "\nBu program PID: %d\n" COLOR_RESET, getpid());
    
    while (choice != 11) {
        printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
        printf("MENÜ:\n");
        printf("  1. PID ile sinyal gönderme\n");
        printf("  2. Süreç grubuna sinyal gönderme (pid = 0)\n");
        printf("  3. Tüm süreçlere sinyal gönderme (pid = -1)\n");
        printf("  4. Negatif PID ile süreç grubuna sinyal gönderme (pid < -1)\n");
        printf("  5. Sinyal 0 ile süreç varlığını kontrol etme\n");
        printf("  6. Farklı sinyal türleri ile kill()\n");
        printf("  7. kill() yetki kontrolleri\n");
        printf("  8. kill() hata kodları\n");
        printf("  9. İnteraktif kill simülatörü\n");
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
            case 1:
                signal_received = 0;
                demo_kill_by_pid();
                break;
            case 2:
                signal_received = 0;
                demo_kill_process_group();
                break;
            case 3:
                demo_kill_all_processes();
                break;
            case 4:
                signal_received = 0;
                demo_kill_by_pgid();
                break;
            case 5:
                demo_kill_check_existence();
                break;
            case 6:
                signal_received = 0;
                demo_kill_different_signals();
                break;
            case 7:
                demo_kill_permissions();
                break;
            case 8:
                demo_kill_errors();
                break;
            case 9:
                interactive_kill_simulator();
                break;
            case 10:
                demo_summary();
                break;
            case 11:
                printf(COLOR_GREEN "\nProgram sonlandırılıyor...\n" COLOR_RESET);
                break;
            default:
                printf(COLOR_RED "Geçersiz seçim! (1-11 arası)\n" COLOR_RESET);
        }
    }
    
    return 0;
}