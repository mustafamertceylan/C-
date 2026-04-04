/*
 * Öğrenci Adı: [Adınız Soyadınız]
 * Öğrenci No: [Öğrenci Numaranız]
 * Ders: Sistem Programlama
 * Ödev: Dinamik String Dizisi (char **) ile Argümanları Heap'te Saklama
 * 
 * Açıklama: Bu program, komut satırı argümanlarını dinamik olarak heap üzerinde
 * oluşturulan bir string dizisine (char **) kopyalar. Tüm bellek yönetimi
 * (malloc/free) programcı tarafından yapılır. Valgrind ile bellek sızıntısı
 * kontrolü yapılmalıdır.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_ARRAY_SIZE 10

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
    printf("     DİNAMİK STRİNG DİZİSİ (char **) İLE ARGÜMAN KOPYALAMA\n");
    print_separator();
}

/**
 * allocate_string_array - char** için bellek ayırır
 * 
 * @param count: Dizideki eleman sayısı
 * @return: Ayrılan bellek adresi (NULL hata durumunda)
 */
char **allocate_string_array(int count) {
    if (count <= 0) {
        return NULL;
    }
    
    // char* tipinde count adet pointer için bellek ayır
    char **arr = (char **)malloc(count * sizeof(char *));
    
    if (arr == NULL) {
        fprintf(stderr, "HATA: char** için bellek ayrılamadı!\n");
        return NULL;
    }
    
    return arr;
}

/**
 * copy_string_to_heap - Bir string'i heap'e kopyalar
 * 
 * @param src: Kaynak string
 * @return: Heap'te ayrılan kopyanın adresi (NULL hata durumunda)
 */
char *copy_string_to_heap(const char *src) {
    if (src == NULL) {
        return NULL;
    }
    
    // String uzunluğu + 1 (null terminator) kadar bellek ayır
    size_t len = strlen(src) + 1;
    char *dest = (char *)malloc(len);
    
    if (dest == NULL) {
        fprintf(stderr, "HATA: String için bellek ayrılamadı: \"%s\"\n", src);
        return NULL;
    }
    
    // String'i kopyala
    strcpy(dest, src);
    
    return dest;
}

/**
 * copy_arguments_to_heap - Komut satırı argümanlarını heap'e kopyalar
 * 
 * @param argc: Argüman sayısı
 * @param argv: Argüman dizisi
 * @return: Heap'te oluşturulan string dizisi (NULL hata durumunda)
 */
char **copy_arguments_to_heap(int argc, char *argv[]) {
    if (argc <= 1 || argv == NULL) {
        return NULL;
    }
    
    // Argüman sayısı kadar pointer için bellek ayır (argc-1 kullanıcı argümanı)
    // Not: argv[0] (program adı) dahil değil, sadece kullanıcı argümanları
    int num_args = argc - 1;
    char **heap_args = allocate_string_array(num_args);
    
    if (heap_args == NULL) {
        return NULL;
    }
    
    // Her bir argümanı heap'e kopyala
    for (int i = 0; i < num_args; i++) {
        heap_args[i] = copy_string_to_heap(argv[i + 1]);  // argv[1]'den başla
        
        if (heap_args[i] == NULL) {
            // Hata durumunda daha önce ayrılan belleği temizle
            for (int j = 0; j < i; j++) {
                free(heap_args[j]);
                heap_args[j] = NULL;
            }
            free(heap_args);
            return NULL;
        }
    }
    
    return heap_args;
}

/**
 * free_string_array - Dinamik string dizisini serbest bırakır
 * 
 * @param arr: Serbest bırakılacak string dizisi
 * @param count: Dizideki eleman sayısı
 * 
 * Double free hatasını önlemek için her free sonrası pointer NULL yapılır.
 */
void free_string_array(char **arr, int count) {
    if (arr == NULL) {
        return;
    }
    
    // Önce her bir string'i serbest bırak
    for (int i = 0; i < count; i++) {
        if (arr[i] != NULL) {
            free(arr[i]);
            arr[i] = NULL;  // Dangling pointer'ı önle
        }
    }
    
    // Sonra pointer dizisini serbest bırak
    free(arr);
    arr = NULL;  // Dangling pointer'ı önle
}

/**
 * print_string_array - Dinamik string dizisini yazdırır
 * 
 * @param arr: Yazdırılacak string dizisi
 * @param count: Dizideki eleman sayısı
 */
void print_string_array(char **arr, int count) {
    if (arr == NULL || count <= 0) {
        printf("(boş dizi)\n");
        return;
    }
    
    printf("\n=== KOPYALANAN ARGÜMANLAR (HEAP) ===\n\n");
    
    for (int i = 0; i < count; i++) {
        printf("  heap_args[%d] = \"%s\"\n", i, arr[i]);
        printf("    -> Adres: %p\n", (void*)arr[i]);
        printf("    -> Uzunluk: %zu karakter\n", strlen(arr[i]));
        printf("    -> Bellek boyutu: %zu byte (null dahil)\n\n", strlen(arr[i]) + 1);
    }
}

/**
 * demonstrate_memory_layout - Bellek düzenini gösterir
 */
void demonstrate_memory_layout(char **heap_args, int num_args) {
    printf("\n=== BELLEK DÜZENİ ===\n\n");
    
    printf("heap_args (char**) adresi: %p\n", (void*)heap_args);
    printf("heap_args boyutu: %zu byte (%d adet char* pointer)\n\n", 
           num_args * sizeof(char *), num_args);
    
    for (int i = 0; i < num_args; i++) {
        printf("  heap_args[%d] = %p -> \"%s\"\n", 
               i, (void*)heap_args[i], heap_args[i]);
    }
    
    printf("\nGörsel Bellek Düzeni:\n");
    printf("┌─────────────────────────────────────────────────────────┐\n");
    printf("│ heap_args (char**) : %-14p │\n", (void*)heap_args);
    printf("├─────────────────────────────────────────────────────────┤\n");
    
    for (int i = 0; i < num_args && i < 5; i++) {
        printf("│ heap_args[%d]        : %-14p │\n", i, (void*)&heap_args[i]);
        printf("│   └─> string[%d]     : %-14p -> \"%s\" │\n", 
               i, (void*)heap_args[i], heap_args[i]);
        printf("├─────────────────────────────────────────────────────────┤\n");
    }
    
    if (num_args > 5) {
        printf("│ ... (%d eleman daha)                                    │\n", num_args - 5);
        printf("├─────────────────────────────────────────────────────────┤\n");
    }
    
    printf("└─────────────────────────────────────────────────────────┘\n");
}

/**
 * demonstrate_memory_allocation - Bellek tahsis sürecini gösterir
 */
void demonstrate_memory_allocation(int argc, char *argv[]) {
    printf("\n=== BELLEK TAHSİS SÜRECİ ===\n\n");
    
    int num_args = argc - 1;
    
    printf("1. Adım: char** için bellek tahsisi\n");
    printf("   malloc(%d * sizeof(char*)) = malloc(%zu) byte\n", 
           num_args, num_args * sizeof(char *));
    printf("   Bu alan, %d adet char* pointer'ı tutar.\n\n", num_args);
    
    printf("2. Adım: Her argüman için bellek tahsisi\n");
    for (int i = 1; i < argc; i++) {
        size_t len = strlen(argv[i]) + 1;
        printf("   Argüman %d: \"%s\" -> malloc(%zu) byte (\"%s\" + null)\n", 
               i, argv[i], len, argv[i]);
    }
    printf("\n");
    
    printf("3. Adım: Bellek kullanımı\n");
    size_t total_pointers = num_args * sizeof(char *);
    size_t total_strings = 0;
    for (int i = 1; i < argc; i++) {
        total_strings += strlen(argv[i]) + 1;
    }
    printf("   - Pointer dizisi: %zu byte\n", total_pointers);
    printf("   - String'ler: %zu byte\n", total_strings);
    printf("   - TOPLAM: %zu byte\n\n", total_pointers + total_strings);
}

int main(int argc, char *argv[]) {
    print_header();
    
    // ============================================================
    // BÖLÜM 1: Argüman kontrolü
    // ============================================================
    printf("\nKomut satırı argümanları:\n");
    printf("  argc = %d\n", argc);
    
    if (argc > 1) {
        printf("  argv[0] (program adı) = \"%s\"\n", argv[0]);
        printf("  Kullanıcı argümanları (%d adet):\n", argc - 1);
        for (int i = 1; i < argc; i++) {
            printf("    argv[%d] = \"%s\"\n", i, argv[i]);
        }
    } else {
        printf("\n!!! UYARI !!!\n");
        printf("Program adı dışında hiçbir argüman girilmemiş!\n");
        printf("Kullanım: %s <arg1> <arg2> ...\n", argv[0]);
        printf("Örnek: %s merhaba dunya marmara\n\n", argv[0]);
        
        // Test için varsayılan argümanlarla devam et
        printf("Test modu: Varsayılan argümanlar kullanılacak.\n");
        
        // Test argümanları oluştur
        char *test_argv[] = {argv[0], "test1", "test2", "test3", "Sistem", "Programlama", NULL};
        int test_argc = 6;
        
        printf("\nTest argümanları: ");
        for (int i = 1; i < test_argc; i++) {
            printf("\"%s\" ", test_argv[i]);
        }
        printf("\n\n");
        
        argv = test_argv;
        argc = test_argc;
    }
    
    // ============================================================
    // BÖLÜM 2: Bellek tahsisi ve kopyalama
    // ============================================================
    int num_args = argc - 1;
    
    demonstrate_memory_allocation(argc, argv);
    
    printf("=== BELLEK TAHSİSİ VE KOPYALAMA ===\n\n");
    printf("%d argüman heap'e kopyalanıyor...\n\n", num_args);
    
    char **heap_args = copy_arguments_to_heap(argc, argv);
    
    if (heap_args == NULL) {
        fprintf(stderr, "HATA: Bellek tahsisi başarısız!\n");
        return EXIT_FAILURE;
    }
    
    // ============================================================
    // BÖLÜM 3: Kopyalanan argümanları yazdır
    // ============================================================
    print_string_array(heap_args, num_args);
    
    // ============================================================
    // BÖLÜM 4: Bellek düzenini göster
    // ============================================================
    demonstrate_memory_layout(heap_args, num_args);
    
    // ============================================================
    // BÖLÜM 5: Bellek sızıntısı olmaması için temizlik
    // ============================================================
    printf("\n=== BELLEK TEMİZLİĞİ (free) ===\n\n");
    printf("1. Önce her bir string (char*) serbest bırakılır:\n");
    
    for (int i = 0; i < num_args; i++) {
        printf("   free(heap_args[%d]) -> \"%s\" serbest bırakıldı\n", i, heap_args[i]);
    }
    
    printf("\n2. Sonra pointer dizisi (char**) serbest bırakılır:\n");
    printf("   free(heap_args) -> pointer dizisi serbest bırakıldı\n");
    
    // Serbest bırak
    free_string_array(heap_args, num_args);
    heap_args = NULL;
    
    printf("\n✓ Tüm bellek başarıyla serbest bırakıldı.\n");
    printf("✓ Bellek sızıntısı (memory leak) yok.\n");
    
    // ============================================================
    // BÖLÜM 6: Valgrind kullanım talimatları
    // ============================================================
    printf("\n=== VALGRIND KULLANIM TALİMATLARI ===\n\n");
    printf("Programı valgrind ile test etmek için:\n\n");
    printf("  valgrind --leak-check=full ./program arg1 arg2 arg3\n\n");
    printf("Örnek:\n");
    printf("  valgrind --leak-check=full ./%s merhaba dunya marmara\n\n", argv[0]);
    printf("Beklenen valgrind çıktısı:\n");
    printf("  \"All heap blocks were freed -- no leaks are possible\"\n");
    printf("  \"0 errors from 0 contexts\"\n");
    
    print_separator();
    printf("Program başarıyla tamamlandı.\n");
    print_separator();
    
    return EXIT_SUCCESS;
}