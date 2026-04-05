#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>   // umask() için GEREKLİ!
#include <signal.h>
#include <time.h>
#include <fcntl.h>


int main(){
    int fd =open("deneme.txt",O_RDWR|O_CREAT,0700);
    pid_t pid=fork();
    
    if(pid==0){
        write(fd,"Ben child",9);
    }
    else if(pid>0){
        write(fd,"Ben Parent",10);
    }
    else{
        printf("pid tanımlamasında hata meydana geldi");
    }

    close(fd);










    return 0;
}