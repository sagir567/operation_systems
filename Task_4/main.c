#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include "reactor.h"



void handleNewConnection(void* context) {
    int newfd = *((int*)context);
    printf("New connection, socket %d\n", newfd);
}

void handleClientData(void* context) {
    int sockfd = *((int*)context);
    char buffer[256];
    ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            printf("Socket %d hung up\n", sockfd);
        } else {
            perror("recv");
        }
        close(sockfd);
    } else {
        buffer[bytesRead] = '\0';
        printf("Socket %d: %s\n", sockfd, buffer);
    }
}

int main() {
    int listener;

    listener = get_listener_socket();

    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    struct pollfd* pfds = calloc(MAX_EVENTS, sizeof(struct pollfd));
    if (pfds == NULL) {
        fprintf(stderr, "Failed to allocate memory for pfds\n");
        exit(1);
    }

    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    Reactor* reactor = createReactor();

    if (reactor == NULL) {
        fprintf(stderr, "error creating reactor\n");
        exit(1);
    }

    reactor->pfds = pfds;

    addEvent(reactor, listener, handleNewConnection);

    startReactor(reactor);

    waitFor(reactor);

    reactorDestructor(reactor);
    close(listener);
    free(pfds);

    return 0;
}
