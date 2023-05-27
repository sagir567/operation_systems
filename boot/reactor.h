#ifndef REACTOR_H
#define REACTOR_H

#include <poll.h>
#include <pthread.h>

// Definition of event handler function type
typedef void* (*EventHandler)(void*);

// Definition of Event struct
typedef struct {
    int id;  // Event identifier
    EventHandler handler;  // Event handler function
} Event;

// Definition of Reactor struct
typedef struct {
    Event* events;  // Array of events
    size_t event_count;  // Number of events
    int running;  // Flag to indicate if the reactor is running
    struct pollfd* pfds;  // Array of poll file descriptors
    int fd_count;  // Number of file descriptors
    int fd_size;  // Size of the file descriptor array
	pthread_t thread;
} Reactor;

Reactor* createReactor();
void stopReactor(Reactor* reactor);
void addEvent(Reactor* reactor, int id, EventHandler handler);
void waitForReactor(Reactor* reactor,void * );
void handleEvent(Reactor* reactor, int id, void* context);
void reactorDestuctor(Reactor* reactor);

#endif
