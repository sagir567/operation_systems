#include "chat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void handleNewConnection(void* context) {
    Reactor* reactor = (Reactor*)context;
    int newfd = reactor->pfds[reactor->fd_count - 1].fd;

    // Customize handling of new connection here
    // For example, send a welcome message to the client
    const char* welcome_msg = "Welcome to the chat server!\n";
    if (send(newfd, welcome_msg, strlen(welcome_msg), 0) == -1) {
        perror("send");
        exit(1);
    }
}

void handleClientData(void* context) {
    int clientfd = *(int*)context;
    char buf[MAX_BUFFER_SIZE];

    int nbytes = recv(clientfd, buf, sizeof(buf), 0);
    if (nbytes <= 0) {
        // Handle error or connection closed by client
        if (nbytes == 0) {
            printf("pollserver: socket %d hung up\n", clientfd);
        } else {
            perror("recv");
        }

        close(clientfd);
    } else {
        buf[nbytes] = '\0';
        printf("Client socket %d: %s\n", clientfd, buf);

        // Handle client data here
        // For example, broadcast the message to other clients
        // Note: You need to implement the logic to send the message to other clients
    }
}
