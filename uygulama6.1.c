#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
 #include <sys/wait.h>
int main(){

pid_t child;


child=fork();

if(child==0){
    if (setpgid(0, 0) == -1) {
            perror("setpgid");
            exit(1);
        }
    printf("%d  %d\n",getpid(),getpgrp());
}
else if(child>0){
    wait(NULL);
    printf("ben  parent %d %d\n",getpid(),getppid());
}
else{
    printf("fork oluşturulamadı....\n program sonlanıyor\n");
    exit(1);
}
printf("program sonlanıyor..");







    return 0;
}