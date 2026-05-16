#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
 #include <sys/wait.h>
 #include<string.h>
int main(){
int fd[2];
pid_t child;

if(pipe(fd)==-1){
    perror("pipe hatası");
    exit(1);
}
child=fork();

if(child==0){
    char str[]="hafta 6 uygulama sorusu 2";

   close(fd[0]);
   write(fd[1],str,strlen(str)); 
}
else if(child>0){
    wait(NULL);
    char str[100];
    close(fd[1]);
    read(fd[0],str,100);
    printf("%s",str);
    
}
else{
    printf("fork oluşturulamadı....\n program sonlanıyor\n");
    exit(1);
}








    return 0;
}