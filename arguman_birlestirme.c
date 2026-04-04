/*
 * Öğrenci Adı: [Adınız Soyadınız]
 * Öğrenci No: [Öğrenci Numaranız]
 * Ders: Sistem Programlama
 * Ödev: Komut Satırı Argümanlarını Birleştirme
 * 
 * Açıklama: Bu program, komut satırında verilen tüm argümanları
 * birleştirerek tek bir metin haline getirir ve ekrana yazdırır.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * arguments_join - Tüm komut satırı argümanlarını birleştirir
 * 
 * @param argc: Argüman sayısı
 * @param argv: Argüman dizisi
 * @param separator: Argümanlar arasına konulacak ayırıcı string (NULL olabilir)
 * 
 * @return: Birleştirilmiş string (malloc ile ayrılır, çağıran free() yapmalıdır)
 *          Hata durumunda NULL döner
 */
char *arguments_join(int argc, char *argv[], const char *separator) {
    // Hata kontrolü
    if (argc <= 1 || argv == NULL) {
        return NULL;
    }
    
    // Varsayılan ayırıcı: boşluk
    if (separator == NULL) {
        separator = " ";
    }
    
    // Toplam uzunluğu hesapla
    size_t total_length = 0;
    size_t sep_len = strlen(separator);
    
    for (int i = 1; i < argc; i++) {
        if (argv[i] != NULL) {
            total_length += strlen(argv[i]);
            // Argümanlar arasına ayırıcı ekle (son argümandan sonra ekleme)
            if (i < argc - 1) {
                total_length += sep_len;
            }
        }
    }
    
    // Null terminator için +1
    total_length++;
    
    // Bellek ayır
    char *result = (char *)malloc(total_length);
    if (result == NULL) {
        fprintf(stderr, "Hata: Bellek ayrılamadı!\n");
        return NULL;
    }
    
    // String'leri birleştir
    result[0] = '\0';  // İlk karakteri null yap
    
    for (int i = 1; i < argc; i++) {
        if (argv[i] != NULL) {
            // Mevcut string'e argümanı ekle
            strcat(result, argv[i]);
            
            // Son argüman değilse ayırıcı ekle
            if (i < argc - 1) {
                strcat(result, separator);
            }
        }
    }
    
    return result;
}

/**
 * arguments_join_simple - Basit versiyon (boşluk ile birleştirir)
 * 
 * @param argc: Argüman sayısı
 * @param argv: Argüman dizisi
 * 
 * @return: Birleştirilmiş string (malloc ile ayrılır, çağıran free() yapmalıdır)
 */
char *arguments_join_simple(int argc, char *argv[]) {
    return arguments_join(argc, argv, " ");
}

/**
 * arguments_join_without_alloc - Bellek tahsisi olmadan doğrudan yazdırır
 * (Alternatif: Bellek ayırmadan çalışan versiyon)
 * 
 * @param argc: Argüman sayısı
 * @param argv: Argüman dizisi
 * @param separator: Argümanlar arasına konulacak ayırıcı
 * @param output: Çıktının yazılacağı FILE* (stdout, stderr, veya dosya)
 */
void arguments_join_print(int argc, char *argv[], const char *separator, FILE *output) {
    if (argc <= 1 || argv == NULL || output == NULL) {
        return;
    }
    
    if (separator == NULL) {
        separator = " ";
    }
    
    for (int i = 1; i < argc; i++) {
        if (argv[i] != NULL) {
            fprintf(output, "%s", argv[i]);
            if (i < argc - 1) {
                fprintf(output, "%s", separator);
            }
        }
    }
    fprintf(output, "\n");
}

int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("  ARGÜMAN BİRLEŞTİRME PROGRAMI\n");
    printf("========================================\n\n");
    
    // Program adını göster
    printf("Program adı: %s\n", argv[0]);
    printf("Toplam argüman sayısı: %d\n\n", argc);
    
    // Tüm argümanları listele
    printf("Girilen argümanlar:\n");
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = \"%s\"\n", i, argv[i] ? argv[i] : "(null)");
    }
    printf("\n");
    
    // ============================================================
    // YÖNTEM 1: Bellek tahsisi ile birleştirme (boşluk ayırıcı)
    // ============================================================
    printf("--- Yöntem 1: Bellek tahsisi ile (boşluk ayırıcı) ---\n");
    char *result = arguments_join_simple(argc, argv);
    
    if (result != NULL) {
        printf("Birleştirilmiş metin: \"%s\"\n", result);
        printf("Metin uzunluğu: %zu karakter\n", strlen(result));
        free(result);  // Belleği serbest bırak!
    } else {
        printf("Birleştirme başarısız! (Argüman yok veya bellek hatası)\n");
    }
    printf("\n");
    
    // ============================================================
    // YÖNTEM 2: Özel ayırıcı ile birleştirme
    // ============================================================
    printf("--- Yöntem 2: Özel ayırıcı ile ---\n");
    
    // Örnek 1: Virgül ve boşluk ile ayır
    char *result_comma = arguments_join(argc, argv, ", ");
    if (result_comma != NULL) {
        printf("Virgül ile: \"%s\"\n", result_comma);
        free(result_comma);
    }
    
    // Örnek 2: Ok işareti ile ayır
    char *result_arrow = arguments_join(argc, argv, " -> ");
    if (result_arrow != NULL) {
        printf("Ok ile:     \"%s\"\n", result_arrow);
        free(result_arrow);
    }
    
    // Örnek 3: Tire ile ayır
    char *result_dash = arguments_join(argc, argv, " - ");
    if (result_dash != NULL) {
        printf("Tire ile:   \"%s\"\n", result_dash);
        free(result_dash);
    }
    printf("\n");
    
    // ============================================================
    // YÖNTEM 3: Bellek tahsisi OLMADAN doğrudan yazdırma
    // ============================================================
    printf("--- Yöntem 3: Doğrudan yazdırma (bellek tahsisi yok) ---\n");
    printf("Birleştirilmiş: ");
    arguments_join_print(argc, argv, " ", stdout);
    printf("\n");
    
    // ============================================================
    // YÖNTEM 4: Farklı ayırıcılarla yazdırma
    // ============================================================
    printf("--- Yöntem 4: Farklı ayırıcı denemeleri ---\n");
    printf("Boşluk ile:     ");
    arguments_join_print(argc, argv, " ", stdout);
    
    printf("Virgül ile:     ");
    arguments_join_print(argc, argv, ", ", stdout);
    
    printf("Yeni satır ile: ");
    arguments_join_print(argc, argv, "\n", stdout);
    
    printf("Hiç ayırıcı:    ");
    arguments_join_print(argc, argv, "", stdout);
    printf("\n");
    
    // ============================================================
    // ÖZEL DURUM: Argümanların işlenmesi (robustluk testi)
    // ============================================================
    printf("--- Robustluk Testi ---\n");
    
    // Test 1: Argümansız çağrı (sadece program adı)
    char *test_argv1[] = {"./program", NULL};
    char *test_result1 = arguments_join_simple(1, test_argv1);
    printf("Test 1 (argümansız): %s\n", test_result1 ? "NULL döndü (doğru)" : "NULL döndü (doğru)");
    
    // Test 2: NULL argv
    char *test_result2 = arguments_join_simple(5, NULL);
    printf("Test 2 (NULL argv): %s\n", test_result2 ? "HATA!" : "NULL döndü (doğru)");
    
    // Test 3: NULL separator
    char *test_argv3[] = {"./program", "merhaba", "dunya", NULL};
    char *test_result3 = arguments_join(3, test_argv3, NULL);
    if (test_result3) {
        printf("Test 3 (NULL separator): \"%s\"\n", test_result3);
        free(test_result3);
    }
    
    printf("\n========================================\n");
    printf("  PROGRAM SONLANDI\n");
    printf("========================================\n");
    
    return EXIT_SUCCESS;
}