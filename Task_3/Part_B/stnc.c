#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <poll.h>
#define SIZE 1024 * 1024 * 100
#define PATH "sample.txt"

bool isServer;
bool isClient;
bool isQuite;
bool isTest;

int create_socket(int domain, int type, int protocol, void *addr, socklen_t addrlen)
{
    int s = socket(domain, type, protocol);
    if (s < 0)
    {
        perror("ERROR: faild to create socket");
        exit(1);
    }
    if (bind(s, addr, addrlen) < 0)
    {
        perror("faild to bind socket");
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

int main(int argc, char *argv[])
{
    // this CASE WILL check the input
    // TODO: change to while loop with regeting input from user.
    // if (argc < 4 || argc >6){
    //     printf("Usage:\n"
    //     "client side:\n"
    //      "\tstnc -c IP PORT -p <type> <param>\n"
    //     "\t-p will indicate to perform the test\n"
    //     "\t<type> will be the communication types: so it can be ipv4,ipv6,mmap,pipe,uds\n"
    //     "\t<type> will be the communication types: so it can be ipv4,ipv6,mmap,pipe,uds\n"
    //     "\tavilable methods:\n"
    //     "\t\tIPv4 tcp\n"
    //     "\t\tIPv4 udp\n"
    //     "\t\tIPv6 tcp\n"
    //     "\t\tIPv6 udp\n"
    //     "\t\tUDS dgram\n"
    //     "\t\tUDS stream\n"
    //     "\t\tmmap <file>\n"
    //     "\t\tpipe <file>\n"
    //     "server side:\n"
    //     "\tstnc -s port -p (p for performance test) -q (q for quiet)\n"
    //     "\t-p flag will let you know that we are going to test the performance.\n"
    //     "\t-q flag will enable quiet mode, in which only testing results will be printed."
    //     "\t\tpipe <file>\n"
    //     "\t\tpipe <file>\n");
    //     exit(1);
    // }

    isClient = (*argv[1] == 'c') ? true : false;
    isServer = (*argv[1] == 's') ? true : false;

    if (isClient)
    {
    }
    

// The server side: stnc -s port -p (p for performance test) -q (q for quiet)
// -p flag will let you know that we are going to test the performance.
// -q flag will enable quiet mode, in which only testing results will be printed.
// (This is extremely important for the automatic test.) the results will be in milliseconds (ms) and printed like this name_type,time

    if (isServer)
    {
        int PORT =atoi(argv[2]); // if were in the server we know that the port number should be on  argv[2]
        // Create and bind all the sockets
        struct sockaddr_in addr_in;
        addr_in.sin_family = AF_INET;
        addr_in.sin_port = htons(PORT);
        addr_in.sin_addr.s_addr = htonl(INADDR_ANY);

        struct sockaddr_in6 addr_in6;
        addr_in6.sin6_family = AF_INET6;
        addr_in6.sin6_port = htons(PORT);
        addr_in6.sin6_addr = in6addr_any;

        struct sockaddr_un addr_un;
        addr_un.sun_family = AF_UNIX;
        strcpy(addr_un.sun_path, UNIX_PATH);

        fds[0].fd = create_socket(AF_INET, SOCK_STREAM, 0, &addr_in, sizeof(addr_in));
        fds[1].fd = create_socket(AF_INET, SOCK_DGRAM, 0, &addr_in, sizeof(addr_in));
        fds[2].fd = create_socket(AF_INET6, SOCK_STREAM, 0, &addr_in6, sizeof(addr_in6));
        fds[3].fd = create_socket(AF_INET6, SOCK_DGRAM, 0, &addr_in6, sizeof(addr_in6));
        fds[4].fd = create_socket(AF_UNIX, SOCK_STREAM, 0, &addr_un, sizeof(addr_un));
        fds[5].fd = create_socket(AF_UNIX, SOCK_DGRAM, 0, &addr_un, sizeof(addr_un));

        // Set up the poll parameters for each socket
        for (int i = 0; i < MAX_SOCKETS; i++)
        {
            fds[i].events = POLLIN;
        }

        // Poll the sockets
        while (1)
        {
            int n = poll(fds, MAX_SOCKETS, -1);
            if (n < 0)
            {
                perror("poll");
                exit(1);
            }

            // Handle the sockets that have data to read
            for (int i = 0; i < MAX_SOCKETS; i++)
            {
                if (fds[i].revents & POLLIN)
                {
                    // Handle incoming data on fds[i].fd
                }
            }
        }
    }
}
