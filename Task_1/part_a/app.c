#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int split(char *command)
{
    char *token;
    token = strtok(command, " ");
    printf("%s\n",token);
    puts(token);
    while((token = strtok(NULL, " "))){

        puts(token);
    }
    return 1;
}
//cmpNULL file1NULL file2NULL       file3NULL
int main(int argc , char *argv[])
{
      puts("welcome\n");
    if (argc<3||argc >5 ){
        printf("valid format : ./cmd <FILE1> <FILE2>\n");
        printf("flags : -v for literal result, -i for case sensevisty\n");
    }
    char command[1024];
  
    while (1)
    {

        fgets(command, 1024, stdin);

        split(command);
    }

    return 0;
}