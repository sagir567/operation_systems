#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE

#include <dlfcn.h>

int main(int argc, char *argv[]){
    while(argc!=3){
        printf("encode/decode <codec> <message>");
    }
    char *codec = argv[1];
    int flag =(strcmp(argv[0],"decode"))?0:1;

    if(strcmp(codec,"codecA")==0){
        void (*library);
        void(*method)(char* [],int);
        library = dlopen("./codecA.so",RTLD_LAZY);

    
        if(!library){
        perror("ERROR:dynamic library init faild\n");
        return 1;
        }
        method = (void (*)(char*[], int))dlsym(library,"codecA");
    
        if(!method){
            perror("ERROR:method linker failure!\n");
            dlclose(library);
            return 1;
        }
        method(argv,flag);
        printf("%s",argv[2]);
        dlclose (library);
    }


    
}