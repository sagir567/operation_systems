#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
bool isClient;

void socketHandler(int client_sock) {
    char buffer[BUFFER_SIZE];
    ssize_t in;

    while ((in = recv(client_sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[in] = '\0';
        printf(isClient ? "in from server" : "in from client:");
        printf("%s", buffer);
        fflush(stdout);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        printf("Usage:\n");
        printf("  Client: stnc -c IP PORT\n");
        printf("  Server: stnc -s PORT\n");
        return 1;
    }

    int soket;
    struct sockaddr_in addr;
    isClient = strcmp(argv[1], "-c") == 0;

    if ((soket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("ERROR: failed to create socket ");
        return 1;
    }

    addr.sin_family = AF_INET; // ipv4 protocol
    addr.sin_port = htons(atoi(argv[isClient ? 3 : 2])); // check port index

    if (isClient) {
        addr.sin_addr.s_addr = inet_addr(argv[2]);
        if (connect(soket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("ERROR: fail to connect server");
            return 1;
        }
    } else {
        addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(soket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("ERROR: fail to bind socket");
            return 1;
        }

        if (listen(soket, 1) == -1) {
            perror("ERROR: fail to connect client ");
            return 1;
        }
        soket = accept(soket, NULL, NULL);
    }



    if (fork() == 0) { // this child print income message
        socketHandler(soket);
    } else { // this father sends messages
        char buffer[BUFFER_SIZE];
        ssize_t out;

        while ((out = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
            if (!strcmp(buffer, "exit")) break;
            send(soket, buffer, out, 0);
        }

        printf(isClient ? "disconnect from server..." : "shutdown server!!!...");
        shutdown(soket, SHUT_WR);
    }

    return 0;
}