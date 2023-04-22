#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE

#include <dlfcn.h>

int main(int argc, char *argv[]){
    while(argc!=3){
        printf("encode/decode <codec> <message>");
    }
    int size = strlen(argv[2]);
    printf("size:%d\n",size);
    char data[size];
    strcpy(data,argv[2]);
    char *codec = argv[1];
    int flag =(strcmp(argv[0],"decode"))?0:1;

    if(strcmp(codec,"codecA")==0){
        void (*library);
        void(*method)(char[],int);
        library = dlopen("./codecA.so",RTLD_LAZY);

    
        if(!library){
        perror("ERROR:dynamic library init failed\n");
        return 1;
        }
        method = (void (*)(char[], int))dlsym(library,"codecA");
    
        if(!method){
            perror("ERROR:method linker failure!\n");
            dlclose(library);
            return 1;
        }
        method(data,size);
        printf("%s\n",data);
        dlclose (library);
    }


    
}