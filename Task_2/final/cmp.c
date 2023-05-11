#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int Fcmp(bool flag, char* argv[]) {
    FILE *file1 = fopen(argv[1], "rb");
    FILE *file2 = fopen(argv[2], "rb");
    char f1[1024], f2[1024];
    
    if (file1 == NULL || file2 == NULL) {
        perror("ERROR: failed to open files!!!");
        return -1;
    }
    int i = 0, j = 0;
    while (fgets(f1, 1024, file1) && fgets(f2, 1024, file2)) {
        if (flag== true) {
            while (f1[i] != '\0') {
                f1[i] = tolower(f1[i]);
                i++;
            }
            while (f2[j] != '\0') {
                f2[j] = tolower(f2[j]);
                j++;
            }
        }
        if (strcmp(f1, f2) != 0) {
            fclose(file1);
            fclose(file2);
            return 1;
        }
        i = 0;
        j = 0;
    }
    if (feof(file1) && feof(file2)) {
        fclose(file1);
        fclose(file2);
        return 0;
    }
    else {
        fclose(file1);
        fclose(file2);
        return -1;
    }
}

int main(int argc, char *argv[]) {
    puts("welcome\n");
    if (argc < 3 || argc > 5) {
        printf("valid format: ./cmd <FILE1> <FILE2>\n");
        printf("flags: -v for literal result, -i for case sensitivity\n");
        return 1;
    }

    int res;
    switch (argc) {
        case 3:
            res = Fcmp(false, argv);
            printf("%d", res);
            break;

        case 4:
            res = 0;
            char *flag = argv[3];
            if (strcmp(flag, "-i") == 0) {
                res = Fcmp(true, argv);
                printf("%d", res);
            }
            else if (strcmp(flag, "-v") == 0) {
                res = Fcmp(false, argv);
                (res == 0) ? printf("equal") : printf("distinct");
            }
            else {
                printf("valid format: ./cmd <FILE1> <FILE2>\n");
                printf("flags: -v for literal result, -i for case sensitivity\n");
                return 1;
            }
            break;

        case 5:
            if(strcmp(argv[3],"-i")!=0&&strcmp(argv[3],"-v")!=0){
                printf("ima shelchem zona");
                break;
            }
            if(strcmp(argv[4],"-i")!=0&&strcmp(argv[4],"-v")!=0){
                printf("ima shelchem zona");
                break;
            }
     
            
            res = Fcmp(true, argv);
            (res == 0) ? printf("equal") : printf("distinct");
            break;

        return 0;
}
}
