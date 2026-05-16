/*
 * ============================================================================
 * jobs KOMUTU - DETAYLI C KODU
 * ============================================================================
 * 
 * Bu program, Unix/Linux kabuklarındaki 'jobs' komutunun çalışma mantığını
 * gösterir. Arka plan işlerini takip eder, durumlarını listeler ve iş kontrolü
 * yapar.
 * 
 * Derleme: gcc -o jobs_demo jobs_demo.c
 * Çalıştır: ./jobs_demo
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

/* ANSI renk kodları */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

/* ============================================================================
 * YAPILAR VE GLOBAL DEĞİŞKENLER
 * ============================================================================ */

#define MAX_JOBS 100
#define MAX_CMD_LEN 256

/**
 * İş (Job) yapısı - Bir işin tüm bilgilerini tutar
 */
typedef struct {
    int job_id;              // İş numarası (1, 2, 3, ...)
    pid_t pgid;              // Process Group ID (işin lideri)
    char command[MAX_CMD_LEN];  // Çalıştırılan komut
    int state;               // İş durumu
    int is_current;          // Varsayılan iş mi? (1: evet, 0: hayır)
    int is_previous;         // Önceki iş mi? (1: evet, 0: hayır)
    time_t start_time;       // Başlangıç zamanı
} Job;

/* İş durumları */
#define JOB_RUNNING    0
#define JOB_STOPPED    1
#define JOB_TERMINATED 2
#define JOB_DONE       3
#define JOB_EXIT       4

/* Global iş listesi */
Job jobs[MAX_JOBS];
int job_count = 0;
int next_job_id = 1;

/* Sinyal işleyicileri için global değişkenler */
volatile sig_atomic_t sigchld_received = 0;
volatile sig_atomic_t sigtstp_received = 0;

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

/**
 * İş durumunu string olarak döndürür
 */
const char* job_state_string(int state) {
    switch (state) {
        case JOB_RUNNING:   return COLOR_GREEN "Running" COLOR_RESET;
        case JOB_STOPPED:   return COLOR_YELLOW "Stopped" COLOR_RESET;
        case JOB_TERMINATED:return COLOR_RED "Terminated" COLOR_RESET;
        case JOB_DONE:      return COLOR_GREEN "Done" COLOR_RESET;
        case JOB_EXIT:      return COLOR_RED "Exit" COLOR_RESET;
        default:            return "Unknown";
    }
}

/**
 * Yeni bir iş ekler
 */
int add_job(pid_t pgid, const char *command) {
    if (job_count >= MAX_JOBS) {
        printf(COLOR_RED "Maksimum iş sayısına ulaşıldı!\n" COLOR_RESET);
        return -1;
    }
    
    jobs[job_count].job_id = next_job_id++;
    jobs[job_count].pgid = pgid;
    strncpy(jobs[job_count].command, command, MAX_CMD_LEN - 1);
    jobs[job_count].command[MAX_CMD_LEN - 1] = '\0';
    jobs[job_count].state = JOB_RUNNING;
    jobs[job_count].is_current = 0;
    jobs[job_count].is_previous = 0;
    jobs[job_count].start_time = time(NULL);
    
    job_count++;
    return job_count - 1;
}

/**
 * İş durumunu günceller
 */
void update_job_status(pid_t pgid, int new_state) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pgid == pgid) {
            jobs[i].state = new_state;
            break;
        }
    }
}

/**
 * İş listesini temizler (sonlanmış işleri kaldırır)
 */
void cleanup_jobs(void) {
    int i = 0;
    while (i < job_count) {
        if (jobs[i].state == JOB_TERMINATED || jobs[i].state == JOB_DONE || jobs[i].state == JOB_EXIT) {
            // İşi listeden kaldır
            for (int j = i; j < job_count - 1; j++) {
                jobs[j] = jobs[j + 1];
            }
            job_count--;
        } else {
            i++;
        }
    }
}

/**
 * Varsayılan işi günceller (en son eklenen çalışan iş)
 */
void update_current_job(void) {
    // Tüm işlerin is_current ve is_previous flag'lerini sıfırla
    for (int i = 0; i < job_count; i++) {
        jobs[i].is_current = 0;
        jobs[i].is_previous = 0;
    }
    
    // En son eklenen çalışan işi bul
    for (int i = job_count - 1; i >= 0; i--) {
        if (jobs[i].state == JOB_RUNNING || jobs[i].state == JOB_STOPPED) {
            jobs[i].is_current = 1;
            
            // Bir önceki işi bul
            for (int j = i - 1; j >= 0; j--) {
                if (jobs[j].state == JOB_RUNNING || jobs[j].state == JOB_STOPPED) {
                    jobs[j].is_previous = 1;
                    break;
                }
            }
            break;
        }
    }
}

/**
 * jobs komutu - Tüm işleri listeler
 */
void jobs_command(int show_pid, int show_only_running, int show_only_stopped) {
    if (job_count == 0) {
        printf(COLOR_YELLOW "Hiç iş yok.\n" COLOR_RESET);
        return;
    }
    
    printf("\n");
    for (int i = 0; i < job_count; i++) {
        // Filtreleme
        if (show_only_running && jobs[i].state != JOB_RUNNING) continue;
        if (show_only_stopped && jobs[i].state != JOB_STOPPED) continue;
        
        // İşaretçi (current/previous)
        char marker = ' ';
        if (jobs[i].is_current) marker = '+';
        else if (jobs[i].is_previous) marker = '-';
        
        // İş numarası ve işaretçi
        printf("[%d]%c ", jobs[i].job_id, marker);
        
        // PID (opsiyonel)
        if (show_pid) {
            printf("%d ", jobs[i].pgid);
        }
        
        // Durum
        printf("%s ", job_state_string(jobs[i].state));
        
        // Geçen süre
        time_t elapsed = time(NULL) - jobs[i].start_time;
        printf("(%ld:%02ld) ", elapsed / 60, elapsed % 60);
        
        // Komut
        printf("%s\n", jobs[i].command);
    }
    printf("\n");
}

/**
 * fg komutu - İşi ön plana alır
 */
void fg_command(int job_id) {
    Job *job = NULL;
    
    // İşi bul
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].job_id == job_id) {
            job = &jobs[i];
            break;
        }
    }
    
    if (job == NULL) {
        printf(COLOR_RED "fg: iş bulunamadı: %d\n" COLOR_RESET, job_id);
        return;
    }
    
    printf(COLOR_GREEN "fg: %s\n" COLOR_RESET, job->command);
    
    // İşi ön plana al (SIGCONT gönder)
    kill(-job->pgid, SIGCONT);
    
    // İşin bitmesini bekle
    int status;
    waitpid(job->pgid, &status, WUNTRACED);
    
    // İş durumunu güncelle
    if (WIFEXITED(status)) {
        job->state = JOB_DONE;
    } else if (WIFSIGNALED(status)) {
        job->state = JOB_TERMINATED;
    } else if (WIFSTOPPED(status)) {
        job->state = JOB_STOPPED;
    }
}

/**
 * bg komutu - Durdurulmuş işi arka planda devam ettirir
 */
void bg_command(int job_id) {
    Job *job = NULL;
    
    // İşi bul
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].job_id == job_id) {
            job = &jobs[i];
            break;
        }
    }
    
    if (job == NULL) {
        printf(COLOR_RED "bg: iş bulunamadı: %d\n" COLOR_RESET, job_id);
        return;
    }
    
    if (job->state != JOB_STOPPED) {
        printf(COLOR_YELLOW "bg: iş %d durdurulmamış\n" COLOR_RESET, job_id);
        return;
    }
    
    printf(COLOR_GREEN "bg: %s &\n" COLOR_RESET, job->command);
    
    // İşi arka planda devam ettir
    kill(-job->pgid, SIGCONT);
    job->state = JOB_RUNNING;
}

/**
 * kill komutu - İşe sinyal gönderir
 */
void kill_job(int job_id, int signal) {
    Job *job = NULL;
    
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].job_id == job_id) {
            job = &jobs[i];
            break;
        }
    }
    
    if (job == NULL) {
        printf(COLOR_RED "kill: iş bulunamadı: %d\n" COLOR_RESET, job_id);
        return;
    }
    
    kill(-job->pgid, signal);
    printf(COLOR_YELLOW "[%d] %s sinyali gönderildi\n" COLOR_RESET, job_id, 
           signal == SIGTERM ? "SIGTERM" : (signal == SIGKILL ? "SIGKILL" : "SIGINT"));
}

/* ============================================================================
 * SİNYAL İŞLEYİCİLERİ
 * ============================================================================ */

void sigchld_handler(int sig) {
    sigchld_received = 1;
    int saved_errno = errno;
    
    // Zombi process'leri temizle ve iş durumunu güncelle
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        if (WIFEXITED(status)) {
            update_job_status(pid, JOB_DONE);
        } else if (WIFSIGNALED(status)) {
            update_job_status(pid, JOB_TERMINATED);
        } else if (WIFSTOPPED(status)) {
            update_job_status(pid, JOB_STOPPED);
        } else if (WIFCONTINUED(status)) {
            update_job_status(pid, JOB_RUNNING);
        }
    }
    
    errno = saved_errno;
}

void sigtstp_handler(int sig) {
    sigtstp_received = 1;
    printf(COLOR_YELLOW "\n[!] SIGTSTP (Ctrl+Z) yakalandı. İş durduruldu.\n" COLOR_RESET);
    fflush(stdout);
}

/* ============================================================================
 * ARKA PLAN İŞİ BAŞLATMA
 * ============================================================================ */

/**
 * Arka plan işi başlatır
 */
int run_background_job(char *cmd) {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    
    if (pid == 0) {
        // Çocuk süreç - yeni process grubu oluştur
        setpgid(0, 0);
        
        // Sinyal işleyicilerini sıfırla (çocuk kendi başına)
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
        
        // Komutu çalıştır
        char *args[] = {cmd, NULL};
        execvp(cmd, args);
        
        // Eğer buraya gelindiyse exec başarısız
        perror("exec");
        exit(1);
    }
    
    // Ebeveyn - çocuğun process grubunu ayarla
    setpgid(pid, pid);
    
    // İşi listeye ekle
    int idx = add_job(pid, cmd);
    if (idx != -1) {
        printf(COLOR_GREEN "[%d] %d\n" COLOR_RESET, jobs[idx].job_id, pid);
    }
    
    return pid;
}

/* ============================================================================
 * TEST KOMUTLARI (Örnek işler)
 * ============================================================================ */

void run_example_jobs(void) {
    print_header("Örnek İşler Başlatılıyor");
    
    printf(COLOR_CYAN "Örnek arka plan işleri başlatılıyor...\n" COLOR_RESET);
    
    // sleep komutları ile örnek işler oluştur
    run_background_job("sleep 30");
    run_background_job("sleep 45");
    run_background_job("sleep 60");
    
    printf(COLOR_GREEN "\n3 örnek iş başlatıldı. 'jobs' komutunu deneyin.\n" COLOR_RESET);
}

/* ============================================================================
 * ANA FONKSİYON - İNTERAKTİF KABUK
 * ============================================================================ */

int main(int argc, char *argv[]) {
    char input[256];
    char cmd[256];
    int job_id;
    
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    jobs KOMUTU DEMO - İŞ KONTROLÜ                 ║\n");
    printf("║                 Arka Plan İşleri ve İş Yönetimi                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    // Sinyal işleyicilerini kur
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);
    
    sa.sa_handler = sigtstp_handler;
    sigaction(SIGTSTP, &sa, NULL);
    
    // Örnek işleri başlat
    run_example_jobs();
    
    print_header("İNTERAKTİF KABUK");
    printf(COLOR_CYAN "Kullanılabilir komutlar:\n" COLOR_RESET);
    printf("  jobs              - Tüm işleri listele\n");
    printf("  jobs -l           - PID ile birlikte listele\n");
    printf("  jobs -r           - Sadece çalışan işleri listele\n");
    printf("  jobs -s           - Sadece durdurulmuş işleri listele\n");
    printf("  fg %%<id>         - İşi ön plana al\n");
    printf("  bg %%<id>         - Durdurulmuş işi arka planda devam ettir\n");
    printf("  kill %%<id>       - İşe SIGTERM gönder\n");
    printf("  kill -9 %%<id>    - İşe SIGKILL gönder\n");
    printf("  run <komut>       - Arka plan işi başlat\n");
    printf("  help              - Bu yardım mesajını göster\n");
    printf("  exit              - Çıkış\n");
    
    print_separator('=', 60);
    
    while (1) {
        // İş durumlarını güncelle
        update_current_job();
        cleanup_jobs();
        
        printf(COLOR_GREEN "\njobs> " COLOR_RESET);
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // Newline karakterini temizle
        input[strcspn(input, "\n")] = '\0';
        
        if (strlen(input) == 0) continue;
        
        // jobs komutları
        if (strcmp(input, "jobs") == 0) {
            jobs_command(0, 0, 0);
        }
        else if (strcmp(input, "jobs -l") == 0) {
            jobs_command(1, 0, 0);
        }
        else if (strcmp(input, "jobs -r") == 0) {
            jobs_command(0, 1, 0);
        }
        else if (strcmp(input, "jobs -s") == 0) {
            jobs_command(0, 0, 1);
        }
        // fg komutu
        else if (sscanf(input, "fg %%%d", &job_id) == 1) {
            fg_command(job_id);
        }
        // bg komutu
        else if (sscanf(input, "bg %%%d", &job_id) == 1) {
            bg_command(job_id);
        }
        // kill komutu
        else if (sscanf(input, "kill %%%d", &job_id) == 1) {
            kill_job(job_id, SIGTERM);
        }
        else if (sscanf(input, "kill -9 %%%d", &job_id) == 1) {
            kill_job(job_id, SIGKILL);
        }
        // run komutu
        else if (strncmp(input, "run ", 4) == 0) {
            char *cmd = input + 4;
            if (strlen(cmd) > 0) {
                run_background_job(cmd);
            } else {
                printf(COLOR_RED "Komut belirtilmedi.\n" COLOR_RESET);
            }
        }
        else if (strcmp(input, "help") == 0) {
            printf(COLOR_CYAN "Kullanılabilir komutlar:\n" COLOR_RESET);
            printf("  jobs              - Tüm işleri listele\n");
            printf("  jobs -l           - PID ile birlikte listele\n");
            printf("  jobs -r           - Sadece çalışan işleri listele\n");
            printf("  jobs -s           - Sadece durdurulmuş işleri listele\n");
            printf("  fg %%<id>         - İşi ön plana al\n");
            printf("  bg %%<id>         - Durdurulmuş işi arka planda devam ettir\n");
            printf("  kill %%<id>       - İşe SIGTERM gönder\n");
            printf("  kill -9 %%<id>    - İşe SIGKILL gönder\n");
            printf("  run <komut>       - Arka plan işi başlat\n");
            printf("  help              - Bu yardım mesajını göster\n");
            printf("  exit              - Çıkış\n");
        }
        else if (strcmp(input, "exit") == 0) {
            printf(COLOR_GREEN "Çıkılıyor...\n" COLOR_RESET);
            break;
        }
        else {
            printf(COLOR_YELLOW "Bilinmeyen komut: %s\n", input);
            printf("'help' yazarak komut listesini görebilirsiniz.\n" COLOR_RESET);
        }
    }
    
    return 0;
}