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
#define input 0
#define output 1

// Global variable to indicate whether the shell is running or not
volatile sig_atomic_t is_running = 1;
struct sigaction sa;

// Signal handler for SIGINT (Ctrl+C)
void handle_signal(int sig) {
    if (sig == SIGINT) {
        if (is_running) {
            printf("\nquiting process ...\n");
        }
    }
    is_running = 0;
}

// Function to execute a command with the specified input and output file descriptors
void commandExe(char **args, int input_fd, int output_fd) {
    // Fork a new process
    pid_t pid = fork();

    if (pid == 0) { // child process
        // If input file descriptor is not standard input, duplicate and close it
        if (input_fd != input) {
            dup2(input_fd, input);
            close(input_fd);
        }

        // If output file descriptor is not standard output, duplicate and close it
        if (output_fd != output) {
            dup2(output_fd, output);
            close(output_fd);
        }

        // Execute the command with the given arguments
        execvp(args[0], args);
        // If the command fails to execute, exit with an error status
        exit(EXIT_FAILURE);

    } else if (pid < 0) {
        // If forking fails, print an error message
        perror("ERROR: general failure forking!!!");
    } else {
        // Parent process waits for the child to finish executing the command
        int status;
        do {
            waitpid(pid, &status, 0);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}
void commandParser(char* command, char** args, int* argc, int* fdIn, int* fdOut) {
    char* token = strtok(command, " ");
    while (token != NULL) {
        if (strcmp(token, "exit") == 0) {
            printf("closing terminal\n");
            exit(1);
        } else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            *fdIn = open(token, O_RDONLY);
            if (*fdIn < 0) {
                perror("open");
                return;
            }
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            *fdOut = open(token, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if (*fdOut < 0) {
                perror("open");
                return;
            }
        } else if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, " ");
            *fdOut = open(token, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
            if (*fdOut < 0) {
                perror("ERROR: general failure");
                return;
            }
        } else if (strcmp(token, "|") == 0) {
            int pipe_fds[2];
            pipe(pipe_fds);

            commandExe(args, *fdIn, pipe_fds[1]);
            close(pipe_fds[1]);

            *argc = 0;
            *fdIn = pipe_fds[0];
        } else {
            args[*argc] = token;
            (*argc)++;
        }

        token = strtok(NULL, " ");
    }
    args[*argc] = NULL;
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
        perror("ERROR : failed to retrieve directory");
    }
}

int main() {
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;

    sigaction(SIGINT, &sa, NULL);

    char command[commandLength];
    char* args[argsLength];
    int argc = 0;
    int fdIn, fdOut;

    while (1) {
        argc = 0;
        fdIn = input;
        fdOut = output;
        print_current_directory();

        fgets(command, commandLength, stdin);

        command[strcspn(command, "\n")] = '\0';
        commandParser(command, args, &argc, &fdIn, &fdOut);
        if (!is_running) argc = 0;

        if (argc > 0) {
            if (strcmp(args[0], "cd") == 0) {
                if (argc > 1) {
                    change_directory(args[1]);
                } else {
                    fprintf(stderr, "cd:ERROR: argument!!!\n");
                }
            } else {
                commandExe(args, fdIn, fdOut);
            }
        }

        if (fdIn != input) {
            close(fdIn);
        }

        if (fdOut != output) {
            close(fdOut);
        }
        is_running = 1;
    }


}