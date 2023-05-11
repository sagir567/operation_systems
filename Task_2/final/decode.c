#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <dlfcn.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("encode/decode <codec> <message>");
    }
    int size = strlen(argv[2]);
    printf("size:%d\n", size);
    char data[size+1];

    strcpy(data, argv[2]);

    char *codec = argv[1];



    if (strcmp(codec, "codecA") == 0) {

        void (*library);
        void (*method)(char[], int);
        library = dlopen("./codecA.so", RTLD_LAZY);


        if (!library) {
            perror("ERROR:dynamic library init failed\n");
            return 0;
        }
        method = (void (*)(char[], int)) dlsym(library, "codecA");

        if (!method) {
            perror("ERROR:method linker failure!\n");
            dlclose(library);
            return 1;
        }
        method(data, size);
        printf("%s\n", data);
        dlclose(library);
    }

    if (strcmp(codec, "codecB") == 0) {

        void (*library);
        void (*method)(bool, char[], int);
        library = dlopen("./codecB.so", RTLD_LAZY);
        if (!library) {
            perror("ERROR:dynamic library init failed\n");
            return -1;
        }

        method = (void (*)(bool, char[], int)) dlsym(library, "codecB");
        if (!method) {
            perror("ERROR:method linker failure!\n");
            dlclose(library);
            return 1;
        }

        method(false, data, size);
        printf("data:%s\n", data);
    }
}