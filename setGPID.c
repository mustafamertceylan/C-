#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid, pgid;

    // 1. Yeni bir süreç oluştur (fork)
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    // 2. Ebeveyn süreç, çocuk oluşturduktan sonra sonlanır.
    //    Bu sayede çocuk, init sürecinin (PID 1) evlatlığı olur ve terminalden ayrılır.
    if (pid > 0) {
        printf("Ebeveyn (PID: %d) sonlanıyor.\n", getpid());
        exit(0);
    }

    // 3. BURASI ÇOCUK SÜREÇ (DAEMON ADAYI)
    //    Şu an çocuk süreç, ebeveyni ile AYNI süreç grubundadır.
    printf("Çocuk (PID: %d) başladı.\n", getpid());
    printf("Başlangıçta içinde bulunduğum süreç grubu (PGID): %d\n", getpgrp());

    // 4. YENİ BİR SÜREÇ GRUBU OLUŞTUR (Kendi grubunun lideri ol)
    //    setpgid(0,0) çağrısı: 
    //    - 0 (ilk argüman): Çağıran sürecin (yani bu çocuğun) grup kimliğini değiştir.
    //    - 0 (ikinci argüman): Yeni grup kimliğini, çağıran sürecin kendi PID'ine eşitle.
    if (setpgid(0, 0) == -1) {
        perror("setpgid");
        exit(1);
    }

    // 5. Artık daemon, yepyeni bir süreç grubunun LİDERİ oldu.
    printf("setpgid()'den sonraki süreç grubum (PGID): %d\n", getpgrp());
    printf("Artık ben bir daemon'um ve terminalden bağımsızım!\n");

    // 6. Bu noktadan sonra daemon'un yapması gereken işler (bir dosyaya log yazmak gibi)
    //    while(1) döngüsü içinde sürekli çalışabilir.
    
    return 0;
}