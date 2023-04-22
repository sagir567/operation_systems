#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int fCopy(bool flag1,bool flag2, char* argv[]) {
    FILE *file1 = fopen(argv[1], "rb");
    FILE *file2 = fopen(argv[2], "rb+");
    char data [1024];
    
    if (file1 == NULL ) {
        perror("ERROR: file1 does not exist!!\n");
        return 1;
    }
    if (file2 == NULL ) {// file2 dosent exist
     file2 = fopen(argv[2],"wb+");
     
     while(!feof(file1)){
        fgets(data,1024,file1);
        fwrite(data,1,sizeof(data),file2);
        }
        return 0;
    }
    // overwritng file2
    else if(flag1){
        file2 = fopen(argv[2],"wb+");
        while(!feof(file1)){
            fgets(data,1024,file1);
            fwrite(data,1,sizeof(data),file2);
        }
        return 0;
    }
    else if(!flag1){
    if (flag2)printf("ERROR: target file exist!\n");
        return 1;
    }
    return 1;
}



int main(int argc, char *argv[]) {
    puts("welcome to our new file transfaring program...\n");
    if (argc < 3 || argc > 5) {
        printf("usage copy <file1> <file2> \n");
        printf("flags: -v for literal result, -f to overwrite file2 if existed\n");
        return 1;
    }

    int res;
    switch (argc) {
        case 3: // copy the file but will not overwriten file2 if existed
            res = fCopy(false,false, argv);
            printf("%d", res);
            break;

        case 4: // one flag is on -v or -f
            res = 0;
            char *flag = argv[3];
            if (strcmp(flag, "-f") == 0) {
                res = fCopy(true,false, argv);
                printf("%d", res);
                break;
            }
            else if (strcmp(flag, "-v") == 0) {
                res = fCopy(false,true, argv);
                (res == 0) ? printf("succses\n") : printf("general failure\n");
                break;
            }
            else {
                printf("valid format: ./cmd <FILE1> <FILE2>\n");
                printf("flags: -v for literal result, -i for case sensitivity\n");
            break;    
            }
            

        case 5: // both flags are on -v,-f
            if(strcmp(argv[3],"-f")!=0&&strcmp(argv[3],"-v")!=0){
                printf("valid format: ./cmd <FILE1> <FILE2>\n");
                printf("flags: -v for literal result, -i for case sensitivity\n");
                break;
            }
            if(strcmp(argv[4],"-f")!=0&&strcmp(argv[4],"-v")!=0){
                printf("valid format: ./cmd <FILE1> <FILE2>\n");
                printf("flags: -v for literal result, -i for case sensitivity\n");
                break;
            }
     
            
            res = fCopy(true,true, argv);
            (res == 0) ? printf("succses\n") : printf("general failure\n");
            break;

        return 0;
}
}
