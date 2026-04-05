#include <stdio.h>
#include <sys/resource.h>
#include <errno.h>

int main() {
    errno = 0; // Önce errno'yu sıfırla
    int nice_value = getpriority(PRIO_PROCESS, 0);

    if (nice_value == -1 && errno != 0) {
        perror("getpriority failed");
        return 1;
    }
    printf("Mevcut nice değeri: %d\n", nice_value);
    return 0;
}