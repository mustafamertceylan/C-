/*
 * ============================================================================
 * GITHUB UZAK BAĞLANTI KONTROLÜ VE KOD YÜKLEME PROGRAMI
 * ============================================================================
 * 
 * Bu program, mevcut Git deposunun uzak bağlantısını kontrol eder,
 * değişiklikleri takip eder ve GitHub'a yükler.
 * 
 * Derleme: gcc -o github_upload github_upload.c
 * Çalıştırma: ./github_upload
 * 
 * Not: Bu programı Git deposunun içinde çalıştırın!
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

void print_success(const char *msg) {
    printf(COLOR_GREEN "✓ %s\n" COLOR_RESET, msg);
}

void print_error(const char *msg) {
    printf(COLOR_RED "✗ %s\n" COLOR_RESET, msg);
}

void print_info(const char *msg) {
    printf(COLOR_BLUE "ℹ %s\n" COLOR_RESET, msg);
}

/* ============================================================================
 * GIT KONTROL VE İŞLEM FONKSİYONLARI
 * ============================================================================ */

/**
 * git_is_installed - Sistemde Git kurulu mu kontrol eder
 */
int git_is_installed(void) {
    return system("git --version > /dev/null 2>&1") == 0;
}

/**
 * git_is_repo - Mevcut dizin bir Git deposu mu kontrol eder
 */
int git_is_repo(void) {
    return system("git rev-parse --git-dir > /dev/null 2>&1") == 0;
}

/**
 * git_get_remote_url - Uzak bağlantı URL'ini alır
 */
void git_get_remote_url(void) {
    printf("\n" COLOR_CYAN "Uzak Bağlantı (Remote) Bilgileri:\n" COLOR_RESET);
    system("git remote -v");
}

/**
 * git_check_status - Git durumunu gösterir
 */
void git_check_status(void) {
    printf("\n" COLOR_CYAN "Depo Durumu:\n" COLOR_RESET);
    system("git status");
}

/**
 * git_add_all - Tüm değişiklikleri stage'e ekler
 */
int git_add_all(void) {
    printf("\n" COLOR_CYAN "Değişiklikler stage'e ekleniyor...\n" COLOR_RESET);
    return system("git add .");
}

/**
 * git_commit - Değişiklikleri commit eder
 */
int git_commit(const char *message) {
    char command[512];
    snprintf(command, sizeof(command), "git commit -m \"%s\"", message);
    printf("\n" COLOR_CYAN "Commit yapılıyor: %s\n" COLOR_RESET, message);
    return system(command);
}

/**
 * git_push - Değişiklikleri GitHub'a gönderir
 */
int git_push(void) {
    printf("\n" COLOR_CYAN "GitHub'a gönderiliyor...\n" COLOR_RESET);
    return system("git push origin main 2>&1 || git push origin master 2>&1");
}

/**
 * git_pull - GitHub'dan güncellemeleri çeker
 */
int git_pull(void) {
    printf("\n" COLOR_CYAN "GitHub'dan güncellemeler çekiliyor...\n" COLOR_RESET);
    return system("git pull origin main 2>&1 || git pull origin master 2>&1");
}

/**
 * git_log - Son commit'leri gösterir
 */
void git_log(int count) {
    char command[100];
    snprintf(command, sizeof(command), "git log -%d --oneline", count);
    printf("\n" COLOR_CYAN "Son %d commit:\n" COLOR_RESET, count);
    system(command);
}

/* ============================================================================
 * ANA FONKSİYONLAR
 * ============================================================================ */

/**
 * check_remote_connection - Uzak bağlantıyı kontrol eder
 */
void check_remote_connection(void) {
    print_header("1. UZAK BAĞLANTI KONTROLÜ");
    
    // Git kurulumunu kontrol et
    if (!git_is_installed()) {
        print_error("Git sistemde kurulu değil!");
        print_info("Ubuntu/Debian: sudo apt install git");
        print_info("Fedora: sudo dnf install git");
        exit(1);
    }
    print_success("Git kurulu");
    
    // Git deposu kontrolü
    if (!git_is_repo()) {
        print_error("Bu dizin bir Git deposu değil!");
        print_info("Önce 'git init' ile depo oluşturun");
        exit(1);
    }
    print_success("Geçerli bir Git deposu");
    
    // Uzak bağlantıyı göster
    git_get_remote_url();
}

/**
 * main_menu - Ana menüyü gösterir
 */
void main_menu(void) {
    printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_YELLOW "  YAPMAK İSTEDİĞİNİZ İŞLEMİ SEÇİN\n" COLOR_RESET);
    printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf("  1. Depo durumunu kontrol et (git status)\n");
    printf("  2. Tüm değişiklikleri GitHub'a yükle (add, commit, push)\n");
    printf("  3. Sadece add + commit yap (push yapma)\n");
    printf("  4. GitHub'dan güncelleme çek (git pull)\n");
    printf("  5. Uzak bağlantı bilgilerini göster\n");
    printf("  6. Son commit'leri göster\n");
    printf("  7. Özel commit mesajı ile yükleme yap\n");
    printf("  8. Tüm işlemleri otomatik yap (add + commit + push)\n");
    printf("  9. Çıkış\n");
    printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf("Seçiminiz: ");
}

/**
 * get_commit_message - Commit mesajını alır (tarihli)
 */
void get_commit_message(char *message, size_t size) {
    time_t now;
    struct tm *local;
    char time_str[100];
    
    time(&now);
    local = localtime(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local);
    
    snprintf(message, size, "Otomatik yükleme: Sistem Programlama Kodları (%s)", time_str);
}

/**
 * auto_upload - Otomatik yükleme (add + commit + push)
 */
int auto_upload(void) {
    char commit_msg[256];
    int ret = 0;
    
    print_header("OTOMATİK YÜKLEME");
    
    // 1. Git pull (öncelikle güncellemeleri çek)
    print_info("Önce GitHub'dan güncellemeler çekiliyor...");
    git_pull();
    
    // 2. Add
    if (git_add_all() != 0) {
        print_error("Add işlemi başarısız!");
        return 1;
    }
    print_success("Değişiklikler stage'e eklendi");
    
    // 3. Commit
    get_commit_message(commit_msg, sizeof(commit_msg));
    if (git_commit(commit_msg) != 0) {
        print_info("Commit yapılacak değişiklik yok (veya hata)");
    } else {
        print_success("Commit tamamlandı");
    }
    
    // 4. Push
    if (git_push() != 0) {
        print_error("Push işlemi başarısız! İnternet bağlantınızı kontrol edin.");
        ret = 1;
    } else {
        print_success("GitHub'a gönderildi!");
        git_log(3);
    }
    
    return ret;
}

/* ============================================================================
 * ANA FONKSİYON
 * ============================================================================ */

int main(int argc, char *argv[]) {
    int choice;
    char commit_msg[256];
    
    printf(COLOR_CYAN "\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    GITHUB YÜKLEME ARACI                           ║\n");
    printf("║                   (Git ile Uzak Bağlantı Yönetimi)               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    // Önce uzak bağlantıyı kontrol et
    check_remote_connection();
    
    // Komut satırı argümanı varsa direkt yükleme yap
    if (argc > 1 && strcmp(argv[1], "--auto") == 0) {
        print_info("Otomatik yükleme modu başlatıldı...");
        auto_upload();
        return 0;
    }
    
    // Menü döngüsü
    while (1) {
        main_menu();
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            print_error("Geçersiz giriş!");
            continue;
        }
        getchar(); // newline temizle
        
        switch (choice) {
            case 1:
                git_check_status();
                break;
                
            case 2:
                print_header("DEĞİŞİKLİKLERİ YÜKLE");
                if (git_add_all() == 0) {
                    get_commit_message(commit_msg, sizeof(commit_msg));
                    if (git_commit(commit_msg) == 0) {
                        git_push();
                        git_log(3);
                    }
                }
                break;
                
            case 3:
                print_header("ADD + COMMIT (PUSH YOK)");
                if (git_add_all() == 0) {
                    get_commit_message(commit_msg, sizeof(commit_msg));
                    git_commit(commit_msg);
                }
                break;
                
            case 4:
                print_header("GITHUB'DAN GÜNCELLEME ÇEK");
                git_pull();
                break;
                
            case 5:
                print_header("UZAK BAĞLANTI BİLGİLERİ");
                git_get_remote_url();
                break;
                
            case 6:
                print_header("SON COMMIT'LER");
                git_log(5);
                break;
                
            case 7:
                print_header("ÖZEL COMMIT MESAJI İLE YÜKLEME");
                printf("Commit mesajını girin: ");
                fflush(stdout);
                fgets(commit_msg, sizeof(commit_msg), stdin);
                commit_msg[strcspn(commit_msg, "\n")] = '\0';
                
                if (strlen(commit_msg) == 0) {
                    get_commit_message(commit_msg, sizeof(commit_msg));
                }
                
                if (git_add_all() == 0) {
                    if (git_commit(commit_msg) == 0) {
                        git_push();
                    }
                }
                break;
                
            case 8:
                auto_upload();
                break;
                
            case 9:
                print_success("Program sonlandırılıyor. İyi çalışmalar!");
                return 0;
                
            default:
                print_error("Geçersiz seçim! (1-9 arası)");
                break;
        }
        
        printf("\n");
    }
    
    return 0;
}