#include <stdio.h>
#include <stdlib.h>


int copy(char* nameFile1,char* nameFile2,char flag){
    
    FILE *file_1; 
    FILE *file_2; 
    char txt1[2056];
    file_1 = fopen(nameFile1, "rb");
    file_2 = fopen(nameFile2, "rb+");

    if (file_1 == NULL ) {
        perror("Error: The file we want to copy from does not exist");
        exit(1);
    }

    if (file_2 == NULL) {//the file is not exist
        
        file_2 = fopen(nameFile2, "wb+");
        while (!feof(file_1)) {
            fgets(txt1, 2056, file_1);            
            fwrite(txt1,1,sizeof(txt1),file_2);
        }
        return 0;

    }
    else if(file_2 != NULL && flag != 'f'){
        printf("There is no permission to write to the file");
        return 1;
    }
    else if(file_2 != NULL && flag == 'f'){
         while (!feof(file_1)) {
            fgets(txt1, 2056, file_1);           
            fwrite(txt1,1,sizeof(txt1),file_2);
        }
        return 0;

    }

    return 1;
}


int main(int argc, char *argv[]){
    if(argc < 3){
        printf("The file names are missing\n");
        exit(1);
    }
    char *namefile1 = argv[1];
    char *namefile2 = argv[2];
    if(argc == 3){
        printf("%d\n", copy(namefile1,namefile2,' '));   
    }
    else if(argc == 4)
    {
        char *flag =  argv[3];
        if(flag[1] == 'v'){
            if(copy(namefile1,namefile2,' ') == 0)
                printf("success\n");
            else
                printf("failure\n");

        }
        else if(flag[1] == 'f'){
            printf("%d\n",copy(namefile1,namefile2,'f')); 
        }
    }
    else if(argc == 5 ){
        char *flag =  argv[3];
        char *flag2 =  argv[4];
        if((flag[1] == 'i' || flag[1] == 'f') && (flag2[1] == 'i' || flag2[1] == 'v'))
        {
            if(copy(namefile1,namefile2,'f') == 0)
                printf("success\n");
            else
                printf("failure\n");
            }
    }
    else{
        printf("ERROR");
    }
    

    return 0;
}