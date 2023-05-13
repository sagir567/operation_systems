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

#define PATH "sample.txt"
#define WIND 1024

#define PATH "sample.txt

int client_ipv4udp(int PORT, char *IP)
{
    int udpsocket;
    struct sockaddr_in serverAdress;
    // char response[1024];
    char buffer[1024];
    int file;
    ssize_t reade;

    if (udpsocket = socket(AF_INET, SOCK_DGRAM, 0) == -1)
    {
        perror("faild to connect socket");
        return -1;
    }

    serverAdress.sin_addr.s_addr = inet_addr(IP);
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(PORT);

    printf("Send file to %s:%d\n", inet_ntoa(serverAdress.sin_addr), ntohs(serverAdress.sin_port));

    file = open(PATH, O_RDONLY);
    if (file == -1)
    {
        y = AF_INET;
        serverAdress.sin_port = htons(PORT);
    }

    while ((read = read(file, buffer, BUFFER_SIZE)) > 0)
    {
        if (sendto(udpsocket, buffer, read, 0, (struct sockaddr *)&serverAdress, sizeof(serverAdress)) < 0)
        {
            perror("sendto");
            return 1;
        }
    }

    close(file);
    close(udpsocket);

    return 0;
}

int ipv6_udp_client(int PORT, char *IP)
{
    int udpsocket;
    struct sockaddr_in6 serverAdress;
    // char response[BUFFER_SIZE];
    char buffer[1024];
    int file;
    ssize_t read;

    if (udpsocket = socket(AF_INET6, SOCK_DGRAM, 0) == -1)
    {
        perror("faild to connect tcp socket");
        return 1;
    }

    serverAdress.sin6_family = AF_INET6;
    if (inet_pton(AF_INET6, IP, &serverAdress.sin6_addr) == -1)
    {
        perror("inet_pton field to successid");
        return 1;
    }

    serverAdress.sin6_port = htons(PORT);

    printf("Sending file to %s:%d\n", IP, ntohs(serverAdress.sin6_port));

    file = open(PATH, O_RDONLY);
    if (file == -1)
    {
        perror("open");
        return 1;
    }

    while ((read = read(file, buffer, 1024)) > 0)
    {
        if (sendto(udpsocket, buffer, read, 0, (struct sockaddr *)&serverAdress, sizeof(serverAdress)) == -1)
        {
            perror("feil to send");
            return 1;
        }
    }

    close(file);
    close(udpsocket);

    return 0;
}