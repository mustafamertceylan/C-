#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
 #include <sys/wait.h>
 #include<string.h>
int main(){

int fd[2];
int i=0;
if(pipe(fd)==-1){
    perror("pipe hatası");
    exit(1);
}
while(1){
    char a[]="A";
    write(fd[1],a,sizeof(a)-1);
    printf("%d\n",i++);
}







    return 0;
}