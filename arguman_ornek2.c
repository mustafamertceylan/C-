/*
 * Öğrenci Adı: [Adınız Soyadınız]
 * Öğrenci No: [Öğrenci Numaranız]
 * Ders: Sistem Programlama
 * Ödev: Komut Satırı Argümanlarını İşleme
 * 
 * Açıklama: Bu program, komut satırından alınan argümanları işler.
 * - Argümanları listeler
 * - Her argümanın strlen() ve sizeof() değerlerini gösterir
 * - "marmara" kelimesini arar ve bulursa indeksini belirtir
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ARGS 100
#define SEARCH_WORD "marmara"

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
    printf("     KOMUT SATIRI ARGÜMAN İŞLEME PROGRAMI\n");
    print_separator();
}

/**
 * print_arg_info - Bir argümanın bilgilerini yazdırır
 * 
 * @param index: Argümanın indeksi
 * @param arg: Argümanın kendisi (string)
 */
void print_arg_info(int index, const char *arg) {
    if (arg == NULL) {
        return;
    }
    
    size_t length = strlen(arg);      // strlen: null karakter hariç uzunluk
    size_t size = sizeof(arg);         // sizeof: pointer'ın boyutu (8 byte, 64-bit sistemde)
    
    printf("  argv[%d] = \"%s\"\n", index, arg);
    printf("    -> strlen() = %zu (karakter sayısı, null hariç)\n", length);
    printf("    -> sizeof() = %zu (pointer boyutu, byte cinsinden)\n", size);
    printf("\n");
}

/**
 * print_arg_info_for_array - Dizi olarak tanımlanmış argümanın bilgilerini yazdırır
 * (sizeof farkını göstermek için alternatif versiyon)
 * 
 * @param index: Argümanın indeksi
 * @param arg: Argüman (char dizisi)
 * @param arg_size: Dizinin boyutu
 */
void print_arg_info_for_array(int index, const char *arg, size_t arg_size) {
    if (arg == NULL) {
        return;
    }
    
    size_t length = strlen(arg);      // strlen: null karakter hariç uzunluk
    
    printf("  argv[%d] = \"%s\"\n", index, arg);
    printf("    -> strlen() = %zu (karakter sayısı, null hariç)\n", length);
    printf("    -> sizeof() = %zu (dizinin toplam byte boyutu, null dahil)\n", arg_size);
    printf("\n");
}

/**
 * check_for_marmara - Argümanlar içinde "marmara" kelimesini arar
 * 
 * @param argc: Argüman sayısı
 * @param argv: Argüman dizisi
 * @return: Bulunan indeks (1-based), bulunamazsa -1
 */
int check_for_marmara(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i] != NULL) {
            if (strcmp(argv[i], SEARCH_WORD) == 0) {
                return i;  // Kelimenin indeksini döndür
            }
        }
    }
    return -1;  // Bulunamadı
}

/**
 * print_all_arguments - Tüm argümanları listeler ve bilgilerini yazdırır
 * 
 * @param argc: Argüman sayısı
 * @param argv: Argüman dizisi
 */
void print_all_arguments(int argc, char *argv[]) {
    printf("=== ARGÜMAN LİSTESİ ===\n\n");
    
    // Program adı (argv[0])
    printf("Program Adı:\n");
    printf("  argv[0] = \"%s\"\n", argv[0]);
    printf("    -> strlen() = %zu (program adının uzunluğu)\n", strlen(argv[0]));
    printf("    -> sizeof() = %zu (argv[0] pointer'ının boyutu)\n", sizeof(argv[0]));
    printf("\n");
    
    // Diğer argümanlar (argv[1]'den itibaren)
    if (argc > 1) {
        printf("Kullanıcı Argümanları:\n");
        for (int i = 1; i < argc; i++) {
            print_arg_info(i, argv[i]);
        }
    } else {
        printf("(Hiçbir kullanıcı argümanı girilmemiş)\n\n");
    }
}

/**
 * demonstrate_strlen_vs_sizeof - strlen() ve sizeof() arasındaki farkı gösterir
 * 
 * @param example_str: Örnek string
 */
void demonstrate_strlen_vs_sizeof(const char *example_str) {
    printf("\n=== strlen() vs sizeof() FARKI ===\n\n");
    
    // Örnek 1: String literal
    char *str_ptr = "Merhaba";
    char str_array[] = "Merhaba";
    
    printf("Örnek 1: String literal (char *str_ptr = \"Merhaba\")\n");
    printf("  strlen(str_ptr) = %zu (karakter sayısı: M,e,r,h,a,b,a = 7)\n", strlen(str_ptr));
    printf("  sizeof(str_ptr) = %zu (pointer'ın boyutu, 64-bit sistemde 8 byte)\n\n", sizeof(str_ptr));
    
    printf("Örnek 2: Karakter dizisi (char str_array[] = \"Merhaba\")\n");
    printf("  strlen(str_array) = %zu (karakter sayısı: 7)\n", strlen(str_array));
    printf("  sizeof(str_array) = %zu (dizinin toplam boyutu: 7 karakter + null = 8)\n\n", sizeof(str_array));
    
    printf("ÖNEMLİ:\n");
    printf("  - strlen() string'in UZUNLUĞUNU (null karakter hariç) hesaplar\n");
    printf("  - sizeof() değişkenin BELLEK BOYUTUNU (byte cinsinden) hesaplar\n");
    printf("  - Dizilerde sizeof() tüm dizinin boyutunu verir\n");
    printf("  - Pointer'larda sizeof() pointer'ın boyutunu verir (8 byte, 64-bit sistemde)\n");
}

int main(int argc, char *argv[]) {
    print_header();
    
    // ============================================================
    // KONTROL 1: Argüman kontrolü (program adı dışında argüman yoksa)
    // ============================================================
    if (argc <= 1) {
        printf("\n!!! UYARI !!!\n");
        printf("Program adı dışında hiçbir argüman girilmemiş!\n");
        printf("Kullanım: %s <kelime1> <kelime2> ...\n", argv[0]);
        printf("Örnek: %s merhaba dunya marmara bilgisayar\n\n", argv[0]);
        printf("Program sonlandırılıyor...\n");
        print_separator();
        return EXIT_FAILURE;
    }
    
    // ============================================================
    // BÖLÜM 1: Tüm argümanları listele
    // ============================================================
    print_all_arguments(argc, argv);
    
    // ============================================================
    // BÖLÜM 2: "marmara" kelimesini ara
    // ============================================================
    printf("=== \"marmara\" KELİMESİ ARANIYOR ===\n\n");
    
    int found_index = check_for_marmara(argc, argv);
    
    if (found_index != -1) {
        printf("🎉 BULUNDU! 🎉\n");
        printf("\"marmara\" kelimesi komut satırında %d. sırada (argv[%d]) girilmiştir.\n", 
               found_index, found_index);
        printf("Kelimenin kendisi: \"%s\"\n", argv[found_index]);
    } else {
        printf("❌ \"%s\" kelimesi argümanlar arasında BULUNAMADI.\n", SEARCH_WORD);
        printf("Girilen argümanlar: ");
        for (int i = 1; i < argc; i++) {
            printf("\"%s\" ", argv[i]);
        }
        printf("\n");
    }
    printf("\n");
    
    // ============================================================
    // BÖLÜM 3: strlen() vs sizeof() farkını göster
    // ============================================================
    demonstrate_strlen_vs_sizeof(NULL);
    
    // ============================================================
    // BÖLÜM 4: Özel durumlar ve uyarılar
    // ============================================================
    printf("\n=== ÖZEL DURUMLAR VE UYARILAR ===\n\n");
    
    // Boş string kontrolü
    printf("1. Boş string (\"\") için:\n");
    printf("   strlen(\"\") = %zu\n", strlen(""));
    printf("   sizeof(\"\") = %zu (string literal, 1 byte - sadece null karakter)\n\n", sizeof(""));
    
    // Uzun argüman uyarısı
    printf("2. Uzun argüman uyarısı:\n");
    printf("   strlen() çok uzun string'lerde bile doğru sonuç verir.\n");
    printf("   Ancak çok uzun argümanlar kabuk (shell) tarafından sınırlanabilir.\n\n");
    
    // strcmp() hakkında not
    printf("3. strcmp() hakkında:\n");
    printf("   strcmp(\"marmara\", \"marmara\") = 0 (eşit)\n");
    printf("   strcmp(\"marmara\", \"MARMARA\") = %d (büyük/küçük harf duyarlı)\n", 
           strcmp("marmara", "MARMARA"));
    printf("   strcmp(\"marmara\", \"marmar\") = %d (farklı uzunluk)\n", 
           strcmp("marmara", "marmar"));
    
    print_separator();
    printf("Program başarıyla tamamlandı.\n");
    print_separator();
    
    return EXIT_SUCCESS;
}