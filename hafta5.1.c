#include <stdlib.h>
#include <stdio.h>

void cleanup() {
    printf("Temizlik 1\n");
}

void cleanup2() {
    printf("Temizlik 2\n");
}

int main() {
    atexit(cleanup);   // 1. kayıt
    atexit(cleanup2);  // 2. kayıt

    printf("Ana program çalışıyor\n");
    exit(15);           // Program sonlanıyor
}