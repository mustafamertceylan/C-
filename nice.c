/*
 * Program: nice_demo_terminal.c
 * Konu: nice Komutunun Terminalde Kullanımı ve C ile Öncelik Yönetimi
 * 
 * Bu program, nice komutunun terminalde nasıl kullanılacağını
 * gösterir ve aynı işlevleri C kodu içinde nasıl yapacağınızı öğretir.
 * 
 * Derleme: gcc -o nice_demo nice_demo_terminal.c
 * Çalıştır: ./nice_demo
 */

 #include<time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
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

void print_separator(char c, int length) {
    for (int i = 0; i < length; i++) putchar(c);
    putchar('\n');
}

void print_header(const char *title) {
    printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_YELLOW "  %s\n" COLOR_RESET, title);
    printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 1: Terminalde nice Komutu Kullanımı (system() ile)
 * ============================================================================ */

void demo_terminal_nice_commands(void) {
    print_header("1. Terminalde nice Komutu Kullanımı (system() ile)");
    
    printf(COLOR_CYAN "Aşağıdaki komutlar terminalde doğrudan çalıştırılabilir:\n\n" COLOR_RESET);
    
    // 1.1. Mevcut nice değerini görüntüleme
    printf(COLOR_GREEN "1.1. Mevcut shell'in nice değerini görüntüleme:\n" COLOR_RESET);
    printf("    Komut: nice -n 0 bash -c 'echo $BASHPID; ps -o pid,nice,comm -p $$'\n");
    system("nice -n 0 bash -c 'echo \"    Mevcut shell PID: $$\"; ps -o pid,nice,comm -p $$ 2>/dev/null || echo \"    ps komutu çalışmıyor\"'");
    
    // 1.2. Varsayılan nice değeri ile program çalıştırma
    printf(COLOR_GREEN "\n1.2. Varsayılan nice değeri (0) ile program çalıştırma:\n" COLOR_RESET);
    printf("    Komut: ./sample_program\n");
    printf("    (Bu örnekte sleep komutu kullanılıyor)\n");
    system("nice -n 0 sleep 1 &");
    
    // 1.3. Düşük öncelikle (yüksek nice) program çalıştırma
    printf(COLOR_GREEN "\n1.3. Düşük öncelikle (nazik) program çalıştırma:\n" COLOR_RESET);
    printf("    Komut: nice -n 19 ./sample_program\n");
    system("nice -n 19 sleep 1 &");
    
    // 1.4. Yüksek öncelikle (düşük nice) program çalıştırma (root gerekir)
    printf(COLOR_GREEN "\n1.4. Yüksek öncelikle (kaba) program çalıştırma (root gerekir):\n" COLOR_RESET);
    printf("    Komut: sudo nice -n -20 ./sample_program\n");
    system("sudo nice -n -20 sleep 1 & 2>/dev/null || echo \"    (sudo gerektiği için atlanıyor)\"");
}

/* ============================================================================
 * BÖLÜM 2: Farklı nice Değerleri ile Program Çalıştırma
 * ============================================================================ */

void demo_run_with_nice_values(void) {
    print_header("2. Farklı nice Değerleri ile Program Çalıştırma");
    
    printf(COLOR_CYAN "nice komutunun temel sözdizimi:\n" COLOR_RESET);
    printf("  nice -n <nice_değeri> <program> <argümanlar>\n\n");
    
    printf(COLOR_YELLOW "Örnekler:\n" COLOR_RESET);
    
    // Örnek 1: Normal öncelik
    printf("\n  %s▶ nice -n 0 ./program%s\n", COLOR_GREEN, COLOR_RESET);
    system("echo '    Normal öncelikle çalıştırıldı'");
    
    // Örnek 2: Düşük öncelik (nazik)
    printf("\n  %s▶ nice -n 10 ./program%s\n", COLOR_GREEN, COLOR_RESET);
    system("echo '    Düşük öncelikle çalıştırıldı (nazik)'");
    
    // Örnek 3: En düşük öncelik (en nazik)
    printf("\n  %s▶ nice -n 19 ./program%s\n", COLOR_GREEN, COLOR_RESET);
    system("echo '    En düşük öncelikle çalıştırıldı (en nazik)'");
    
    // Örnek 4: Yüksek öncelik (kaba) - root gerekir
    printf("\n  %s▶ sudo nice -n -10 ./program%s\n", COLOR_RED, COLOR_RESET);
    system("echo '    Yüksek öncelikle çalıştırıldı (kaba) - root gerekir'");
    
    // Örnek 5: En yüksek öncelik (en kaba) - root gerekir
    printf("\n  %s▶ sudo nice -n -20 ./program%s\n", COLOR_RED, COLOR_RESET);
    system("echo '    En yüksek öncelikle çalıştırıldı (en kaba) - root gerekir'");
}

/* ============================================================================
 * BÖLÜM 3: Çalışan Process'in Nice Değerini Değiştirme (renice)
 * ============================================================================ */

void demo_renice_command(void) {
    print_header("3. Çalışan Process'in Nice Değerini Değiştirme (renice)");
    
    printf(COLOR_CYAN "renice komutu, zaten çalışan bir process'in nice değerini değiştirir.\n\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "Sözdizimi:\n" COLOR_RESET);
    printf("  renice -n <yeni_nice_değeri> -p <PID>\n\n");
    
    printf(COLOR_YELLOW "Örnekler:\n" COLOR_RESET);
    
    // Uzun süre çalışan bir process oluştur
    printf("\n  Uzun süre çalışan bir process oluşturuluyor...\n");
    pid_t pid = fork();
    
    if (pid == 0) {
        // Çocuk process: 30 saniye çalış
        printf("    Çocuk process (PID: %d) başladı. 30 saniye çalışacak.\n", getpid());
        printf("    Başlangıç nice değeri: %d\n", getpriority(PRIO_PROCESS, 0));
        
        for (int i = 0; i < 30; i++) {
            sleep(1);
            if (i == 2) {
                printf("    Çocuk: 3 saniye geçti, nice değeri hala %d\n", 
                       getpriority(PRIO_PROCESS, 0));
            }
        }
        exit(0);
    } else {
        // Ebeveyn: çocuğun nice değerini değiştir
        sleep(1);
        
        printf("\n  %s▶ renice -n 15 -p %d%s\n", COLOR_GREEN, pid, COLOR_RESET);
        char cmd[100];
        snprintf(cmd, sizeof(cmd), "renice -n 15 -p %d 2>/dev/null", pid);
        system(cmd);
        printf("    Çocuğun yeni nice değeri: %d\n", getpriority(PRIO_PROCESS, pid));
        
        sleep(2);
        
        printf("\n  %s▶ sudo renice -n -5 -p %d%s (root gerekir)\n", COLOR_RED, pid, COLOR_RESET);
        snprintf(cmd, sizeof(cmd), "sudo renice -n -5 -p %d 2>/dev/null", pid);
        system(cmd);
        
        // Çocuğu bekle
        wait(NULL);
    }
}

/* ============================================================================
 * BÖLÜM 4: C Kodu İçinde nice() Sistem Çağrısı Kullanımı
 * ============================================================================ */

void demo_c_nice_call(void) {
    print_header("4. C Kodu İçinde nice() Sistem Çağrısı Kullanımı");
    
    printf(COLOR_CYAN "C programı içinde kendi önceliğinizi değiştirebilirsiniz.\n\n" COLOR_RESET);
    
    // Mevcut nice değerini al
    errno = 0;
    int current = getpriority(PRIO_PROCESS, 0);
    if (current == -1 && errno != 0) {
        perror("getpriority");
    } else {
        printf("Başlangıç nice değeri: %d\n", current);
    }
    
    // nice() ile değeri artır (daha nazik yap)
    printf("\nnice(10) çağrılıyor...\n");
    int new_val = nice(10);
    if (new_val == -1 && errno != 0) {
        perror("nice");
    } else {
        printf("Yeni nice değeri: %d (10 artırıldı)\n", new_val);
    }
    
    // setpriority() ile doğrudan değer ayarla
    printf("\nsetpriority(PRIO_PROCESS, 0, 5) çağrılıyor...\n");
    if (setpriority(PRIO_PROCESS, 0, 5) == -1) {
        perror("setpriority");
        printf(COLOR_RED "Not: Normal kullanıcı nice değerini DÜŞÜREMEZ (5 < mevcut değer)\n" COLOR_RESET);
    } else {
        printf("Yeni nice değeri: %d\n", getpriority(PRIO_PROCESS, 0));
    }
    
    // Eski değere geri dön (isteğe bağlı)
    setpriority(PRIO_PROCESS, 0, current);
    printf("\nEski nice değerine dönüldü: %d\n", getpriority(PRIO_PROCESS, 0));
}

/* ============================================================================
 * BÖLÜM 5: fork() ile Çocuk Process'te nice Değeri Değiştirme
 * ============================================================================ */

void demo_fork_with_nice(void) {
    print_header("5. fork() ile Çocuk Process'te nice Değeri Değiştirme");
    
    printf(COLOR_CYAN "Terminalde yapılan 'nice -n 10 ./program' işleminin C karşılığı:\n\n" COLOR_RESET);
    
    printf(COLOR_GREEN "// Terminal komutu:\n");
    printf("nice -n 10 ./program\n\n");
    
    printf("// C kodu karşılığı:\n");
    printf("pid_t pid = fork();\n");
    printf("if (pid == 0) {\n");
    printf("    nice(10);        // nice değerini 10 artır\n");
    printf("    execlp(\"./program\", \"program\", NULL);\n");
    printf("}\n" COLOR_RESET);
    
    // Gerçek örnek
    printf("\n" COLOR_YELLOW "Gerçek örnek çalıştırılıyor...\n" COLOR_RESET);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Çocuk process: nice değerini değiştir
        printf("Çocuk process (PID: %d) başladı\n", getpid());
        printf("Başlangıç nice değeri: %d\n", getpriority(PRIO_PROCESS, 0));
        
        nice(15);
        printf("nice(15) sonrası nice değeri: %d\n", getpriority(PRIO_PROCESS, 0));
        
        // Biraz iş yap
        printf("Çocuk: 2 saniye çalışıyor...\n");
        sleep(2);
        printf("Çocuk tamamlandı.\n");
        exit(0);
    } else {
        wait(NULL);
        printf("\nEbeveyn: Çocuk process tamamlandı.\n");
    }
}

/* ============================================================================
 * BÖLÜM 6: Process Önceliğini Görüntüleme Komutları
 * ============================================================================ */

void demo_view_priority_commands(void) {
    print_header("6. Process Önceliğini Görüntüleme Komutları");
    
    printf(COLOR_CYAN "Terminalde process'lerin nice değerlerini görüntüleme yöntemleri:\n\n" COLOR_RESET);
    
    printf(COLOR_GREEN "1. ps komutu ile:\n" COLOR_RESET);
    printf("   ps -eo pid,nice,comm --sort=nice\n");
    system("ps -eo pid,nice,comm --sort=nice 2>/dev/null | head -10 || echo \"   ps komutu çalışmıyor\"");
    
    printf(COLOR_GREEN "\n2. top komutu ile (interaktif):\n" COLOR_RESET);
    printf("   top -o NI\n");
    printf("   (top içinde 'NI' sütunu nice değerini gösterir)\n");
    
    printf(COLOR_GREEN "\n3. Belirli bir process'in nice değerini görüntüleme:\n" COLOR_RESET);
    printf("   ps -o pid,nice,comm -p <PID>\n");
    printf("   Örnek: ps -o pid,nice,comm -p %d\n", getpid());
    char cmd[100];
    snprintf(cmd, sizeof(cmd), "ps -o pid,nice,comm -p %d 2>/dev/null", getpid());
    system(cmd);
}

/* ============================================================================
 * BÖLÜM 7: Pratik Örnek - Yoğun Hesaplama ile nice Etkisi
 * ============================================================================ */

void demo_practical_example(void) {
    print_header("7. Pratik Örnek - Yoğun Hesaplama ile nice Etkisi");
    
    printf(COLOR_CYAN "Bu örnekte, farklı nice değerlerine sahip process'lerin\n");
    printf("CPU zamanı dağılımını gözlemleyeceğiz.\n\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "Terminalde şu komutları çalıştırarak test edebilirsiniz:\n\n" COLOR_RESET);
    
    printf("  # Terminal 1 (düşük öncelik - nazik)\n");
    printf("  nice -n 19 bash -c 'while true; do i=0; done'\n\n");
    
    printf("  # Terminal 2 (normal öncelik)\n");
    printf("  bash -c 'while true; do i=0; done'\n\n");
    
    printf("  # Terminal 3 (yüksek öncelik - kaba, root gerekir)\n");
    printf("  sudo nice -n -20 bash -c 'while true; do i=0; done'\n\n");
    
    printf(COLOR_GREEN "top komutu ile hangi process'in daha fazla CPU aldığını gözlemleyin.\n" COLOR_RESET);
    
    // Basit bir demo: kısa süreli test
    printf("\n" COLOR_YELLOW "Kısa süreli test yapılıyor (5 saniye)...\n" COLOR_RESET);
    
    pid_t pids[3];
    int nice_values[] = {19, 0, -20};
    const char *labels[] = {"Düşük Öncelik (19)", "Normal Öncelik (0)", "Yüksek Öncelik (-20)"};
    
    for (int i = 0; i < 3; i++) {
        pids[i] = fork();
        
        if (pids[i] == 0) {
            // Çocuk process
            if (nice_values[i] < 0) {
                // Yüksek öncelik için root gerekir, deneyelim
                setpriority(PRIO_PROCESS, 0, nice_values[i]);
            } else {
                nice(nice_values[i]);
            }
            
            // Yoğun hesaplama
            volatile unsigned long count = 0;
            time_t start = time(NULL);
            while (time(NULL) - start < 5) {
                count++;
            }
            exit(0);
        }
    }
    
    // Ebeveyn: tüm çocukları bekle
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }
    
    printf(COLOR_GREEN "\nTest tamamlandı.\n" COLOR_RESET);
    printf(COLOR_YELLOW "Not: Yüksek öncelikli (-20) process root yetkisi gerektirdiği için\n");
    printf("normal kullanıcıda etkili olmayabilir.\n" COLOR_RESET);
}

/* ============================================================================
 * BÖLÜM 8: Özet Tablo
 * ============================================================================ */

void demo_summary_table(void) {
    print_header("8. Özet: nice Komutu Kullanım Tablosu");
    
    printf(COLOR_CYAN "┌─────────────────────────────────────────────────────────────────────────┐\n");
    printf(COLOR_CYAN "│                     nice KOMUTU KULLANIM TABLOSU                        │\n");
    printf(COLOR_CYAN "├─────────────────────────────┬───────────────────────────────────────────┤\n");
    printf(COLOR_CYAN "│ Amaç                        │ Komut                                     │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ Varsayılan nice ile çalıştır │ nice ./program                               │\n");
    printf("│ Belirli nice ile çalıştır    │ nice -n 10 ./program                         │\n");
    printf("│ En düşük öncelikle çalıştır  │ nice -n 19 ./program                         │\n");
    printf("│ En yüksek öncelikle çalıştır │ sudo nice -n -20 ./program                   │\n");
    printf("│ Çalışan process'i değiştir   │ renice -n 15 -p <PID>                        │\n");
    printf("│ Nice değerini görüntüle      │ ps -o pid,nice,comm -p <PID>                 │\n");
    printf("│ Tüm process'leri listele     │ ps -eo pid,nice,comm --sort=nice             │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────────────────────────────┤\n");
    printf(COLOR_CYAN "│ C Kodu Karşılığı            │ Fonksiyon                                 │\n");
    printf(COLOR_CYAN "├─────────────────────────────┼───────────────────────────────────────────┤\n");
    printf(COLOR_RESET);
    printf("│ Nice değerini artır         │ nice(10);                                    │\n");
    printf("│ Nice değerini oku           │ getpriority(PRIO_PROCESS, 0);                │\n");
    printf("│ Nice değerini ayarla        │ setpriority(PRIO_PROCESS, 0, 10);            │\n");
    printf(COLOR_CYAN "└─────────────────────────────┴───────────────────────────────────────────┘\n" COLOR_RESET);
}

/* ============================================================================
 * ANA FONKSİYON
 * ============================================================================ */

int main(int argc, char *argv[]) {
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║              nice KOMUTU - TERMİNAL KULLANIM REHBERİ              ║\n");
    printf("║                 (İşlem Önceliği Yönetimi)                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    printf(COLOR_YELLOW "\nBu program, nice komutunun terminalde nasıl kullanılacağını\n");
    printf("gösterir ve aynı işlemleri C kodu içinde nasıl yapacağınızı öğretir.\n");
    printf("\nNOT: Bazı komutlar root yetkisi gerektirir.\n" COLOR_RESET);
    
    int choice = 0;
    while (choice != 9) {
        printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
        printf("MENÜ:\n");
        printf("  1. Terminalde nice komutu kullanımı (system ile)\n");
        printf("  2. Farklı nice değerleri ile program çalıştırma\n");
        printf("  3. renice ile çalışan process önceliğini değiştirme\n");
        printf("  4. C kodu içinde nice() sistem çağrısı\n");
        printf("  5. fork() ile çocuk process'te nice değeri değiştirme\n");
        printf("  6. Process önceliğini görüntüleme komutları\n");
        printf("  7. Pratik örnek - nice etkisini gözlemleme\n");
        printf("  8. Özet tablo\n");
        printf("  9. Çıkış\n");
        printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
        printf("Seçiminiz (1-9): ");
        fflush(stdout);
        
        scanf("%d", &choice);
        getchar(); // newline temizle
        
        switch (choice) {
            case 1: demo_terminal_nice_commands(); break;
            case 2: demo_run_with_nice_values(); break;
            case 3: demo_renice_command(); break;
            case 4: demo_c_nice_call(); break;
            case 5: demo_fork_with_nice(); break;
            case 6: demo_view_priority_commands(); break;
            case 7: demo_practical_example(); break;
            case 8: demo_summary_table(); break;
            case 9: printf(COLOR_GREEN "\nProgram sonlandırılıyor...\n" COLOR_RESET); break;
            default: printf(COLOR_RED "Geçersiz seçim!\n" COLOR_RESET); break;
        }
    }
    
    return 0;
}