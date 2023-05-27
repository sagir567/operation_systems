#ifndef REACTOR_H
#define REACTOR_H

#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#define MAX_EVENTS 5   // Maximum number of events
#define MAX_BUFFER_SIZE 256   // Maximum buffer size
#define PORT "3940"

// Event structure
typedef struct {
    int id;
    void (*handler)(void*);
} Event;

// Reactor structure
typedef struct {
    Event* events;
    size_t event_count;
    int running;
    struct pollfd* pfds;
    size_t fd_count;
    size_t fd_size;
    pthread_t thread;
} Reactor;

// Event handler function pointer type
typedef void (*EventHandler)(void*);

Reactor* createReactor();
void stopReactor(Reactor* reactor);
void startReactor(Reactor* reactor);
void addEvent(Reactor* reactor, int id, EventHandler handler);
void waitFor(void* this);
void handleEvent(Reactor* reactor, int id, void* context);
void reactorDestructor(Reactor* reactor);
void* get_in_addr(struct sockaddr* sa);
int set_nonblocking(int sockfd);
int get_listener_socket(void);
void addFd(Reactor* reactor, int newfd, EventHandler handler);
void remove_fd(Reactor* reactor, int i);



#endif  // REACTOR_H
