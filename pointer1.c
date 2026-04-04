/*
 * Öğrenci Adı: [Adınız Soyadınız]
 * Öğrenci No: [Öğrenci Numaranız]
 * Ders: Sistem Programlama
 * Ödev: Pointer Aritmetiği ile String Ters Çevirme
 * 
 * Açıklama: Bu program, bir string'i pointer aritmetiği kullanarak ters çevirir.
 * Kesinlikle [] operatörü kullanılmaz, sadece pointer aritmetiği (*ptr, ptr++, ptr--, vb.)
 * kullanılır. String'in başlangıç ve bitiş adresleri üzerinden swap işlemi yapılır.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STRING_LEN 1024

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
    printf("     POINTER ARİTMETİĞİ İLE STRING TERS ÇEVİRME\n");
    print_separator();
}

/**
 * string_length - Pointer aritmetiği ile string uzunluğunu hesaplar
 * 
 * @param str: String'in başlangıç pointer'ı
 * @return: String'in uzunluğu (null terminator hariç)
 */
size_t string_length(const char *str) {
    if (str == NULL) {
        return 0;
    }
    
    const char *ptr = str;
    size_t len = 0;
    
    // Pointer aritmetiği ile string sonuna kadar ilerle
    while (*ptr != '\0') {
        len++;
        ptr++;  // Bir sonraki karaktere geç
    }
    
    return len;
}

/**
 * string_reverse - Pointer aritmetiği ile string'i ters çevirir
 * 
 * @param str: Ters çevrilecek string (in-place işlem)
 * 
 * Bu fonksiyon kesinlikle [] operatörü kullanmaz.
 * Sadece pointer aritmetiği (*ptr, ptr++, ptr--, vb.) kullanır.
 */
void string_reverse(char *str) {
    // Hata kontrolü: NULL pointer veya boş string
    if (str == NULL || *str == '\0') {
        return;
    }
    
    // Başlangıç pointer'ı
    char *start = str;
    
    // Bitiş pointer'ını bul (string'in son karakteri)
    char *end = str;
    while (*end != '\0') {
        end++;  // Null terminator'ün ötesine geç
    }
    end--;  // Son karaktere geri dön
    
    // Pointer aritmetiği ile swap işlemi
    // start ve end ortada buluşana kadar devam et
    while (start < end) {
        // Swap işlemi (pointer aritmetiği ile)
        char temp = *start;  // start'ın gösterdiği değeri geçici değişkene al
        *start = *end;       // end'in gösterdiği değeri start'a kopyala
        *end = temp;         // geçici değeri end'e kopyala
        
        // Pointer'ları birbirine doğru hareket ettir
        start++;  // Bir sonraki karaktere geç
        end--;    // Bir önceki karaktere geç
    }
}

/**
 * string_reverse_copy - Pointer aritmetiği ile string'i ters çevirip kopya oluşturur
 * 
 * @param src: Kaynak string
 * @param dest: Hedef string (yeterli boyutta olmalı)
 * 
 * Orijinal string'i değiştirmez, ters çevrilmiş halini dest'e kopyalar.
 */
void string_reverse_copy(const char *src, char *dest) {
    if (src == NULL || dest == NULL) {
        return;
    }
    
    // Kaynak string'in uzunluğunu bul
    size_t len = string_length(src);
    
    // Bitiş pointer'ını ayarla (kaynağın sonu)
    const char *src_ptr = src;
    while (*src_ptr != '\0') {
        src_ptr++;
    }
    src_ptr--;  // Son karaktere git
    
    // Ters çevrilmiş kopyayı oluştur
    char *dest_ptr = dest;
    const char *src_rev = src_ptr;
    
    while (src_rev >= src) {
        *dest_ptr = *src_rev;  // Karakteri kopyala
        dest_ptr++;
        src_rev--;
    }
    *dest_ptr = '\0';  // Null terminator ekle
}

/**
 * print_string_pointer - Pointer aritmetiği ile string yazdırır
 * 
 * @param str: Yazdırılacak string
 */
void print_string_pointer(const char *str) {
    if (str == NULL) {
        printf("(null)\n");
        return;
    }
    
    const char *ptr = str;
    while (*ptr != '\0') {
        putchar(*ptr);
        ptr++;
    }
}

/**
 * print_string_with_pointer_artihmetic - Pointer aritmetiği ile detaylı string yazdırır
 */
void print_string_with_pointer_artihmetic(const char *str) {
    if (str == NULL) {
        printf("(null)\n");
        return;
    }
    
    const char *ptr = str;
    int index = 0;
    
    printf("    Karakterler (pointer ile): ");
    while (*ptr != '\0') {
        printf("[%d]='%c' ", index, *ptr);
        ptr++;
        index++;
    }
    printf("\n");
}

/**
 * get_user_input - Kullanıcıdan string alır (güvenli)
 * 
 * @param buffer: Kullanıcı girdisinin yazılacağı buffer
 * @param size: Buffer boyutu
 * @param prompt: Gösterilecek prompt mesajı
 */
void get_user_input(char *buffer, size_t size, const char *prompt) {
    if (buffer == NULL || size == 0) {
        return;
    }
    
    printf("%s", prompt);
    fflush(stdout);
    
    if (fgets(buffer, size, stdin) != NULL) {
        // Newline karakterini temizle (pointer aritmetiği ile!)
        char *ptr = buffer;
        while (*ptr != '\0') {
            if (*ptr == '\n') {
                *ptr = '\0';
                break;
            }
            ptr++;
        }
    }
}

/**
 * demonstrate_pointer_arithmetic - Pointer aritmetiğinin nasıl çalıştığını gösterir
 */
void demonstrate_pointer_arithmetic(const char *str) {
    printf("\n=== POINTER ARİTMETİĞİ DEMONSTRASYONU ===\n\n");
    
    if (str == NULL || *str == '\0') {
        printf("(gösterilecek string yok)\n");
        return;
    }
    
    const char *ptr = str;
    int position = 0;
    
    printf("String: \"%s\"\n", str);
    printf("Adresler:\n");
    
    while (*ptr != '\0') {
        printf("  str + %d = %p (değer: '%c', ASCII: %d)\n", 
               position, (void*)ptr, *ptr, (int)*ptr);
        ptr++;
        position++;
    }
    
    printf("\nNot: ptr++ işlemi, bir sonraki karakterin adresine geçer.\n");
    printf("      char* tipinde +1 işlemi adrese 1 byte ekler.\n");
}

int main() {
    print_header();
    
    // ============================================================
    // BÖLÜM 1: Statik olarak tanımlanmış string ile
    // ============================================================
    printf("\n--- 1. Statik Olarak Tanımlanmış String ---\n\n");
    
    // Dizi olarak tanımlanmış string (değiştirilebilir)
    char static_str[] = "Sistem Programlama";
    
    printf("Orijinal string: \"%s\"\n", static_str);
    print_string_with_pointer_artihmetic(static_str);
    
    // Ters çevir
    string_reverse(static_str);
    
    printf("\nTers çevrilmiş: \"%s\"\n", static_str);
    print_string_with_pointer_artihmetic(static_str);
    
    // ============================================================
    // BÖLÜM 2: Kullanıcıdan alınan string ile
    // ============================================================
    printf("\n--- 2. Kullanıcıdan Alınan String ---\n\n");
    
    char user_str[MAX_STRING_LEN];
    get_user_input(user_str, sizeof(user_str), "Bir metin giriniz: ");
    
    if (user_str[0] != '\0') {
        printf("\nOrijinal string: \"%s\"\n", user_str);
        print_string_with_pointer_artihmetic(user_str);
        
        // Ters çevir
        string_reverse(user_str);
        
        printf("\nTers çevrilmiş: \"%s\"\n", user_str);
        print_string_with_pointer_artihmetic(user_str);
    } else {
        printf("Hiçbir girdi yapılmadı.\n");
    }
    
    // ============================================================
    // BÖLÜM 3: Orijinali koruyarak kopya üzerinde ters çevirme
    // ============================================================
    printf("\n--- 3. Orijinali Koruyarak Ters Çevirme (Kopya ile) ---\n\n");
    
    char original[] = "Merhaba Dunya";
    char reversed[MAX_STRING_LEN];
    
    printf("Orijinal string: \"%s\"\n", original);
    
    string_reverse_copy(original, reversed);
    
    printf("Ters çevrilmiş kopya: \"%s\"\n", reversed);
    printf("Orijinal (değişmedi): \"%s\"\n", original);
    
    // ============================================================
    // BÖLÜM 4: Farklı test senaryoları
    // ============================================================
    printf("\n--- 4. Farklı Test Senaryoları ---\n\n");
    
    // Test 1: Boş string
    char test1[] = "";
    printf("Test 1 (boş string): \"%s\" -> ", test1);
    string_reverse(test1);
    printf("\"%s\"\n", test1);
    
    // Test 2: Tek karakterli string
    char test2[] = "A";
    printf("Test 2 (tek karakter): \"%s\" -> ", test2);
    string_reverse(test2);
    printf("\"%s\"\n", test2);
    
    // Test 3: İki karakterli string
    char test3[] = "AB";
    printf("Test 3 (iki karakter): \"%s\" -> ", test3);
    string_reverse(test3);
    printf("\"%s\"\n", test3);
    
    // Test 4: Palindrome (tersi kendisine eşit olan)
    char test4[] = "kabak";
    printf("Test 4 (palindrome - kabak): \"%s\" -> ", test4);
    string_reverse(test4);
    printf("\"%s\"\n", test4);
    
    // Test 5: Boşluklu string
    char test5[] = "Merhaba Dunya Nasilsin";
    printf("Test 5 (boşluklu): \"%s\" ->\n      ", test5);
    string_reverse(test5);
    printf("\"%s\"\n", test5);
    
    // Test 6: Özel karakterli string
    char test6[] = "123!@#ABC";
    printf("Test 6 (özel karakterler): \"%s\" -> ", test6);
    string_reverse(test6);
    printf("\"%s\"\n", test6);
    
    // ============================================================
    // BÖLÜM 5: Pointer Aritmetiği Gösterimi
    // ============================================================
    char demo_str[] = "Pointer";
    demonstrate_pointer_arithmetic(demo_str);
    
    // ============================================================
    // BÖLÜM 6: Uzunluk hesaplama demonstrasyonu
    // ============================================================
    printf("\n=== UZUNLUK HESAPLAMA (Pointer Aritmetiği ile) ===\n\n");
    
    char length_str[] = "Algoritma";
    size_t len = string_length(length_str);
    
    printf("String: \"%s\"\n", length_str);
    printf("Uzunluk: %zu karakter\n", len);
    
    // Pointer aritmetiği ile karakterleri tek tek göster
    printf("Karakterler (pointer ile): ");
    char *ptr = length_str;
    while (*ptr != '\0') {
        printf("%c ", *ptr);
        ptr++;
    }
    printf("\n");
    
    print_separator();
    printf("Program başarıyla tamamlandı.\n");
    print_separator();
    
    return 0;
}