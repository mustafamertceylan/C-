/*
 * ============================================================================
 * setpgid() Fonksiyonu - Detaylı Gösterim
 * ============================================================================
 * 
 * Bu program, setpgid() sistem çağrısının nasıl çalıştığını, süreç gruplarının
 * nasıl oluşturulduğunu ve bu mekanizmanın pratikte ne işe yaradığını 
 * göstermektedir.
 *
 * setpgid() Prototipi:
 *    int setpgid(pid_t pid, pid_t pgid);
 *
 * Parametreler:
 *    pid  : Grup kimliği değiştirilecek sürecin PID'i.
 *           0 ise çağıran sürecin kendisi kastedilir.
 *    pgid : Yeni grup kimliği (PGID).
 *           0 ise, yeni PGID, 'pid' ile belirtilen sürecin PID'ine eşitlenir.
 *
 * Dönüş Değeri:
 *    Başarılı ise 0, hata durumunda -1 döner (errno ayarlanır).
 *
 * Yetki:
 *    - Süreç, kendi grubunu değiştirebilir (pid = 0 veya pid = getpid()).
 *    - Başka bir sürecin grubunu değiştirmek için genellikle root yetkisi gerekir.
 *    - Süreç, kendisiyle aynı oturumdaki (session) bir sürecin grubunu,
 *      ancak o süreç henüz bir grup lideri değilse değiştirebilir.
 *
 * Önemli Kurallar:
 *    1. Bir süreç, bir kez grup lideri olduktan sonra grubu değiştirilemez.
 *    2. setpgid() genellikle fork()'tan hemen sonra, exec()'ten ÖNCE çağrılır.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

/* ANSI renk kodları (daha iyi görsel ayrım için) */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

/* Yardımcı fonksiyon: proses bilgilerini yazdır */
void print_process_info(const char *label, pid_t pid) {
    pid_t actual_pid = (pid == 0) ? getpid() : pid;
    pid_t pgid = getpgid(actual_pid);
    pid_t sid = getsid(actual_pid);
    
    printf("%s:\n", label);
    printf("  ├─ PID  : %d\n", actual_pid);
    printf("  ├─ PGID : %d", pgid);
    if (pgid == actual_pid) {
        printf(" (Grup Lideri!)");
    }
    printf("\n");
    printf("  └─ SID  : %d\n", sid);
}

/* Açıklamalı ayırıcı çizgi */
void print_separator(const char *title) {
    printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_YELLOW "  %s\n" COLOR_RESET, title);
    printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════════\n" COLOR_RESET);
}

/* ============================================================================
 * ÖRNEK 1: Kendi süreç grubunu oluşturma (Daemon başlangıcı)
 * ============================================================================ */
void example1_create_own_group(void) {
    print_separator("ÖRNEK 1: Kendi Süreç Grubunu Oluşturma (setpgid(0,0))");
    
    printf(COLOR_CYAN "Bir süreç kendi grubunu oluşturarak grup lideri olabilir.\n" COLOR_RESET);
    printf(COLOR_CYAN "Bu, özellikle daemon programlarının başlangıcında kullanılır.\n\n" COLOR_RESET);
    
    print_process_info("Başlangıç Durumu", 0);
    
    printf("\n" COLOR_YELLOW "setpgid(0, 0) çağrılıyor...\n" COLOR_RESET);
    
    if (setpgid(0, 0) == -1) {
        perror("setpgid");
        return;
    }
    
    print_process_info("setpgid() Sonrası Durum", 0);
    
    printf(COLOR_GREEN "\n✓ Süreç artık kendi grubunun lideridir.\n" COLOR_RESET);
    printf("  Bu sayede terminalden gelen sinyallerden (Ctrl+C) etkilenmez.\n");
}

/* ============================================================================
 * ÖRNEK 2: fork() ile çocuk süreç oluşturup farklı gruba koyma
 * ============================================================================ */
void example2_child_different_group(void) {
    print_separator("ÖRNEK 2: Çocuk Süreci Farklı Bir Gruba Koyma");
    
    printf(COLOR_CYAN "Varsayılan olarak, fork() ile oluşturulan çocuk süreç,\n");
    printf(COLOR_CYAN "ebeveyni ile aynı süreç grubunda bulunur.\n");
    printf(COLOR_CYAN "setpgid() ile bu durumu değiştirebiliriz.\n\n" COLOR_RESET);
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        /* ÇOCUK SÜREÇ */
        printf(COLOR_MAGENTA "\n[ÇOCUK] Başlangıç durumu:\n" COLOR_RESET);
        print_process_info("[ÇOCUK]", 0);
        
        printf("\n[ÇOCUK] setpgid(0, 0) ile kendi grubumu oluşturuyorum...\n");
        if (setpgid(0, 0) == -1) {
            perror("[ÇOCUK] setpgid");
            exit(1);
        }
        
        print_process_info("[ÇOCUK] setpgid() Sonrası", 0);
        
        printf(COLOR_GREEN "\n[ÇOCUK] Artık ebeveynimden FARKLI bir gruptayım!\n" COLOR_RESET);
        sleep(2);  /* Ebeveynin beklemesi için */
        exit(0);
    } 
    else {
        /* EBEVEYN SÜREÇ */
        sleep(1);  /* Çocuğun işlemlerini tamamlaması için bekle */
        
        printf(COLOR_BLUE "\n[EBEVEYN] Durum:\n" COLOR_RESET);
        print_process_info("[EBEVEYN]", 0);
        
        /* Çocuğun yeni PGID'sini al */
        pid_t child_pgid = getpgid(pid);
        printf("\n[EBEVEYN] Çocuğun yeni PGID'si: %d\n", child_pgid);
        
        if (child_pgid != getpgrp()) {
            printf(COLOR_GREEN "\n✓ Çocuk süreç, ebeveynden FARKLI bir süreç grubundadır!\n" COLOR_RESET);
        }
        
        wait(NULL);  /* Zombi oluşmasını engelle */
    }
}

/* ============================================================================
 * ÖRNEK 3: Pipeline simülasyonu - Tüm süreçleri aynı gruba koyma
 * ============================================================================ */
void example3_pipeline_group(void) {
    print_separator("ÖRNEK 3: Pipeline Simülasyonu (Tüm Süreçleri Aynı Gruba Koyma)");
    
    printf(COLOR_CYAN "Bir kabuk (shell), boru hattındaki (pipeline) tüm süreçleri\n");
    printf(COLOR_CYAN "aynı süreç grubuna koyarak tek bir 'iş' (job) olarak yönetir.\n");
    printf(COLOR_CYAN "Bu sayede Ctrl+C tüm süreçleri aynı anda sonlandırabilir.\n\n" COLOR_RESET);
    
    pid_t pid1, pid2;
    pid_t pipeline_pgid;
    
    /* İlk süreci oluştur */
    pid1 = fork();
    
    if (pid1 == 0) {
        /* BURASI 1. ÇOCUK (Örn: ls komutu) */
        
        /* Eğer bu ilk süreçse, yeni bir grup oluştur ve lider ol */
        if (setpgid(0, 0) == -1) {
            perror("[ls] setpgid");
            exit(1);
        }
        
        printf("[ls] PID: %d, PGID: %d (Grup Lideri)\n", getpid(), getpgrp());
        printf("[ls] 'ls -l' komutunu çalıştırıyormuş gibi yapıyorum...\n");
        sleep(2);
        exit(0);
    }
    
    /* İkinci süreci oluştur */
    pid2 = fork();
    
    if (pid2 == 0) {
        /* BURASI 2. ÇOCUK (Örn: grep komutu) */
        
        /* Bu süreci, ilk sürecin grubuna ekle (pipeline_pgid = pid1) */
        if (setpgid(0, pid1) == -1) {
            perror("[grep] setpgid");
            exit(1);
        }
        
        printf("[grep] PID: %d, PGID: %d (Aynı grupta!)\n", getpid(), getpgrp());
        printf("[grep] 'grep' komutunu çalıştırıyormuş gibi yapıyorum...\n");
        sleep(2);
        exit(0);
    }
    
    /* EBEVEYN: Her iki çocuğu da aynı gruba koy */
    printf(COLOR_YELLOW "Ebeveyn, her iki çocuğu da aynı süreç grubuna yerleştiriyor...\n" COLOR_RESET);
    
    /* İlk sürecin grubunu oluştur (zaten oluşturdu) */
    /* İkinci süreci ilk sürecin grubuna ekle */
    if (setpgid(pid2, pid1) == -1) {
        perror("setpgid (parent)");
    }
    
    /* Pipeline grubunun PGID'sini al (ilk sürecin PID'i) */
    pipeline_pgid = getpgid(pid1);
    
    printf("\nPipeline Grubu Bilgileri:\n");
    printf("  ├─ Pipeline PGID: %d\n", pipeline_pgid);
    printf("  ├─ ls sürecinin PID: %d\n", pid1);
    printf("  └─ grep sürecinin PID: %d\n\n", pid2);
    
    printf(COLOR_GREEN "✓ Her iki süreç de AYNI süreç grubundadır!\n" COLOR_RESET);
    printf(COLOR_GREEN "  Tek bir kill(-%d, SIGINT) ile her ikisi de sonlanır.\n" COLOR_RESET, pipeline_pgid);
    
    /* Süreçlerin bitmesini bekle */
    wait(NULL);
    wait(NULL);
}

/* ============================================================================
 * ÖRNEK 4: Hata durumları ve yetki kontrolleri
 * ============================================================================ */
void example4_error_handling(void) {
    print_separator("ÖRNEK 4: Hata Durumları ve Yetki Kontrolleri");
    
    printf(COLOR_CYAN "setpgid() bazı durumlarda başarısız olabilir.\n");
    printf(COLOR_CYAN "Bu durumları ve hata kodlarını inceleyelim.\n\n" COLOR_RESET);
    
    /* Durum 1: Geçersiz PID */
    printf(COLOR_YELLOW "1. Geçersiz PID ile çağrı:\n" COLOR_RESET);
    errno = 0;
    if (setpgid(999999, 0) == -1) {
        printf(COLOR_RED "   Hata: %s (errno: %d)\n" COLOR_RESET, strerror(errno), errno);
    }
    
    /* Durum 2: Süreç zaten bir grup lideri */
    printf(COLOR_YELLOW "\n2. Grup liderinin grubunu değiştirmeye çalışmak:\n" COLOR_RESET);
    pid_t pid = fork();
    
    if (pid == 0) {
        /* Çocuk kendi grubunu oluştur (grup lideri ol) */
        setpgid(0, 0);
        printf("   Çocuk (PID: %d) grup lideri oldu.\n", getpid());
        
        /* Şimdi grup liderinin grubunu değiştirmeye çalış */
        if (setpgid(0, 0) == -1) {
            printf(COLOR_RED "   Hata: %s (Grup lideri zaten!)\n" COLOR_RESET, strerror(errno));
        }
        exit(0);
    }
    wait(NULL);
}

/* ============================================================================
 * ÖRNEK 5: getpgid() ile PGID okuma
 * ============================================================================ */
void example5_getpgid_usage(void) {
    print_separator("ÖRNEK 5: getpgid() ile PGID Okuma");
    
    printf(COLOR_CYAN "getpgid(pid) fonksiyonu, bir sürecin grup kimliğini (PGID) döndürür.\n\n" COLOR_RESET);
    
    printf("Mevcut Süreç Bilgileri:\n");
    print_process_info("Kabuk (Shell)", getppid());
    print_process_info("Bu Program", 0);
    
    /* Kendi süreç grubumuzu bir değişkene alalım */
    pid_t my_pgid = getpgid(0);
    printf("\nNot: getpgid(0) = %d, getpgrp() = %d (aynı!)\n", my_pgid, getpgrp());
}

/* ============================================================================
 * ÖRNEK 6: setpgid() ve exec() birlikte kullanımı
 * ============================================================================ */
void example6_setpgid_before_exec(void) {
    print_separator("ÖRNEK 6: setpgid() ve exec() Birlikte Kullanımı");
    
    printf(COLOR_CYAN "setpgid() genellikle fork()'tan hemen sonra, exec()'ten ÖNCE çağrılır.\n");
    printf(COLOR_CYAN "Çünkü exec() çalıştıktan sonra artık grup değiştirme şansımız kalmaz.\n\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "Simülasyon: 'ls -l' komutunu yeni bir grupta çalıştırma\n" COLOR_RESET);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        /* ÇOCUK SÜREÇ */
        
        /* ÖNCE grubu değiştir */
        if (setpgid(0, 0) == -1) {
            perror("setpgid");
            exit(1);
        }
        printf("Çocuk: Yeni grubum oluşturuldu (PGID: %d)\n", getpgrp());
        
        /* SONRA programı değiştir (exec) */
        printf("Çocuk: Şimdi 'ls -l' komutunu çalıştırıyorum...\n");
        execlp("ls", "ls", "-l", NULL);
        
        /* Eğer buraya gelindiyse exec hata vermiştir */
        perror("execlp");
        exit(1);
    } 
    else {
        sleep(1);
        printf("Ebeveyn: Çocuk süreç (PID: %d) farklı bir grupta çalışıyor.\n", pid);
        wait(NULL);
    }
}

/* ============================================================================
 * ANA FONKSİYON
 * ============================================================================ */
int main(int argc, char *argv[]) {
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    setpgid() SİSTEM ÇAĞRISI                       ║\n");
    printf("║                  Süreç Grubu Yönetimi ve İş Kontrolü              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    printf("\nBu program, setpgid() fonksiyonunun 6 farklı kullanım senaryosunu göstermektedir.\n");
    printf("Her bir örneği sırayla inceleyelim.\n");
    
    int choice = 0;
    while (choice != 7) {
        printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
        printf("MENÜ:\n");
        printf("  1. Kendi süreç grubunu oluşturma (setpgid(0,0))\n");
        printf("  2. Çocuk süreci farklı bir gruba koyma\n");
        printf("  3. Pipeline simülasyonu - Tüm süreçleri aynı gruba koyma\n");
        printf("  4. Hata durumları ve yetki kontrolleri\n");
        printf("  5. getpgid() ile PGID okuma\n");
        printf("  6. setpgid() ve exec() birlikte kullanımı\n");
        printf("  7. Çıkış\n");
        printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
        printf("Seçiminiz (1-7): ");
        fflush(stdout);
        
        if (scanf("%d", &choice) != 1) {
            printf(COLOR_RED "Geçersiz giriş!\n" COLOR_RESET);
            while (getchar() != '\n');
            choice = 0;
            continue;
        }
        getchar();
        
        switch (choice) {
            case 1: example1_create_own_group(); break;
            case 2: example2_child_different_group(); break;
            case 3: example3_pipeline_group(); break;
            case 4: example4_error_handling(); break;
            case 5: example5_getpgid_usage(); break;
            case 6: example6_setpgid_before_exec(); break;
            case 7: printf(COLOR_GREEN "\nProgram sonlandırılıyor...\n" COLOR_RESET); break;
            default: printf(COLOR_RED "Geçersiz seçim! (1-7 arası girin)\n" COLOR_RESET); break;
        }
    }
    
    return 0;
}