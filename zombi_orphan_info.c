/*
 * Program: zombie_orphan_demo.c
 * Konu: Zombie Process ve Orphan Process Kavramları
 * 
 * Bu program, zombie ve orphan process'lerin nasıl oluşturulacağını,
 * nasıl tespit edileceğini ve nasıl temizleneceğini gösterir.
 * 
 * Zombie Process: Sonlanmış ancak ebeveyni wait() çağırmadığı için
 *                process tablosunda temizlenmemiş process.
 * 
 * Orphan Process: Ebeveyni sonlanmış, init (PID 1) tarafından evlat edinilmiş process.
 * 
 * Derleme: gcc -o zombie_orphan_demo zombie_orphan_demo.c
 * Çalıştır: ./zombie_orphan_demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>   // umask() için GEREKLİ!
#include <signal.h>
#include <time.h>

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

void print_separator(char c, int length) {
    for (int i = 0; i < length; i++) putchar(c);
    putchar('\n');
}

void print_header(const char *title) {
    printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_YELLOW "  %s\n" COLOR_RESET, title);
    printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
}

void print_section(const char *title) {
    printf("\n" COLOR_MAGENTA "--- %s ---\n" COLOR_RESET, title);
}

/**
 * show_process_status - Belirli bir PID'nin durumunu gösterir
 */
void show_process_status(pid_t pid, const char *label) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ps -o pid,stat,comm -p %d 2>/dev/null | tail -1", pid);
    printf("%s (PID: %d) durumu: ", label, pid);
    fflush(stdout);
    system(cmd);
}

/**
 * list_zombie_processes - Sistemdeki zombie process'leri listeler
 */
void list_zombie_processes(void) {
    printf(COLOR_YELLOW "\nSistemdeki Zombie Process'ler:\n" COLOR_RESET);
    system("ps aux | awk '$8==\"Z\" {print \"  PID: \"$2\", Komut: \"$11\", Durum: \"$8}' 2>/dev/null || echo \"  Zombie bulunamadı\"");
}

/* ============================================================================
 * BÖLÜM 1: Zombie Process Oluşturma ve Gözlemleme
 * ============================================================================ */

void demo_zombie_creation(void) {
    print_header("1. Zombie Process Oluşturma");
    
    printf(COLOR_CYAN "Zombie process, sonlanmış ancak ebeveyni wait() çağırmadığı için\n");
    printf("process tablosunda temizlenmemiş process'tir.\n\n" COLOR_RESET);
    
    printf(COLOR_GREEN "Terminalde Zombie Process Gözlemleme Komutları:\n" COLOR_RESET);
    printf("  ps aux | awk '$8==\"Z\"'                    # Zombie'leri listele\n");
    printf("  ps -eo pid,stat,comm | grep '^.* Z'       # Alternatif liste\n");
    printf("  top -o NI                                  # top ile gözlem\n\n");
    
    printf(COLOR_YELLOW "C Kodu ile Zombie Process Oluşturma:\n" COLOR_RESET);
    printf("  pid_t pid = fork();\n");
    printf("  if (pid == 0) {\n");
    printf("      exit(0);        // Çocuk hemen sonlanır\n");
    printf("  } else {\n");
    printf("      sleep(30);      // Ebeveyn wait() yapmaz!\n");
    printf("  }\n");
    printf("  // Çocuk 30 saniye boyunca ZOMBIE olarak kalır!\n\n");
    
    printf(COLOR_RED "Bu örnekte bir zombie process oluşturulacak. Devam etmek istiyor musunuz? (e/h): " COLOR_RESET);
    char answer = getchar();
    while (getchar() != '\n'); // buffer temizle
    
    if (answer == 'e' || answer == 'E') {
        printf("\n" COLOR_YELLOW "Zombie process oluşturuluyor...\n" COLOR_RESET);
        
        pid_t pid = fork();
        
        if (pid == 0) {
            // Çocuk process - hemen sonlanır
            printf("  Çocuk process (PID: %d) başladı ve hemen sonlanıyor.\n", getpid());
            exit(0);
        } else {
            // Ebeveyn process - wait() yapmaz!
            printf("  Ebeveyn process (PID: %d)\n", getpid());
            printf("  Çocuk process (PID: %d) oluşturuldu.\n", pid);
            printf("  Ebeveyn wait() çağırmayacak!\n");
            printf("  Çocuk 15 saniye boyunca ZOMBIE olarak kalacak.\n\n");
            
            printf(COLOR_CYAN "Başka bir terminalde şu komutu çalıştırın:\n");
            printf("  ps aux | grep %d\n", pid);
            printf("  (Durum sütununda 'Z' veya 'defunct' göreceksiniz)\n\n" COLOR_RESET);
            
            sleep(15);
            
            printf(COLOR_GREEN "15 saniye sonra ebeveyn sonlanıyor...\n" COLOR_RESET);
            printf("Çocuk process artık temizlenecek (ebeveyn sonlandığı için init devralır).\n");
        }
    } else {
        printf("Zombie oluşturma atlandı.\n");
    }
}

/* ============================================================================
 * BÖLÜM 2: Zombie Process Temizleme (wait() Kullanımı)
 * ============================================================================ */

void demo_zombie_cleanup(void) {
    print_header("2. Zombie Process Temizleme (wait() Kullanımı)");
    
    printf(COLOR_CYAN "Zombie process'leri temizlemenin tek yolu, ebeveynin wait() çağırmasıdır.\n\n" COLOR_RESET);
    
    printf(COLOR_GREEN "Terminalde Zombie Temizleme:\n" COLOR_RESET);
    printf("  Zombie'leri doğrudan terminalden temizleyemezsiniz!\n");
    printf("  Sadece ebeveyn process wait() çağırabilir veya ebeveyn sonlanabilir.\n\n");
    
    printf(COLOR_YELLOW "C Kodu ile Zombie Temizleme (Doğru Kullanım):\n" COLOR_RESET);
    printf("  pid_t pid = fork();\n");
    printf("  if (pid == 0) {\n");
    printf("      exit(0);\n");
    printf("  } else {\n");
    printf("      wait(NULL);     // Çocuk bitene kadar bekle ve temizle!\n");
    printf("  }\n");
    printf("  // Zombie oluşmaz!\n\n");
    
    printf(COLOR_RED "Doğru kullanımı göstermek için bir örnek çalıştırılsın mı? (e/h): " COLOR_RESET);
    char answer = getchar();
    while (getchar() != '\n');
    
    if (answer == 'e' || answer == 'E') {
        printf("\n" COLOR_YELLOW "Doğru kullanım örneği çalıştırılıyor...\n" COLOR_RESET);
        
        pid_t pid = fork();
        
        if (pid == 0) {
            printf("  Çocuk process (PID: %d) başladı ve sonlanıyor.\n", getpid());
            sleep(1);
            exit(42);
        } else {
            int status;
            printf("  Ebeveyn wait() ile çocuğu bekliyor...\n");
            pid_t waited = wait(&status);
            
            if (WIFEXITED(status)) {
                printf("  Çocuk (PID: %d) normal bitti. Çıkış kodu: %d\n", 
                       waited, WEXITSTATUS(status));
            }
            printf("  Zombie oluşmadı!\n");
        }
    } else {
        printf("Örnek atlandı.\n");
    }
}

/* ============================================================================
 * BÖLÜM 3: SIGCHLD Sinyali ile Otomatik Zombie Temizleme
 * ============================================================================ */

// Global değişken - SIGCHLD handler için
volatile sig_atomic_t child_terminated = 0;

void sigchld_handler(int sig) {
    // Çocuk bittiğinde otomatik temizle
    while (waitpid(-1, NULL, WNOHANG) > 0);
    child_terminated = 1;
    printf(COLOR_GREEN "  SIGCHLD yakalandı! Zombie temizlendi.\n" COLOR_RESET);
}

void demo_sigchld_cleanup(void) {
    print_header("3. SIGCHLD Sinyali ile Otomatik Zombie Temizleme");
    
    printf(COLOR_CYAN "SIGCHLD sinyali, bir çocuk process sonlandığında ebeveyne gönderilir.\n");
    printf("Bu sinyali yakalayarak otomatik temizleme yapabiliriz.\n\n" COLOR_RESET);
    
    printf(COLOR_GREEN "Terminalde SIGCHLD Gözlemleme:\n" COLOR_RESET);
    printf("  SIGCHLD sinyalini doğrudan terminalden gözlemleyemezsiniz.\n");
    printf("  strace ile izleyebilirsiniz: strace -e signal ./program\n\n");
    
    printf(COLOR_YELLOW "C Kodu ile SIGCHLD Handler:\n" COLOR_RESET);
    printf("  void sigchld_handler(int sig) {\n");
    printf("      while (waitpid(-1, NULL, WNOHANG) > 0);  // Tüm çocukları temizle\n");
    printf("  }\n");
    printf("  signal(SIGCHLD, sigchld_handler);\n\n");
    
    printf(COLOR_RED "SIGCHLD handler örneği çalıştırılsın mı? (e/h): " COLOR_RESET);
    char answer = getchar();
    while (getchar() != '\n');
    
    if (answer == 'e' || answer == 'E') {
        printf("\n" COLOR_YELLOW "SIGCHLD handler örneği çalıştırılıyor...\n" COLOR_RESET);
        
        // SIGCHLD handler kur
        struct sigaction sa;
        sa.sa_handler = sigchld_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
        sigaction(SIGCHLD, &sa, NULL);
        
        // 3 tane çocuk oluştur
        for (int i = 0; i < 3; i++) {
            pid_t pid = fork();
            
            if (pid == 0) {
                printf("  Çocuk %d (PID: %d) başladı, 2 saniye sonra bitecek.\n", i, getpid());
                sleep(2);
                exit(i);
            }
        }
        
        printf("  Ebeveyn çalışmaya devam ediyor (5 saniye)...\n");
        sleep(5);
        printf("  Tüm çocuklar otomatik temizlendi.\n");
    } else {
        printf("Örnek atlandı.\n");
    }
}

/* ============================================================================
 * BÖLÜM 4: Orphan Process (Yetim Process) Oluşturma
 * ============================================================================ */

void demo_orphan_creation(void) {
    print_header("4. Orphan Process (Yetim Process) Oluşturma");
    
    printf(COLOR_CYAN "Orphan process, ebeveyni kendisinden önce sonlanan process'tir.\n");
    printf("Bu process'ler init process (PID 1) tarafından evlat edinilir.\n\n" COLOR_RESET);
    
    printf(COLOR_GREEN "Terminalde Orphan Process Gözlemleme:\n" COLOR_RESET);
    printf("  ps -eo pid,ppid,stat,comm | grep <PID>\n");
    printf("  pstree -p <PID>                         # Process ağacını göster\n\n");
    
    printf(COLOR_YELLOW "C Kodu ile Orphan Process Oluşturma:\n" COLOR_RESET);
    printf("  pid_t pid = fork();\n");
    printf("  if (pid == 0) {\n");
    printf("      sleep(10);      // Çocuk uzun süre çalışır\n");
    printf("  } else {\n");
    printf("      exit(0);        // Ebeveyn hemen sonlanır!\n");
    printf("  }\n");
    printf("  // Çocuk ORPHAN olur, init (PID 1) tarafından evlat edinilir!\n\n");
    
    printf(COLOR_RED "Orphan process oluşturmak istiyor musunuz? (e/h): " COLOR_RESET);
    char answer = getchar();
    while (getchar() != '\n');
    
    if (answer == 'e' || answer == 'E') {
        printf("\n" COLOR_YELLOW "Orphan process oluşturuluyor...\n" COLOR_RESET);
        
        pid_t pid = fork();
        
        if (pid == 0) {
            // Çocuk process
            printf("  Çocuk process (PID: %d) başladı.\n", getpid());
            printf("  Çocuğun ebeveyni (PPID): %d\n", getppid());
            printf("  Çocuk 10 saniye çalışacak...\n");
            
            sleep(3);
            printf("  Çocuk: 3 saniye geçti, ebeveynim hala %d\n", getppid());
            
            sleep(4);
            printf("  Çocuk: 7 saniye geçti, ebeveynim artık %d (init!)\n", getppid());
            
            sleep(3);
            printf("  Çocuk: Sonlanıyor...\n");
            exit(0);
        } else {
            // Ebeveyn process - hemen sonlanır
            printf("  Ebeveyn process (PID: %d) hemen sonlanıyor!\n", getpid());
            printf("  Çocuk (PID: %d) ORPHAN olacak.\n", pid);
            printf("  init process (PID 1) çocuğu evlat edinecek.\n");
            exit(0);
        }
    } else {
        printf("Orphan oluşturma atlandı.\n");
    }
}

/* ============================================================================
 * BÖLÜM 5: Daemon Process (Orphan Özel Durumu)
 * ============================================================================ */

void demo_daemon_creation(void) {
    print_header("5. Daemon Process (Orphan Process Özel Durumu)");
    
    printf(COLOR_CYAN "Daemon'lar, bilinçli olarak oluşturulan orphan process'lerdir.\n");
    printf("Arka planda çalışan sistem hizmetleri daemon olarak adlandırılır.\n\n" COLOR_RESET);
    
    printf(COLOR_GREEN "Terminalde Daemon'ları Listeleme:\n" COLOR_RESET);
    printf("  ps aux | grep '^root.*d$'              # 'd' ile biten daemon'lar\n");
    printf("  systemctl list-units --type=service    # systemd servisleri\n\n");
    
    printf(COLOR_YELLOW "C Kodu ile Basit Daemon Oluşturma:\n" COLOR_RESET);
    printf("  pid_t pid = fork();\n");
    printf("  if (pid > 0) exit(0);          // Ebeveyn sonlanır\n");
    printf("  setsid();                       // Yeni oturum aç\n");
    printf("  chdir(\"/\");                    // Kök dizine git\n");
    printf("  umask(0);                       // İzin maskesini sıfırla\n");
    printf("  close(0); close(1); close(2);   // STDIN, STDOUT, STDERR kapat\n\n");
    
    printf(COLOR_RED "Basit bir daemon oluşturmak istiyor musunuz? (e/h): " COLOR_RESET);
    char answer = getchar();
    while (getchar() != '\n');
    
    if (answer == 'e' || answer == 'E') {
        printf("\n" COLOR_YELLOW "Basit daemon oluşturuluyor (5 saniye çalışacak)...\n" COLOR_RESET);
        
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("fork");
        } else if (pid > 0) {
            printf("  Ebeveyn sonlanıyor. Daemon arka planda çalışacak.\n");
            exit(0);
        }
        
        // Çocuk: daemon ol
        setsid();                       // Yeni oturum
        chdir("/");                     // Kök dizine git
        umask(0);                       // İzin maskesini sıfırla
        
        // Standart dosya tanımlayıcılarını kapat
        close(0); close(1); close(2);
        
        // Daemon işi: 5 saniye bekle
        FILE *log = fopen("/tmp/daemon_test.log", "w");
        if (log) {
            fprintf(log, "Daemon PID: %d çalışıyor\n", getpid());
            fprintf(log, "Ebeveyn PID (PPID): %d\n", getppid());
            fclose(log);
        }
        
        sleep(5);
        
        log = fopen("/tmp/daemon_test.log", "a");
        if (log) {
            fprintf(log, "Daemon sonlanıyor\n");
            fclose(log);
        }
        
        printf(COLOR_GREEN "  Daemon oluşturuldu. Log: /tmp/daemon_test.log\n" COLOR_RESET);
        exit(0);
    } else {
        printf("Daemon oluşturma atlandı.\n");
    }
}

/* ============================================================================
 * BÖLÜM 6: Zombie ve Orphan Karşılaştırmalı Örnek
 * ============================================================================ */

void demo_comparison(void) {
    print_header("6. Zombie ve Orphan Process Karşılaştırması");
    
    printf(COLOR_CYAN "┌─────────────────┬─────────────────────────────────────────────────────┐\n");
    printf(COLOR_CYAN "│ Özellik         │ Açıklama                                            │\n");
    printf(COLOR_CYAN "├─────────────────┼─────────────────────────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ Zombie Process  │ Sonlanmış, ancak ebeveyn wait() yapmamış                    │\n");
    printf("│                 │ Process tablosunda yer kaplar (PID, çıkış kodu)              │\n");
    printf("│                 │ Bellek ve CPU kullanmaz                                      │\n");
    printf("│                 │ kill ile sonlandırılamaz                                     │\n");
    printf("│                 │ Sadece ebeveyn wait() yaparak veya sonlanarak temizlenir    │\n");
    printf("├─────────────────┼─────────────────────────────────────────────────────────────┤\n");
    printf("│ Orphan Process  │ Ebeveyni sonlanmış, hala çalışan process                     │\n");
    printf("│                 │ init (PID 1) tarafından evlat edinilir                       │\n");
    printf("│                 │ Normal şekilde çalışmaya devam eder                          │\n");
    printf("│                 │ Daemon'lar bilinçli oluşturulmuş orphan process'lerdir      │\n");
    printf(COLOR_CYAN "└─────────────────┴─────────────────────────────────────────────────────┘\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 7: Pratik Alıştırmalar
 * ============================================================================ */

void demo_exercises(void) {
    print_header("7. Pratik Alıştırmalar");
    
    printf(COLOR_CYAN "Terminalde deneyebileceğiniz komutlar:\n\n" COLOR_RESET);
    
    printf(COLOR_GREEN "1. Zombie Process Gözlemleme:\n" COLOR_RESET);
    printf("   # Uzun süreli zombie oluşturan bir program çalıştırın\n");
    printf("   ./zombie_orphan_demo --make-zombie\n");
    printf("   # Başka bir terminalde zombie'leri listeleyin\n");
    printf("   ps aux | awk '$8==\"Z\"'\n\n");
    
    printf(COLOR_GREEN "2. Orphan Process Gözlemleme:\n" COLOR_RESET);
    printf("   # Orphan oluşturan programı çalıştırın\n");
    printf("   ./zombie_orphan_demo --make-orphan\n");
    printf("   # Process ağacını görüntüleyin\n");
    printf("   pstree -p <PID>\n\n");
    
    printf(COLOR_GREEN "3. Process Durumu İzleme:\n" COLOR_RESET);
    printf("   # Tüm process'leri durumlarıyla listele\n");
    printf("   ps -eo pid,stat,comm\n");
    printf("   # Durum kodları:\n");
    printf("     R  = Running (çalışıyor)\n");
    printf("     S  = Sleeping (uyuyor)\n");
    printf("     Z  = Zombie (ölü)\n");
    printf("     D  = Disk sleep (disk bekliyor)\n");
    printf("     T  = Stopped (durdurulmuş)\n\n");
}

/* ============================================================================
 * BÖLÜM 8: Özet Tablo
 * ============================================================================ */

void demo_summary_table(void) {
    print_header("8. Özet: Zombie vs Orphan Process");
    
    printf(COLOR_CYAN "┌─────────────────────────────────────────────────────────────────────────┐\n");
    printf(COLOR_CYAN "│                     ZOMBIE vs ORPHAN PROCESS TABLOSU                    │\n");
    printf(COLOR_CYAN "├─────────────────────────────┬───────────────────┬───────────────────────┤\n");
    printf(COLOR_CYAN "│ Özellik                     │ Zombie Process    │ Orphan Process        │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────┼───────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ Durumu                      │ Sonlanmış         │ Çalışıyor             │\n");
    printf("│ Bellek Kullanımı            │ Yok               │ Var                   │\n");
    printf("│ CPU Kullanımı               │ Yok               │ Var                   │\n");
    printf("│ Process Tablosu             │ Yer kaplar        │ Yer kaplar            │\n");
    printf("│ Ebeveyni                    │ Var (bekliyor)    │ Yok (init evlat edinir)│\n");
    printf("│ Nasıl Oluşur?               │ wait() yapılmazsa │ Ebeveyn önce sonlanırsa│\n");
    printf("│ Nasıl Temizlenir?           │ wait() çağrılmalı │ init otomatik temizler│\n");
    printf("│ kill ile sonlandırılabilir mi?│ Hayır            │ Evet                  │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────┼───────────────────────┤\n");
    printf(COLOR_CYAN "│ C Kodu (Oluşturma)          │ fork();           │ fork();               │\n");
    printf(COLOR_CYAN "│                             │ if(child) exit(0);│ if(parent) exit(0);   │\n");
    printf(COLOR_CYAN "│                             │ if(parent) sleep();│ if(child) sleep();    │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────┼───────────────────────┤\n");
    printf(COLOR_CYAN "│ C Kodu (Temizleme)          │ wait(&status);    │ (otomatik)            │\n");
    printf(COLOR_CYAN "│                             │ signal(SIGCHLD, handler);│                 │\n");
    printf(COLOR_CYAN "└─────────────────────────────┴───────────────────┴───────────────────────┘\n" COLOR_RESET);
}

/* ============================================================================
 * ANA FONKSİYON - MENÜ SİSTEMİ
 * ============================================================================ */

int main(int argc, char *argv[]) {
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║              ZOMBIE PROCESS ve ORPHAN PROCESS DEMO                ║\n");
    printf("║                    (İşlem Durumları ve Yönetimi)                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    // Komut satırı argümanı ile direkt çalıştırma desteği
    if (argc > 1) {
        if (strcmp(argv[1], "--make-zombie") == 0) {
            printf(COLOR_YELLOW "\nZombie process oluşturuluyor...\n" COLOR_RESET);
            pid_t pid = fork();
            if (pid == 0) {
                printf("Çocuk (PID: %d) sonlanıyor.\n", getpid());
                exit(0);
            } else {
                printf("Ebeveyn (PID: %d), çocuk (PID: %d) için wait() yapmıyor.\n", getpid(), pid);
                printf("Çocuk 30 saniye zombie olarak kalacak. Ctrl+C ile sonlandırın.\n");
                sleep(30);
            }
            return 0;
        } else if (strcmp(argv[1], "--make-orphan") == 0) {
            printf(COLOR_YELLOW "\nOrphan process oluşturuluyor...\n" COLOR_RESET);
            pid_t pid = fork();
            if (pid == 0) {
                printf("Çocuk (PID: %d) başladı. Ebeveynim: %d\n", getpid(), getppid());
                sleep(5);
                printf("Çocuk (PID: %d) sonlanıyor. Son ebeveynim: %d\n", getpid(), getppid());
            } else {
                printf("Ebeveyn (PID: %d) sonlanıyor. Çocuk orphan olacak.\n", getpid());
                exit(0);
            }
            return 0;
        }
    }
    
    int choice = 0;
    while (choice != 9) {
        printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
        printf("MENÜ:\n");
        printf("  1. Zombie Process Oluşturma ve Gözlemleme\n");
        printf("  2. Zombie Process Temizleme (wait() Kullanımı)\n");
        printf("  3. SIGCHLD Sinyali ile Otomatik Zombie Temizleme\n");
        printf("  4. Orphan Process (Yetim Process) Oluşturma\n");
        printf("  5. Daemon Process (Orphan Özel Durumu)\n");
        printf("  6. Zombie ve Orphan Karşılaştırması\n");
        printf("  7. Pratik Alıştırmalar\n");
        printf("  8. Özet Tablo\n");
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
        getchar(); // newline temizle
        
        switch (choice) {
            case 1: demo_zombie_creation(); break;
            case 2: demo_zombie_cleanup(); break;
            case 3: demo_sigchld_cleanup(); break;
            case 4: demo_orphan_creation(); break;
            case 5: demo_daemon_creation(); break;
            case 6: demo_comparison(); break;
            case 7: demo_exercises(); break;
            case 8: demo_summary_table(); break;
            case 9: printf(COLOR_GREEN "\nProgram sonlandırılıyor...\n" COLOR_RESET); break;
            default: printf(COLOR_RED "Geçersiz seçim! (1-9 arası girin)\n" COLOR_RESET); break;
        }
    }
    
    // Son olarak sistemdeki zombie'leri listele
    list_zombie_processes();
    
    return 0;
}