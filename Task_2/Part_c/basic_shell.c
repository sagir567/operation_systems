#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>

#define commandLength 1024
#define argsLength 64
#define	input	0
#define	output	1

volatile sig_atomic_t is_running = 1;
struct sigaction sa;

void handle_signal(int sig) {
    if (sig == SIGINT) {
        if(is_running) {
            printf("\nStopping current process...\n");
        }
    }
    is_running = 0;
}

void commandExe(char **args, int input_fd, int output_fd) {
    pid_t pid = fork();

    if (pid == 0) { // child process
        if (input_fd != input) {
            dup2(input_fd, input);
            close(input_fd);
        }

        if (output_fd != output) {
            dup2(output_fd, output);
            close(output_fd);
        }

        execvp(args[0], args);
        exit(EXIT_FAILURE);

    } else if (pid < 0) {
        perror("fork");
    } else {
        int status;
        do {
            waitpid(pid, &status, 0);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

void commandParser(char* command, char** args, int* num_args, int* input_fd, int* output_fd) {
    char* token = strtok(command, " ");
    while (token != NULL) {
        if (strcmp(token, "exit") == 0) {
            printf("bye\n");
            exit(1);
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

            commandExe(args, *input_fd, pipe_fds[1]);
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

void change_directory(char *path) {
    if (chdir(path) != 0) {
        perror("chdir");
    }
}

void print_current_directory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s: ", cwd);
    } else {
        perror("getcwd");
    }
}

int main() {
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;

    sigaction(SIGINT, &sa, NULL);

    char command[commandLength];
    char* args[argsLength];
    int num_args = 0;
    int input_fd, output_fd;

    while (1) {
        num_args = 0;
        input_fd = input;
        output_fd = output;
        print_current_directory();

        fgets(command, commandLength, stdin);

        command[strcspn(command, "\n")] = '\0';
        commandParser(command, args, &num_args, &input_fd, &output_fd);
        if (!is_running) num_args = 0;

        if (num_args > 0) {
            if (strcmp(args[0], "cd") == 0) {
                if (num_args > 1) {
                    change_directory(args[1]);
                } else {
                    fprintf(stderr, "cd: Missing argument\n");
                }
            } else {
                commandExe(args, input_fd, output_fd);
            }
        }

        if (input_fd != input) {
            close(input_fd);
        }

        if (output_fd != output) {
            close(output_fd);
        }
        is_running = 1;
    }

    return 0;
}