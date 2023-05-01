
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define commandLength 1024
#define argsLength 64
#define	Standard_input	0
#define	Standard_output	1

volatile sig_atomic_t is_running = 1;
struct sigaction sa;
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\nStopping current process...\n");

    }
}

void runCommand(char** args, int num_args, int input_fd, int output_fd) {

    pid_t pid = fork();

    if (pid == 0) {//child process
        if (input_fd != Standard_input) {

            dup2(input_fd, Standard_input);
            close(input_fd);

        }

        if (output_fd != Standard_output) {

            dup2(output_fd, Standard_output);

            close(output_fd);

        }


            execvp(args[0], args);
            exit(EXIT_FAILURE);


    }
    else if (pid < 0) {
        perror("fork");
    } else {
        int status;
        do {
            waitpid(pid, &status, 0);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

/*
    O_WRONLY: This flag means "open for writing only". 
                
    O_CREAT: This flag means "create the file if it does not exist". 
    O_TRUNC: This flag means "truncate the file". If the file already exists and is opened for writing,
    its length is truncated to 0, effectively deleting its contents.
    
    S_IRWXU: Read, write, and search, or execute, for the file owner;
    O_RDONLY flag is used when opening a file in read-only mode
    “>>” operator appends an already present file or creates a new file
*/
void parse_command(char* command, char** args, int* num_args, int* input_fd, int* output_fd) {

    char* token = strtok(command, " ");
    while (token != NULL) {
        if(strcmp(token, "exit") == 0){
            printf("bye\n");
            exit(1);
        } else if (strcmp(token, "cd") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                if (chdir(token) != 0) {
                    perror("chdir");
                }
            }
        } else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            *input_fd = open(token, O_RDONLY);
            if (*input_fd < 0) {
                perror("open");
                return;
            }
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            *output_fd = open(token, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if (*output_fd < 0) {
                perror("open");
                return;
            }
        } else if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, " ");
            *output_fd = open(token, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
            if (*output_fd < 0) {
                perror("open");
                return;
            }

        } else if (strcmp(token, "|") == 0) {
            int pipe_fds[2];
            pipe(pipe_fds);

            runCommand(args, *num_args, *input_fd, pipe_fds[1]);
            close(pipe_fds[1]);

            *num_args = 0;
            *input_fd = pipe_fds[0];
        } else {
            args[*num_args] = token;
            (*num_args)++;
        }

        token = strtok(NULL, " ");
    }
    args[*num_args] = NULL;

}


int main() {

    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;

    sigaction(SIGINT, &sa, NULL);

    char command[commandLength];
    char* args[argsLength];
    int num_args = 0;
    int input_fd, output_fd;

    while (is_running) {
        num_args = 0;
        input_fd = Standard_input;
        output_fd = Standard_output;
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s$ ", cwd);
        } else {
            perror("getcwd");
            printf("stshell$ ");
        }
        // fflush(stdout);
        fgets(command, commandLength, stdin);

        command[strcspn(command, "\n")] = '\0';
        parse_command(command, args, &num_args, &input_fd, &output_fd);
        if(!is_running)num_args = 0;
        if (num_args > 0) {


            runCommand(args, num_args, input_fd, output_fd);
        }

        if (input_fd != Standard_input) {
            close(input_fd);
        }

        if (output_fd != Standard_output) {
            close(output_fd);
        }

    }

    return 0;
}