/*
 * Öğrenci Adı: [Adınız Soyadınız]
 * Öğrenci No: [Öğrenci Numaranız]
 * Ders: Sistem Programlama
 * Ödev: DumpHex Fonksiyonu - Bellek içeriğini hexadecimal olarak yazdırma
 * 
 * Açıklama: Bu program, verilen bir bellek adresindeki baytları 
 * hexadecimal formatında ekrana yazdırır.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * DumpHex - Verilen bellek adresindeki baytları hexadecimal olarak yazdırır
 * 
 * @param pData: İncelenecek bellek bölgesinin başlangıç adresi (void*)
 * @param byteLen: Yazdırılacak bayt sayısı
 * 
 * Çıktı formatı: "The X bytes starting at 0xADRES are: XX XX XX ..."
 * 
 * Not: Bu fonksiyon, herhangi bir veri tipinin bellek içeriğini 
 *       (endianness dahil) olduğu gibi gösterir.
 */
void DumpHex(void *pData, int byteLen) {
    // 1. Hata kontrolü: Geçersiz pointer veya sıfır uzunluk
    if (pData == NULL || byteLen <= 0) {
        fprintf(stderr, "Hata: Geçersiz parametreler!\n");
        return;
    }
    
    // 2. Bellek adresini yazdır (void* doğrudan %p ile basılabilir)
    printf("The %d bytes starting at %p are:", byteLen, pData);
    
    // 3. unsigned char pointer kullanarak baytlara erişim
    //    (char* kullanımı, byte-byte erişim için standart yöntemdir)
    unsigned char *bytePtr = (unsigned char *)pData;
    
    // 4. Tüm baytları döngü ile gez
    for (int i = 0; i < byteLen; i++) {
        // Her baytı 2 basamaklı hexadecimal olarak yazdır
        // %02x: 2 basamak, küçük harf, 0 ile doldurulmuş hexadecimal
        printf(" %02x", bytePtr[i]);
    }
    
    // 5. Satır sonu
    printf("\n");
}

/**
 * Alternatif DumpHex versiyonu - satır başına belirli sayıda bayt gösterir
 * (opsiyonel, robust kod örneği)
 */
void DumpHexFormatted(void *pData, int byteLen, int bytesPerLine) {
    if (pData == NULL || byteLen <= 0) {
        fprintf(stderr, "Hata: Geçersiz parametreler!\n");
        return;
    }
    
    if (bytesPerLine <= 0) {
        bytesPerLine = 16; // Varsayılan değer
    }
    
    unsigned char *bytePtr = (unsigned char *)pData;
    
    printf("The %d bytes starting at %p are:\n", byteLen, pData);
    
    for (int i = 0; i < byteLen; i++) {
        // Satır başında adres bilgisi (isteğe bağlı)
        if (i % bytesPerLine == 0) {
            printf("  %04x: ", i);
        }
        
        // Baytı yazdır
        printf("%02x ", bytePtr[i]);
        
        // Satır sonu kontrolü
        if ((i + 1) % bytesPerLine == 0 || i == byteLen - 1) {
            printf("\n");
        }
    }
}

int main(int argc, char **argv) {
    // Test değişkenleri
    char charVal = '0';
    int32_t intVal = 1;
    float floatVal = 1.0f;
    double doubleVal = 1.0;
    
    // Struct tanımı ve değişkeni
    typedef struct {
        char charVal;
        int32_t intVal;
        float floatVal;
        double doubleVal;
    } Ex2Struct;
    
    Ex2Struct structVal = { '0', 1, 1.0f, 1.0 };
    
    // DumpHex fonksiyonunu her değişken için çağır
    printf("=== DumpHex Fonksiyonu Testi ===\n\n");
    
    DumpHex(&charVal, sizeof(char));
    DumpHex(&intVal, sizeof(int32_t));
    DumpHex(&floatVal, sizeof(float));
    DumpHex(&doubleVal, sizeof(double));
    DumpHex(&structVal, sizeof(structVal));
    DumpHex(&DumpHex, sizeof(DumpHex));
    
    printf("\n=== Formatted DumpHex (opsiyonel) ===\n\n");
    DumpHexFormatted(&structVal, sizeof(structVal), 4);
    
    return EXIT_SUCCESS;
}

/* 
 * PROGRAM ÇIKTISI (örnek - adresler sisteme göre değişir):
 * 
 * === DumpHex Fonksiyonu Testi ===
 * 
 * The 1 bytes starting at 0x7ffc8d5c3b3f are: 30
 * The 4 bytes starting at 0x7ffc8d5c3b40 are: 01 00 00 00
 * The 4 bytes starting at 0x7ffc8d5c3b44 are: 00 00 80 3f
 * The 8 bytes starting at 0x7ffc8d5c3b48 are: 00 00 00 00 00 00 f0 3f
 * The 24 bytes starting at 0x7ffc8d5c3b20 are: 30 00 00 00 01 00 00 00 00 00 80 3f 00 00 00 00 00 00 f0 3f 00 00 00 00
 * 
 * === Formatted DumpHex (opsiyonel) ===
 * 
 * The 24 bytes starting at 0x7ffc8d5c3b20 are:
 *   0000: 30 00 00 00 
 *   0004: 01 00 00 00 
 *   0008: 00 00 80 3f 
 *   0012: 00 00 00 00 
 *   0016: 00 00 f0 3f 
 *   0020: 00 00 00 00 
 */

/* 
 * TEKNİK AÇIKLAMALAR:
 * 
 * 1. Neden unsigned char* kullanıyoruz?
 *    - char*, byte-byte bellek erişimi için standarttır
 *    - unsigned kullanımı, negatif değerlerle uğraşmamak içindir
 *    - Farklı pointer tipleri arasında cast yapmak C'de geçerlidir
 * 
 * 2. %p format belirteci:
 *    - void* tipindeki pointer'ları yazdırmak için kullanılır
 *    - Adresin formatı (hex, büyüklük) sisteme bağlıdır
 * 
 * 3. %02x format belirteci:
 *    - %x: hexadecimal (küçük harf)
 *    - %02: en az 2 basamak, eksik basamakları 0 ile doldur
 *    - Örnek: 0x0F -> "0f", 0x01 -> "01"
 * 
 * 4. Endianness (Byte Sıralaması):
 *    - x86/x64 işlemciler little-endian kullanır
 *    - Bu nedenle 1 sayısı bellekte "01 00 00 00" olarak görünür
 *    - Ağ protokolleri genellikle big-endian kullanır
 * 
 * 5. Struct Bellek Düzeni (Padding/Hizalama):
 *    - Derleyici, performans için struct üyeleri arasına boşluk ekleyebilir
 *    - Bu nedenle sizeof(Ex2Struct) üyelerin toplam boyutundan büyük olabilir
 *    - Örnek: char(1) + int(4) + float(4) + double(8) = 17 byte
 *    - Ancak alignment nedeniyle 24 byte olabilir (sisteme bağlı)
 * 
 * 6. Robust kod özellikleri:
 *    - NULL pointer kontrolü
 *    - Geçersiz byteLen kontrolü
 *    - Alternatif formatlı çıktı seçeneği
 *    - Açıklayıcı hata mesajları
 */