#include "reactor.h"

// Function to create a new Reactor instance
// Returns a pointer to the created Reactor structure
Reactor* createReactor() {
    // Allocate memory for the Reactor structure
    Reactor* reactor = (Reactor*)malloc(sizeof(Reactor));
    if (reactor == NULL) {
        return NULL;  // Failed to allocate memory
    }

    // Initialize the Reactor fields
    reactor->events = NULL;
    reactor->event_count = 0;
    reactor->running = 0;
    reactor->pfds = NULL;
    reactor->fd_count = 0;
    reactor->fd_size = MAX_EVENTS;

    return reactor;
}

// Function to add an event to the Reactor
// reactor: Pointer to the Reactor structure
// id: Event ID
// handler: Event handler function pointer
void addEvent(Reactor* reactor, int id, EventHandler handler) {
    // Extend the events array to accommodate the new event
    reactor->events = (Event*)realloc(reactor->events, sizeof(Event) * (reactor->event_count + 1));
    if (reactor->events == NULL) {
        perror("addEvent");
        return;  // Failed to allocate memory
    }

    // Add the new event to the Reactor
    reactor->events[reactor->event_count].id = id;
    reactor->events[reactor->event_count].handler = handler;
    reactor->event_count++;
}

// Function to stop the Reactor
// reactor: Pointer to the Reactor structure
void stopReactor(Reactor* reactor) {
    reactor->running = 0;
}

// Function to handle an event
// reactor: Pointer to the Reactor structure
// id: Event ID
// context: Event context
void handleEvent(Reactor* reactor, int id, void* context) {
    size_t n = reactor->event_count;
    for (size_t i = 0; i < n; i++) {
        // Find the event with the matching ID and call its handler
        if (reactor->events[i].id == id) {
            reactor->events[i].handler(context);
            return;
        }
    }
}

// Function to free the memory occupied by the Reactor
// reactor: Pointer to the Reactor structure
void reactorDestructor(Reactor* reactor) {
    if (reactor == NULL) {
        perror("reactorDestructor");
        return;
    }

    // Free the allocated memory for events and pfds arrays
    if (reactor->events) {
        free(reactor->events);
    }
    if (reactor->pfds) {
        free(reactor->pfds);
    }

    // Free the Reactor structure itself
    free(reactor);
}

// Function to retrieve the IP address from a sockaddr structure
// sa: Pointer to the sockaddr structure
// Returns a void pointer to the IP address
void* get_in_addr(struct sockaddr* sa) {
    // Check the IP version and return the appropriate IP address pointer
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Function to set a socket as non-blocking
// sockfd: Socket file descriptor
// Returns 0 on success, -1 on failure
int set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        return -1;
    }
    return 0;
}

// Function to create a listener socket and bind it to a port
// Returns the listener socket file descriptor, or -1 on failure
int get_listener_socket(void) {
    int listener;
    int yes = 1;
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    // Iterate through the available address structures and create the listener socket
    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Allow reuse of the address
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        // Bind the socket to the address
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // Check if the loop reached the end without finding a valid address
    if (p == NULL) {
        freeaddrinfo(ai);
        return -1;
    }

    freeaddrinfo(ai);

    // Start listening on the socket
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

// Function to add a new file descriptor to the Reactor
// reactor: Pointer to the Reactor structure
// newfd: New file descriptor to add
// handler: Event handler function pointer
void addFd(Reactor* reactor, int newfd, EventHandler handler) {
    // If the Reactor's pfds array is full, double its size and reallocate memory
    if (reactor->fd_count == reactor->fd_size) {
        reactor->fd_size *= 2;

        reactor->pfds = realloc(reactor->pfds, sizeof(struct pollfd) * reactor->fd_size);
        if (reactor->pfds == NULL) {
            fprintf(stderr, "Failed to allocate memory for pfds\n");
            exit(1);
        }
    }

    // Add the new file descriptor and associated event to the Reactor
    reactor->pfds[reactor->fd_count].fd = newfd;
    reactor->pfds[reactor->fd_count].events = POLLIN;
    reactor->fd_count++;

    addEvent(reactor, newfd, handler);
}

// Function to remove a file descriptor from the Reactor's pfds array
// reactor: Pointer to the Reactor structure
// i: Index of the file descriptor to remove
void del_from_pfds(Reactor* reactor, int i) {
    // Copy the last file descriptor in the array to the position being removed
    reactor->pfds[i] = reactor->pfds[reactor->fd_count - 1];

    // Decrement the file descriptor count
    reactor->fd_count--;

    // Stop the Reactor if there are no more file descriptors
    if (reactor->fd_count == 0) {
        stopReactor(reactor);
    }
}

// Function to start the Reactor
// reactor: Pointer to the Reactor structure
void startReactor(Reactor* reactor) {
    // Check if the Reactor is already running
    if (reactor->running) {
        return;
    }

    reactor->running = 1;
    reactor->fd_count = 1;

    // Set the Reactor's listening socket as non-blocking
    set_nonblocking(reactor->pfds[0].fd);

    // Main loop of the Reactor
    while (reactor->running) {
        // Perform polling on the file descriptors
        int poll_count = poll(reactor->pfds, reactor->fd_count, 25000);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        // Array to store descriptors that need to be removed
        int descriptorsToRemove[reactor->fd_count];
        size_t descriptorsToRemoveCount = 0;

        for (size_t i = 0; i < reactor->fd_count; i++) {
            if (reactor->pfds[i].revents != 0) {
                if (reactor->pfds[i].revents & POLLIN) {
                    if (reactor->pfds[i].fd == reactor->pfds[0].fd) {
                        // New client connection
                        socklen_t addrlen;
                        int newfd;
                        struct sockaddr_storage remoteaddr;
                        char remoteIP[INET6_ADDRSTRLEN];

                        addrlen = sizeof remoteaddr;
                        newfd = accept(reactor->pfds[0].fd,
                            (struct sockaddr *)&remoteaddr,
                            &addrlen);

                        if (newfd == -1) {
                            perror("accept");
                        } else {
                            printf("pollserver: new connection from %s on "
                                "socket %d\n",
                                inet_ntop(remoteaddr.ss_family,
                                    get_in_addr((struct sockaddr*)&remoteaddr),
                                    remoteIP, INET6_ADDRSTRLEN),
                                newfd);

                            // Handle the new connection event
                            handleEvent(reactor, newfd, NULL);

                            // Add the new file descriptor to the Reactor
                            addFd(reactor, newfd, NULL);
                        }
                    } else {
                        // Data received from a client
                        int nbytes;
                        char buf[MAX_BUFFER_SIZE], msg[MAX_BUFFER_SIZE] = "Client socket ", num_c[5];
                        nbytes = recv(reactor->pfds[i].fd, buf, sizeof(buf), 0);

                        if (nbytes <= 0) {
                            if (nbytes == 0) {
                                printf("pollserver: socket %d hung up\n", reactor->pfds[i].fd);
                            } else {
                                perror("recv");
                            }

                            // Add the descriptor to the remove list
                            descriptorsToRemove[descriptorsToRemoveCount++] = i;

                            continue;
                        }

                        buf[nbytes] = '\0';
                        printf("Client socket %d: %s\n", reactor->pfds[i].fd, buf);
                        sprintf(num_c, "%d", reactor->pfds[i].fd);
                        strcat(msg, num_c);
                        strcat(msg, ": ");
                        strcat(msg, buf);

                        // Forward the received message to other connected clients
                        for (size_t j = 0; j < reactor->fd_count; j++) {
                            int dest_fd = reactor->pfds[j].fd;

                            if (dest_fd != reactor->pfds[0].fd && dest_fd != reactor->pfds[i].fd) {
                                if (send(dest_fd, msg, sizeof(msg), 0) == -1) {
                                    if (errno != EWOULDBLOCK && errno != EAGAIN) {
                                        perror("send");
                                        exit(1);
                                    }
                                }
                            }
                        }

                        memset(buf, 0, sizeof(buf));
                        memset(msg, 0, sizeof(msg));
                    }
                }

                if (reactor->pfds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                    printf("pollserver: socket %d error or hang up\n", reactor->pfds[i].fd);

                    // Close the socket and handle the event
                    close(reactor->pfds[i].fd);

                    handleEvent(reactor, reactor->pfds[i].fd, NULL);
                    descriptorsToRemove[descriptorsToRemoveCount++] = i;
                }
            }
        }

        // Remove descriptors that need to be removed
        for (size_t i = 0; i < descriptorsToRemoveCount; i++) {
            del_from_pfds(reactor, descriptorsToRemove[i]);
        }
    }
}

// Function to wait for the Reactor thread to complete
// this: Pointer to the Reactor structure
void waitFor(void* this) {
    Reactor* reactor = (Reactor*)this;

    // Wait for the Reactor thread to finish
    pthread_join(reactor->thread, NULL);
    pthread_detach(reactor->thread);
}
