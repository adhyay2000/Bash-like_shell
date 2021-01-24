/*
you can allocate maximum of 10 shared memeory segment using shmcreate
*/
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<sys/ipc.h>
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
    //create a shared memory for updating the number of available  
    int shmkey=ftok("./shell.c",1);
    int shmid = shmget(shmkey,21*sizeof(int),0600|IPC_CREAT);
    // printf("original %d\n",shmid);
    void *shmadress=shmat(shmid,NULL,0);
    if(*(int*)shmadress==0){
        *(int*)shmadress=2;
    }
    // printf("data at shared memory %d\n",*(int*)shmadress);
    if(shmadress==(void*)-1){
        fprintf(stderr,"shmat return NULL ,errno(%d:%s)\n",errno,strerror(errno));
        return -1;
    }
    while(1){
        char *tmp,*path;
        char ttmp;
        char **arguments;
        tmp=(char*) malloc(sizeof(char)*100);
        printf(">");
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
                break;
            }
        }
        free(tmp);
        //arguments list contains the arguments and paths contains the path
        bool path_found=false;
        if(strcmp(arguments[0],"shmcreate")==0){
            int current_value=*(int*)shmadress;
            if(current_value-2>=10){
                printf("ERROR: upto 10 shared memory segment are allowed to be created\n");
                continue;                
            }
            int shmkey=ftok("./shell.c",current_value);
            *(int*)shmadress=current_value+1;
            int shmid=shmget(shmkey,atoi(arguments[1]),0600|IPC_CREAT|IPC_EXCL);
            if(shmid==-1){
                printf("ERROR: creating new shm,exiting with %d",errno);
                return -1;
            }
            *((int*)shmadress+(2*(current_value-2)+1))=shmid;
            *((int*)shmadress+(2*(current_value-2)+2))=atoi(arguments[1]);
        }else if(strcmp(arguments[0],"shmls")==0){
            printf("CURRENT ID:%d\n",*((int*)shmadress)-2);
            printf("SHMID SIZE\n");
            for(int i=0;i<10;i++){
                printf("%d %d\n",*((int*)shmadress+2*i+1),*((int*)shmadress+2*i+2));
            }
        }
        else{
            for(int i=0;i<100;i++){
                if(paths[i]==NULL) break;
                char buffer[110];
                memset(buffer,'\0',110);
                char *d1=strcpy(buffer,paths[i]);
                d1=strcat(d1,"/");
                d1=strcat(d1,arguments[0]);
                int status;
                if(access(d1,F_OK)==-1){
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
}