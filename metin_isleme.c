/*
 * Öğrenci Adı: [Adınız Soyadınız]
 * Öğrenci No: [Öğrenci Numaranız]
 * Ders: Sistem Programlama
 * Ödev: Pointer Aritmetiği ile String'de Boşlukları Tire ile Değiştirme
 * 
 * Açıklama: Bu program, bir string içindeki boşluk karakterlerini (' ') 
 * tire karakteri ('-') ile değiştirir. İşlemler sırasında kesinlikle 
 * köşeli parantez (indeksleme operatörü) kullanılmaz, sadece pointer 
 * aritmetiği (*ptr, ptr++, vb.) kullanılır.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_STRING_LEN 1024

/**
 * replace_spaces_with_dash - String içindeki boşlukları tire ile değiştirir
 * 
 * @param str: Değiştirilecek string (pointer aritmetiği ile işlenecek)
 * 
 * Bu fonksiyon sadece pointer aritmetiği kullanır, [] operatörü kullanılmaz.
 * String üzerinde doğrudan değişiklik yapar (in-place modification).
 */
void replace_spaces_with_dash(char *str) {
    // Hata kontrolü: NULL pointer
    if (str == NULL) {
        return;
    }
    
    char *ptr = str;  // String'in başlangıcına pointer
    
    // String'in sonuna (null terminator) gelene kadar devam et
    while (*ptr != '\0') {
        // Eğer boşluk karakteri bulunduysa
        if (*ptr == ' ') {
            *ptr = '-';  // Boşluğu tire ile değiştir
        }
        ptr++;  // Bir sonraki karaktere geç (pointer aritmetiği)
    }
}

/**
 * replace_spaces_with_dash_copy - Orijinal string'i değiştirmeden yeni bir string döndürür
 * 
 * @param src: Kaynak string
 * @param dest: Hedef string (yeterli boyutta olmalı)
 * 
 * Bu fonksiyon kaynak string'i kopyalar ve kopya üzerinde değişiklik yapar.
 */
void replace_spaces_with_dash_copy(const char *src, char *dest) {
    if (src == NULL || dest == NULL) {
        return;
    }
    
    const char *src_ptr = src;
    char *dest_ptr = dest;
    
    while (*src_ptr != '\0') {
        if (*src_ptr == ' ') {
            *dest_ptr = '-';
        } else {
            *dest_ptr = *src_ptr;
        }
        src_ptr++;
        dest_ptr++;
    }
    *dest_ptr = '\0';  // Null terminator ekle
}

/**
 * replace_spaces_with_dash_static - Statik string üzerinde işlem yapar
 * 
 * @param str: String (dizi olarak tanımlanmış, pointer'a dönüşür)
 * 
 * Dizi ismi aslında pointer'dır, bu nedenle aynı mantıkla işlenebilir.
 */
void replace_spaces_with_dash_static(char str[]) {
    // str aslında bir pointer'dır! Dizi ismi pointer'a dönüşür (array decay)
    char *ptr = str;
    
    while (*ptr) {
        if (*ptr == ' ') *ptr = '-';
        ptr++;
    }
}

/**
 * print_string - String'i pointer aritmetiği ile ekrana yazdırır
 * 
 * @param str: Yazdırılacak string
 */
void print_string(const char *str) {
    if (str == NULL) {
        printf("(null)\n");
        return;
    }
    
    const char *ptr = str;
    while (*ptr != '\0') {
        putchar(*ptr);
        ptr++;
    }
    putchar('\n');
}

/**
 * get_string_length - String uzunluğunu pointer aritmetiği ile hesaplar
 * 
 * @param str: String
 * @return: Uzunluk (null terminator hariç)
 */
size_t get_string_length(const char *str) {
    if (str == NULL) {
        return 0;
    }
    
    const char *ptr = str;
    size_t len = 0;
    
    while (*ptr != '\0') {
        len++;
        ptr++;
    }
    
    return len;
}

int main() {
    printf("========================================\n");
    printf("  BOŞLUKLARI TİRE İLE DEĞİŞTİRME\n");
    printf("  (Pointer Aritmetiği ile)\n");
    printf("========================================\n\n");
    
    // ============================================================
    // YÖNTEM 1: Kullanıcıdan girdi alarak (en yaygın kullanım)
    // ============================================================
    printf("--- Yöntem 1: Kullanıcıdan girdi alarak ---\n");
    
    char user_input[MAX_STRING_LEN];
    printf("Bir metin giriniz: ");
    fflush(stdout);
    
    // Kullanıcı girdisini al (fgets güvenlidir, buffer overflow önler)
    if (fgets(user_input, sizeof(user_input), stdin) != NULL) {
        // fgets newline karakterini de alır, onu temizle
        char *newline_ptr = user_input;
        while (*newline_ptr != '\0') {
            if (*newline_ptr == '\n') {
                *newline_ptr = '\0';
                break;
            }
            newline_ptr++;
        }
        
        printf("Orijinal metin: ");
        print_string(user_input);
        
        // Boşlukları tire ile değiştir (pointer aritmetiği ile)
        replace_spaces_with_dash(user_input);
        
        printf("Değiştirilmiş metin: ");
        print_string(user_input);
    }
    printf("\n");
    
    // ============================================================
    // YÖNTEM 2: Kod içinde tanımlanmış string ile (static dizi)
    // ============================================================
    printf("--- Yöntem 2: Kod içinde tanımlanmış string ---\n");
    
    // Dizi olarak tanımlanmış string (değiştirilebilir)
    char str1[] = "Sistem Programlama Lab Uygulamasi";
    
    printf("Orijinal: ");
    print_string(str1);
    
    replace_spaces_with_dash(str1);
    
    printf("Değiştirilmiş: ");
    print_string(str1);
    printf("\n");
    
    // ============================================================
    // YÖNTEM 3: Örnek girdi ile (ödevde istenen örnek)
    // ============================================================
    printf("--- Yöntem 3: Ödevde istenen örnek ---\n");
    
    char example[] = "Sistem Programlama Lab Uygulamasi";
    
    printf("Girdi:  ");
    print_string(example);
    
    replace_spaces_with_dash(example);
    
    printf("Çıktı: ");
    print_string(example);
    printf("\n");
    
    // ============================================================
    // YÖNTEM 4: Farklı test senaryoları
    // ============================================================
    printf("--- Yöntem 4: Farklı test senaryoları ---\n");
    
    // Test 1: Birden fazla boşluk içeren
    char test1[] = "C   dilinde   pointer   aritmetiği";
    printf("Test 1 (çoklu boşluk): ");
    printf("\"%s\" -> ", test1);
    replace_spaces_with_dash(test1);
    printf("\"%s\"\n", test1);
    
    // Test 2: Başında ve sonunda boşluk olan
    char test2[] = "  başta ve sonda boşluk var  ";
    printf("Test 2 (baş/son boşluk): ");
    printf("\"%s\" -> ", test2);
    replace_spaces_with_dash(test2);
    printf("\"%s\"\n", test2);
    
    // Test 3: Boşluk olmayan
    char test3[] = "BoşluksuzMetin";
    printf("Test 3 (boşluksuz): ");
    printf("\"%s\" -> ", test3);
    replace_spaces_with_dash(test3);
    printf("\"%s\"\n", test3);
    
    // Test 4: Boş string
    char test4[] = "";
    printf("Test 4 (boş string): ");
    printf("\"%s\" -> ", test4);
    replace_spaces_with_dash(test4);
    printf("\"%s\"\n", test4);
    
    // Test 5: Tek karakterli string
    char test5[] = " ";
    printf("Test 5 (sadece boşluk): ");
    printf("\"%s\" -> ", test5);
    replace_spaces_with_dash(test5);
    printf("\"%s\"\n", test5);
    printf("\n");
    
    // ============================================================
    // YÖNTEM 5: Orijinali koruyarak kopya üzerinde işlem
    // ============================================================
    printf("--- Yöntem 5: Orijinali koruyarak işlem (kopya ile) ---\n");
    
    char original[] = "Bu orijinal metin değişmeyecek";
    char copy[MAX_STRING_LEN];
    
    printf("Orijinal: ");
    print_string(original);
    
    replace_spaces_with_dash_copy(original, copy);
    
    printf("Kopya (değiştirilmiş): ");
    print_string(copy);
    printf("Orijinal (hala aynı): ");
    print_string(original);
    printf("\n");
    
    // ============================================================
    // Pointer Aritmetiği ile String Uzunluğu Hesaplama
    // ============================================================
    printf("--- Pointer Aritmetiği ile Bilgiler ---\n");
    
    char demo[] = "Merhaba Dunya";
    printf("String: \"%s\"\n", demo);
    printf("Uzunluk (pointer ile): %zu karakter\n", get_string_length(demo));
    
    // Pointer aritmetiği ile karakterleri tek tek gösterme
    printf("Karakterler (pointer ile): ");
    char *ptr = demo;
    while (*ptr != '\0') {
        printf("%c ", *ptr);
        ptr++;
    }
    printf("\n");
    
    printf("========================================\n");
    printf("  PROGRAM SONLANDI\n");
    printf("========================================\n");
    
    return 0;
}