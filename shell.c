#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<ctype.h>
#include<sys/wait.h>
extern char **environ;

int main(){
    //extract all possible paths
    char *ret=getenv("PATH");
    char **paths=(char**) calloc(100,sizeof(char*));
    int index=0;
    for(int i=0;i<100;i++){
        paths[i]=(char*) calloc(100,sizeof(char));
        for(int j=0;j<100;j++){
            if(*ret==':' || iscntrl(*ret)) break;
            paths[i][j]=*ret;
            ret++;
        }
        if(*ret==':') ret++;
        if(iscntrl(*ret)) break;
    }
    // for(int i=0;i<100;i++){
    //     if(paths[i]==NULL) break;
    //     puts(paths[i]);
    // }
    while(1){
        char *tmp,*path;
        char ttmp;
        char **arguments;
        tmp=(char*) malloc(sizeof(char)*100);
        scanf("%[^\n]",tmp);
        scanf("%c",&ttmp);
        char* command = tmp;
        arguments=(char**)calloc(11,sizeof(char*));
        for(int i=0;i<10;i++){
            arguments[i]=(char*) calloc(100,sizeof(char));
            for(int j=0;j<100;j++){
                arguments[i][j]=*command++;
                if(*command==' ' || iscntrl(*command)) break;
            }
            if(*command==' ') command++;
            if(iscntrl(*command)) {
                // printf("Number of argument scanned are %d\n",i);
                break;
            }
        }
        free(tmp);
        //arguments list contains the arguments and paths contains the path
        bool path_found=false;
        // sleep(1);
        for(int i=0;i<100;i++){
            if(paths[i]==NULL) break;
            char buffer[110];
            memset(buffer,'\0',110);
            char *d1=strcpy(buffer,paths[i]);
            d1=strcat(d1,"/");
            d1=strcat(d1,arguments[0]);
            int status;
            if(access(d1,F_OK)==-1){
                // printf("file not present at %s\n",d1);
                continue;
            }else{
                int pid=fork();
                if(pid==0){
                    execv(d1,arguments);
                    exit(0);
                }else{
                    wait(&status);
                    printf("\nchild pid %d exited with status %d\n\n",pid,status);
                }
                path_found=true;
                break;
            }
        }
        if(path_found==false){
            printf("%s:command not found\n",arguments[0]);
        }
    }
}