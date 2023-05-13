#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <netdb.h>
#include <sys/un.h>
#include "clientTCP.c"

#define SIZE 1024 * 1024 * 100
#define PATH "sample.txt"
#define MAX_SOCKETS 6
#define CHUNK 1024

bool isServer;
bool isClient;
bool isQuite;
bool isTest;
char *IP = "";
int PORT = 0;

int IPv4TCP(int socket)
{
    FILE *file = fopen(PATH, "a");
    if (file == NULL) // Check if file opened successfully
    {
        perror("IPv4TCP: failed to open file");
        return 0;
    }

    char buffer[CHUNK];
    ssize_t bytesRead;
    while ((bytesRead = read(socket, buffer, CHUNK)) > 0)
    {
        size_t bytesWritten = fwrite(buffer, sizeof(char), bytesRead, file);
        if (bytesWritten < bytesRead)
        {
            perror("IPv4TCP:failed to write to file");
            fclose(file);
            return 0;
        }
    }

    if (bytesRead < 0)
    {
        perror("IPv4TCP:failed to read from socket");
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}
int create_socket(int domain, int type, int protocol, void *addr, socklen_t addrlen)
{
    printf("Creating socket\n"); // Added newline
    int s = socket(domain, type, protocol);
    if (s < 0)
    {
        perror("socket()");
        exit(1);
    }
    if (bind(s, addr, addrlen) < 0)
    {
        perror("bind()");
        close(s);
        exit(1);
    }
    return s;
}

void generateFile()
{
    FILE *file;
    file = fopen("sample.txt", "w");
    if (fseek(file, SIZE, SEEK_SET))
        perror("ERROR:");
    fputs("this is the end of the file", file);
    fclose(file);
}
void printUsage()
{

    printf("Usage:\n"
           "client side:\n"
           "\tstnc -c IP PORT -p <type> <param>\n"
           "\t-p will indicate to perform the test\n"
           "\t<type> will be the communication types: so it can be ipv4,ipv6,mmap,pipe,uds\n"
           "\t<type> will be the communication types: so it can be ipv4,ipv6,mmap,pipe,uds\n"
           "\tavilable methods:\n"
           "\t\tIPv4 tcp\n"
           "\t\tIPv4 udp\n"
           "\t\tIPv6 tcp\n"
           "\t\tIPv6 udp\n"
           "\t\tUDS dgram\n"
           "\t\tUDS stream\n"
           "\t\tmmap <file>\n"
           "\t\tpipe <file>\n"
           "server side:\n"
           "\tstnc -s port -p (p for performance test) -q (q for quiet)\n"
           "\t-p flag will let you know that we are going to test the performance.\n"
           "\t-q flag will enable quiet mode, in which only testing results will be printed."
           "\t\tpipe <file>\n"
           "\t\tpipe <file>\n");
}
int main(int argc, char *argv[])
{
    // this CASE WILL check the input
    // TODO: change to while loop with regeting input from user.
    // if (argc < 4 || argc >7){
    // printUsage();
    //     exit(1);
    // }    if(tcpSocket = socket(AF_INET6, SOCK_STREAM, 0)==-1){
    printf("init program\n");
    isClient = (!strcmp(argv[1], "-c")) ? true : false;
    isServer = (!strcmp(argv[1], "-s")) ? true : false;
    // stnc -c IP PORT -p <type> <param>
    if (isClient)
    {
        printf("client\n");
        IP = argv[2];
        PORT = atoi(argv[3]);
        if (!strcmp(argv[5], "ipv4"))
        {
            printf("Port:%d \n ip:%s\n", PORT, IP);
            printf("%d", sendIPv4(PORT, IP));
        }
        else if (!strcmp(argv[5], "ipv6"))
            sendIPv6(PORT, IP);
    }
    // The server side: stnc -s port -p (p for performance test) -q (q for quiet)
    // -p flag will let you know that we are going to test the performance.
    // -q flag will enable quiet mode, in which only testing results will be printed.
    // (This is extremely important for the automatic test.) the results will be in milliseconds (ms) and printed like this name_type,time

    if (isServer)
    {
        printf("server\n");
        struct pollfd fds[MAX_SOCKETS];
        int PORT = atoi(argv[2]); // if were in the server we know that the port number should be on  argv[2]
                                  // Create and bind all the sockets

        printf("build ipv4 \n");
        printf("PORT:%d\n", PORT);
        struct sockaddr_in addr_in;
        addr_in.sin_family = AF_INET;
        addr_in.sin_port = htons(PORT);
        addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
        printf("-----------");
        // printf("build ipv6 \n");
        // struct sockaddr_in6 addr_in6;
        // addr_in6.sin6_family = AF_INET6;
        // addr_in6.sin6_port = htons(PORT + 1);
        // addr_in6.sin6_addr = in6addr_any;

        struct sockaddr_un addr_un;

        addr_un.sun_family = AF_UNIX;
        strcpy(addr_un.sun_path, PATH);
        printf("-------fd0-----------\n");
        int socketFD = create_socket(AF_INET, SOCK_STREAM, 0, &addr_in, sizeof(addr_in));
        if (socketFD == -1)
        {
            printf("Failed to create socket. Exiting...\n");
            return 1; // or any other error handling you prefer
        }
        fds[0].fd = socketFD;
        printf("Socket created!\n");

        // printf("--------fd1----------");
        // fds[1].fd = create_socket(AF_INET, SOCK_DGRAM, 0, &addr_in, sizeof(addr_in)); // ipv4 udp
        // if (listen(fds[1].fd, SOMAXCONN) < 0)
        // { // NEW LINE
        //     perror("listen:ipv4 udp");
        //     exit(EXIT_FAILURE);
        // }
        // printf("----------fd2--------");
        // fds[2].fd = create_socket(AF_INET6, SOCK_STREAM, 0, &addr_in6, sizeof(addr_in6)); // ipv6 tcp
        // if (listen(fds[2].fd, SOMAXCONN) < 0)
        // { // NEW LINE
        //     perror("listen:ipv6 tcp");
        //     exit(EXIT_FAILURE);
        // }
        printf("--------fd3----------");
        // fds[3].fd = create_socket(AF_INET6, SOCK_DGRAM, 0, &addr_in6, sizeof(addr_in6)); // ipv6 udp
        // if (listen(fds[3].fd, SOMAXCONN) < 0)
        // { // NEW LINE
        //     perror("listen:ipv6 udp");
        //     exit(EXIT_FAILURE);
        // }

        // fds[4].fd = create_socket(AF_UNIX, SOCK_STREAM, 0, &addr_un, sizeof(addr_un));    // uds strem

        // fds[5].fd = create_socket(AF_UNIX, SOCK_DGRAM, 0, &addr_un, sizeof(addr_un));     // uds dgram

        // Set up the poll parameters for each socket
        printf("------------------");
        for (int i = 0; i < MAX_SOCKETS; i++)
        {
            fds[i].events = POLLIN;
        }

        // Poll the sockets
        while (1)
        {

            int n = poll(fds, MAX_SOCKETS, 2);
            if (n < 0)
            {
                perror("poll");
                exit(1);
            }
                
            // Handle the sockets that have data to read
            for (int i = 0; i < MAX_SOCKETS; i++)
            {

                if (fds[i].revents & POLLIN)
                {  if (listen(fds[i].fd, 5) < 0)
                    {
                        perror("listen");
                        return 1;
                    }
                  
                    int new_socket; // NEW LINE
                    if ((new_socket = accept(fds[i].fd, NULL, NULL)) < 0)
                    {                       // NEW LINE
                        perror("accept");   // NEW LINE
                        exit(EXIT_FAILURE); // NEW LINE
                    }
                    if (i == 0)
                    {
                        IPv4TCP(new_socket);
                    }
                    else if (i == 1)
                    {
                    }
                    else if (i == 2)
                    {
                    }
                    else if (i == 3)
                    {
                    }
                    else if (i == 4)
                    {
                    }
                    else if (i == 5)
                    {
                    }
                }
            }
        }
    }

    return 1;
}
