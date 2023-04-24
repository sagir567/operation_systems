#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>

void handle_sigint(int sig){
    printf("\n exit trough ctrl c, %d, shell is still running\n", sig);
}


int countPipes(char* command){
    int count = 0;
    char* last = strchr(command,'|');
    while (last != NULL){
        printf("\n%s",last);
        if(*(last-1) != '\\'){
            count++;
        }
        last = strchr(last+1,'|');
    }
    return count;
}
struct cmdline{
    char *argv[10];
    struct cmdline* next;
}cmdl;

struct cmdline* parse_by_pipes(char* command , int size){

    int i;
    char* tokenp,*token;
    i = 0;

    tokenp = strchr(command,'|');
    while(*(tokenp-1)!='\\'){
        tokenp = strchr (tokenp+1,'|');
    }
    // found where is the pipe
    
    while (token != NULL)
    {
        argv[i] = token;
        token = strtok (NULL, " ");
        i++;
    }

}
char* parseCommand(char command[1024]){
    int i;
    char *argv[10];
    char *token;
    i = 0;
    if(strchr(command,'|')!=NULL){

    }
    token = strtok (command," ");
    while (token != NULL)
    {
        argv[i] = token;
        token = strtok (NULL, " ");
        i++;
    }
}
//int main(){
//    char command[1024];
//    fgets(command, 1024, stdin);
//    command[strlen(command) - 1] = '\0'; // replace \n with \0
//    printf("\n %d",countPipes(command));
//
//}
int main1() {
    int i;
    char *argv[10];
    char command[1024];
    char *token;

    signal (SIGINT , handle_sigint);


    while (1) {
        printf("hello: ");
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0'; // replace \n with \0
        int pipes_amount = countPipes(command);

        char** commands = parse_by_pipes(command,pipes_amount);
        /* parse command line */
        //first lets find pipe
        if(pipes_amount>0){
            int pipeline[pipes_amount];
            pipe(pipeline);
        }
        i = 0;
        token = strtok (command," ");
        while (token != NULL)
        {
            argv[i] = token;
            token = strtok (NULL, " ");
            i++;
        }
        argv[i] = NULL;

        if(strcmp(argv[0],"cd")==0){
            if(argv[1]== NULL){
                printf("cd:missing arguments\n");
            continue;
            }
            else if(!strchr(argv[1],'|')){
                if (chdir(argv[1]) != 0) {
                    perror("cd:general failure ");
                }
            }
            else if(argv[2][0]!='|'){

            }
        }
        /* Is command empty */
        if (argv[0] == NULL)
            continue;


        /* for commands not part of the shell command language */
        if (fork() == 0) {
            execvp(argv[0], argv);
        }
        wait(NULL);
    }
}
