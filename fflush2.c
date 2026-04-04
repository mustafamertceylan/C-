/*
 * Öğrenci Adı: [Adınız Soyadınız]
 * Öğrenci No: [Öğrenci Numaranız]
 * Ders: Sistem Programlama
 * Ödev: I/O Arabelleğe Alma (Buffering) ve exit() vs _exit() Karşılaştırması
 * 
 * Açıklama: Bu program, stdout'un line buffered (satır arabelleğe almalı) 
 * davranışını ve exit() ile _exit() arasındaki farkı gösterir.
 * 
 * exit()   : I/O buffer'ları temizler (fflush), atexit() fonksiyonlarını çağırır
 * _exit()  : I/O buffer'larını temizlemeden, atexit() çağırmadan anında sonlanır
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// ANSI renk kodları (daha iyi görsel ayrım için)
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

/**
 * print_separator - Ekrana ayırıcı çizgi yazdırır
 */
void print_separator(void) {
    printf("============================================================\n");
}

/**
 * print_header - Program başlığını yazdırır
 */
void print_header(void) {
    print_separator();
    printf(COLOR_CYAN "     I/O ARABELLEĞE ALMA (BUFFERING) VE exit() vs _exit()\n" COLOR_RESET);
    print_separator();
}

/**
 * print_section_header - Bölüm başlığını yazdırır
 */
void print_section_header(const char *title) {
    printf("\n" COLOR_YELLOW "--- %s ---\n" COLOR_RESET, title);
}

/**
 * demonstrate_no_newline_exit - \n olmadan exit() ile çıkış
 * 
 * Bu durumda exit() buffer'ı temizler (fflush), bu nedenle çıktı görünür.
 */
void demonstrate_no_newline_exit(void) {
    print_section_header("Durum 1: '\\n' yok + exit() kullanımı");
    
    printf(COLOR_MAGENTA "  Bu yazı ekrana basılacak MI? (exit() kullanılıyor)" COLOR_RESET);
    printf("\n  (Hemen altında exit(0) çağrılacak...)\n");
    
    printf(COLOR_RED "\n  !! Çıktıda '\\n' olmadığı için normalde buffer'da bekler !!\n" COLOR_RESET);
    printf(COLOR_GREEN "  !! ANCAK exit() buffer'ı otomatik temizler (fflush) !!\n" COLOR_RESET);
    printf(COLOR_GREEN "  !! Bu nedenle çıktı EKRANA BASILACAK !!\n\n" COLOR_RESET);
    
    printf(COLOR_CYAN "  Beklenen çıktı:\n" COLOR_RESET);
    printf(COLOR_CYAN "  > Bu yazı ekrana basılacak MI? (exit() kullanılıyor)\n" COLOR_RESET);
    printf(COLOR_CYAN "  > (Program exit(0) ile sonlanıyor)\n\n" COLOR_RESET);
    
    printf("Press Enter to continue...");
    fflush(stdout);
    getchar();
    
    printf("Bu yazi ekrana basilacak MI? (exit() kullaniliyor)");
    exit(0);
}

/**
 * demonstrate_no_newline_exit_manual - \n olmadan manuel fflush ile çıkış
 */
void demonstrate_no_newline_exit_manual(void) {
    print_section_header("Durum 2: '\\n' yok + manuel fflush() + exit()");
    
    printf(COLOR_MAGENTA "  Bu yazı ekrana basılacak MI? (manuel fflush + exit)" COLOR_RESET);
    printf("\n  (Hemen altında fflush(stdout) ve exit(0) çağrılacak...)\n");
    
    printf(COLOR_GREEN "\n  !! Manuel fflush(stdout) ile buffer hemen temizlenir !!\n" COLOR_RESET);
    printf(COLOR_GREEN "  !! Bu nedenle çıktı EKRANA BASILACAK !!\n\n" COLOR_RESET);
    
    printf("Press Enter to continue...");
    fflush(stdout);
    getchar();
    
    printf("Bu yazi ekrana basilacak MI? (manuel fflush + exit)");
    fflush(stdout);  // Manuel olarak buffer'ı temizle
    exit(0);
}

/**
 * demonstrate_no_newline_exit_with_newline - \n ile exit() kullanımı
 */
void demonstrate_no_newline_exit_with_newline(void) {
    print_section_header("Durum 3: '\\n' var + exit() kullanımı");
    
    printf(COLOR_MAGENTA "  Bu yazı ekrana basılacak MI? (\\n var + exit)\n" COLOR_RESET);
    printf(COLOR_GREEN "  !! '\\n' olduğu için buffer otomatik temizlenir !!\n" COLOR_RESET);
    printf(COLOR_GREEN "  !! Çıktı EKRANA BASILACAK !!\n\n" COLOR_RESET);
    
    printf("Press Enter to continue...");
    fflush(stdout);
    getchar();
    
    printf("Bu yazi ekrana basilacak MI? (\\n var + exit)\n");
    exit(0);
}

/**
 * demonstrate_no_newline_exit_without_flush - \n olmadan _exit() ile çıkış
 * 
 * Bu durumda _exit() buffer'ı temizlemez, bu nedenle çıktı görünmez.
 */
void demonstrate_no_newline_exit_without_flush(void) {
    print_section_header("Durum 4: '\\n' yok + _exit() kullanımı (BUFFER TEMİZLENMEZ)");
    
    printf(COLOR_MAGENTA "  Bu yazı ekrana basılacak MI? (_exit() kullanılıyor)" COLOR_RESET);
    printf("\n  (Hemen altında _exit(0) çağrılacak...)\n");
    
    printf(COLOR_RED "\n  !! '\\n' olmadığı için buffer'da bekler !!\n" COLOR_RESET);
    printf(COLOR_RED "  !! _exit() buffer'ı TEMİZLEMEDEN anında sonlanır !!\n" COLOR_RESET);
    printf(COLOR_RED "  !! Bu nedenle çıktı EKRANA BASILMAZ !!\n\n" COLOR_RESET);
    
    printf("Press Enter to continue...");
    fflush(stdout);
    getchar();
    
    printf("Bu yazi ekrana basilacak MI? (_exit kullaniliyor)");
    _exit(0);
}

/**
 * demonstrate_buffering_with_fork - fork() ile buffer davranışını gösterir
 */
void demonstrate_buffering_with_fork(void) {
    print_section_header("Durum 5: fork() ile Buffer Davranışı (Özel Durum)");
    
    printf(COLOR_MAGENTA "  fork() öncesi buffer'da veri varsa, child'a da kopyalanır!\n" COLOR_RESET);
    printf(COLOR_MAGENTA "  Bu durumda aynı çıktı iki kez görünebilir.\n\n" COLOR_RESET);
    
    printf("Press Enter to continue...");
    fflush(stdout);
    getchar();
    
    printf("fork() oncesi buffer'da bekleyen yazi (\\n yok)");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        printf(" [CHILD] Bu mesaj child'dan\n");
        _exit(0);
    } else {
        printf(" [PARENT] Bu mesaj parent'dan\n");
        // Ebeveyn child'ı bekle
        wait(NULL);
    }
    
    printf(COLOR_YELLOW "\n  !! Eğer çıktıda " COLOR_RESET);
    printf(COLOR_RED "\"fork() oncesi buffer'da bekleyen yazi\" iki kez göründüyse\n" COLOR_RESET);
    printf(COLOR_YELLOW "  !! buffer fork() ile kopyalanmış demektir!\n" COLOR_RESET);
}

/**
 * demonstrate_buffering_types - Farklı buffer tiplerini gösterir
 */
void demonstrate_buffering_types(void) {
    print_section_header("Buffer Tipleri ve Davranışları");
    
    printf("\n");
    printf("┌─────────────────┬────────────────────────────────────────────────┐\n");
    printf("│ Buffer Tipi     │ Davranış                                       │\n");
    printf("├─────────────────┼────────────────────────────────────────────────┤\n");
    printf("│ Unbuffered      │ Her yazma işlemi HEMEN sistem çağrısına dönüşür│\n");
    printf("│ (_IONBF)        │ Örnek: stderr                                 │\n");
    printf("├─────────────────┼────────────────────────────────────────────────┤\n");
    printf("│ Line Buffered   │ '\\n' görülene veya buffer dolana kadar bekler  │\n");
    printf("│ (_IOLBF)        │ Örnek: stdout (terminal)                       │\n");
    printf("├─────────────────┼────────────────────────────────────────────────┤\n");
    printf("│ Fully Buffered  │ Sadece buffer dolduğunda yazılır               │\n");
    printf("│ (_IOFBF)        │ Örnek: Dosya I/O                               │\n");
    printf("└─────────────────┴────────────────────────────────────────────────┘\n");
}

/**
 * demonstrate_exit_vs_exit - exit() ve _exit() karşılaştırması
 */
void demonstrate_exit_vs_exit(void) {
    print_section_header("exit() vs _exit() Karşılaştırması");
    
    printf("\n");
    printf("┌─────────────────┬────────────────────────────┬────────────────────────────┐\n");
    printf("│ Özellik         │ exit()                     │ _exit()                    │\n");
    printf("├─────────────────┼────────────────────────────┼────────────────────────────┤\n");
    printf("│ I/O Buffer      │ TEMİZLER (fflush yapar)    │ TEMİZLEMEZ                 │\n");
    printf("├─────────────────┼────────────────────────────┼────────────────────────────┤\n");
    printf("│ atexit()        │ ÇAĞIRIR                    │ ÇAĞIRMAZ                   │\n");
    printf("├─────────────────┼────────────────────────────┼────────────────────────────┤\n");
    printf("│ tmpfile()       │ TEMİZLER                   │ TEMİZLEMEZ                 │\n");
    printf("├─────────────────┼────────────────────────────┼────────────────────────────┤\n");
    printf("│ Seviye          │ Kütüphane fonksiyonu       │ Sistem çağrısı             │\n");
    printf("├─────────────────┼────────────────────────────┼────────────────────────────┤\n");
    printf("│ Kullanım yeri   │ Normal program sonlanması  │ fork() sonrası child'da    │\n");
    printf("└─────────────────┴────────────────────────────┴────────────────────────────┘\n");
}

/**
 * show_source_code - Ödevde istenen temel kodu gösterir
 */
void show_source_code(void) {
    print_section_header("Ödevde İstenen Temel Kod");
    
    printf("\n");
    printf(COLOR_CYAN "/* Ödevde istenen temel kod parçacığı */\n" COLOR_RESET);
    printf(COLOR_YELLOW "1. printf(\"Bu yazi ekrana basilacak mi?\");\n" COLOR_RESET);
    printf(COLOR_YELLOW "2. _exit(0);\n" COLOR_RESET);
    printf(COLOR_YELLOW "   // Bu durumda çıktı GÖRÜNMEZ!\n\n" COLOR_RESET);
    
    printf(COLOR_CYAN "/* Çözüm 1: exit() kullanarak */\n" COLOR_RESET);
    printf(COLOR_GREEN "1. printf(\"Bu yazi ekrana basilacak mi?\");\n" COLOR_RESET);
    printf(COLOR_GREEN "2. exit(0);\n" COLOR_RESET);
    printf(COLOR_GREEN "   // exit() buffer'ı temizler, çıktı GÖRÜNÜR!\n\n" COLOR_RESET);
    
    printf(COLOR_CYAN "/* Çözüm 2: Manuel fflush() kullanarak */\n" COLOR_RESET);
    printf(COLOR_GREEN "1. printf(\"Bu yazi ekrana basilacak mi?\");\n" COLOR_RESET);
    printf(COLOR_GREEN "2. fflush(stdout);\n" COLOR_RESET);
    printf(COLOR_GREEN "3. _exit(0);\n" COLOR_RESET);
    printf(COLOR_GREEN "   // fflush() buffer'ı temizler, çıktı GÖRÜNÜR!\n\n" COLOR_RESET);
    
    printf(COLOR_CYAN "/* Çözüm 3: '\\n' ekleyerek */\n" COLOR_RESET);
    printf(COLOR_GREEN "1. printf(\"Bu yazi ekrana basilacak mi?\\n\");\n" COLOR_RESET);
    printf(COLOR_GREEN "2. _exit(0);\n" COLOR_RESET);
    printf(COLOR_GREEN "   // '\\n' buffer'ı temizler, çıktı GÖRÜNÜR!\n" COLOR_RESET);
}

int main(void) {
    print_header();
    
    printf("\n");
    printf(COLOR_CYAN "Bu program, I/O arabelleğe alma (buffering) ve çıkış fonksiyonları\n" COLOR_RESET);
    printf(COLOR_CYAN "arasındaki farkı göstermektedir.\n" COLOR_RESET);
    printf(COLOR_YELLOW "\nNOT: Program, farklı durumları göstermek için birden fazla bölümden oluşur.\n" COLOR_RESET);
    printf(COLOR_YELLOW "Her bölümde 'Press Enter to continue...' mesajı görünecek ve sizden\n" COLOR_RESET);
    printf(COLOR_YELLOW "Enter tuşuna basmanız beklenecektir.\n" COLOR_RESET);
    
    // Buffer tiplerini ve exit karşılaştırmasını göster
    demonstrate_buffering_types();
    demonstrate_exit_vs_exit();
    show_source_code();
    
    printf("\n");
    print_separator();
    printf(COLOR_YELLOW "\nAşağıdaki örneklerde, her seferinde farklı bir durum gösterilecektir.\n" COLOR_RESET);
    printf(COLOR_YELLOW "Her örnek sonrası program sonlanacaktır. Tekrar çalıştırmanız gerekecektir.\n" COLOR_RESET);
    printf(COLOR_YELLOW "\nNot: Bu programı her durumu ayrı ayrı görmek için birden fazla kez\n" COLOR_RESET);
    printf(COLOR_YELLOW "çalıştırmanız ve her seferinde farklı bir main fonksiyonu kullanmanız\n" COLOR_RESET);
    printf(COLOR_YELLOW "gerekmektedir. Aşağıdaki seçeneklerden birini seçin:\n\n" COLOR_RESET);
    
    printf("Seçenekler:\n");
    printf("  1 - '\\n' yok + exit() (çıktı GÖRÜNÜR)\n");
    printf("  2 - '\\n' yok + manuel fflush + exit() (çıktı GÖRÜNÜR)\n");
    printf("  3 - '\\n' var + exit() (çıktı GÖRÜNÜR)\n");
    printf("  4 - '\\n' yok + _exit() (çıktı GÖRÜNMEZ) - ÖDEVLİK KISIM\n");
    printf("  5 - fork() ile buffer kopyalama (ekstra)\n");
    printf("  0 - Çıkış\n");
    
    printf("\nSeçiminizi yapın (0-5): ");
    fflush(stdout);
    
    int choice;
    scanf("%d", &choice);
    
    // Buffer'da kalan newline karakterini temizle
    while (getchar() != '\n');
    
    switch (choice) {
        case 1:
            printf("\n" COLOR_GREEN "Seçim 1: '\\n' yok + exit()\n" COLOR_RESET);
            printf(COLOR_GREEN "Bu durumda exit() buffer'ı temizleyeceği için çıktı GÖRÜNÜR!\n\n" COLOR_RESET);
            printf("Press Enter to continue...");
            getchar();
            demonstrate_no_newline_exit();
            break;
            
        case 2:
            printf("\n" COLOR_GREEN "Seçim 2: '\\n' yok + manuel fflush + exit()\n" COLOR_RESET);
            printf(COLOR_GREEN "Bu durumda fflush() buffer'ı temizleyeceği için çıktı GÖRÜNÜR!\n\n" COLOR_RESET);
            printf("Press Enter to continue...");
            getchar();
            demonstrate_no_newline_exit_manual();
            break;
            
        case 3:
            printf("\n" COLOR_GREEN "Seçim 3: '\\n' var + exit()\n" COLOR_RESET);
            printf(COLOR_GREEN "Bu durumda '\\n' buffer'ı temizleyeceği için çıktı GÖRÜNÜR!\n\n" COLOR_RESET);
            printf("Press Enter to continue...");
            getchar();
            demonstrate_no_newline_exit_with_newline();
            break;
            
        case 4:
            printf("\n" COLOR_RED "Seçim 4: '\\n' yok + _exit() (ÖDEVLİK KISIM)\n" COLOR_RESET);
            printf(COLOR_RED "Bu durumda _exit() buffer'ı TEMİZLEMEYECEĞİ için çıktı GÖRÜNMEZ!\n" COLOR_RESET);
            printf(COLOR_RED "Terminalde hiçbir şey görmeyeceksiniz!\n\n" COLOR_RESET);
            printf("Press Enter to continue...");
            getchar();
            demonstrate_no_newline_exit_without_flush();
            break;
            
        case 5:
            printf("\n" COLOR_YELLOW "Seçim 5: fork() ile buffer kopyalama (ekstra)\n" COLOR_RESET);
            printf("Press Enter to continue...");
            getchar();
            demonstrate_buffering_with_fork();
            printf("\nPress Enter to exit...");
            getchar();
            break;
            
        default:
            printf(COLOR_CYAN "\nProgram sonlandırılıyor...\n" COLOR_RESET);
            break;
    }
    
    print_separator();
    printf(COLOR_CYAN "Program başarıyla tamamlandı.\n" COLOR_RESET);
    print_separator();
    
    return 0;
}