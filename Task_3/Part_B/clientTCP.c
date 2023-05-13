
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>


// DEFINES
#define PATH "sample.txt"
#define WIND 1024

int sendIPv4(int PORT, char *IP)
{
    
    int tcpSocket;
    struct sockaddr_in serverAdrres;
    char buffer[WIND];
    int file;
    ssize_t readFromFile;

    if ((tcpSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("sendIPv4: faild to connect socket");
        return -1;
    }
    sif (inet_pton(AF_INET, IP, &serverAdrres.sin_addr) <= 0) {
    perror("sendIPv4:inet_pton general failure");
    return -1;
}
   

    serverAdrres.sin_family = AF_INET;
    serverAdrres.sin_port = htons(PORT);
    printf("before connect\n");
    if (connect(tcpSocket, (struct sockaddr *)&serverAdrres, sizeof(serverAdrres)) < 0)
    {
        printf("this is the connect function\n");
        perror("sendIPv4:connect");
        return -1;
    }
    printf("sendIPv4:connection estblish!!!");

  

    if ((file = open(PATH, O_RDONLY)) == -1)
    {
        perror("sendIPv4:faild to open file ");
        return-1;
    }
    printf("opened file");
     while ((readFromFile = read(file, buffer, WIND)) > 0)
    {
        if (send(tcpSocket, buffer, readFromFile, 0) < 0) 
        {
            perror("send");
            return -1;
        }
        
    }

    return 1;
}
int sendIPv6(int PORT, char *IP){
    int tcpSocket;
    struct sockaddr_in6 serverAdrres;
    char buffer[WIND];
    int file;
    ssize_t readFromFile;

    if((tcpSocket = socket(AF_INET6, SOCK_STREAM, 0))==-1){
        perror("faild to connect tcp socket");
        return -1;
    }

    serverAdrres.sin6_family = AF_INET6;
    if (inet_pton(AF_INET6, IP, &serverAdrres.sin6_addr)<=0)
    {
        perror("sendIPv6:inet_pton general failure");
        return -1;
    }
  

    serverAdrres.sin6_port = htons(PORT);

    printf("Connecting to %s:%d\n", IP, ntohs(serverAdrres.sin6_port));

    if (connect(tcpSocket, (struct sockaddr *)&serverAdrres, sizeof(serverAdrres))<0)
    {
        perror("sendIPv6:connect");
        return 1;
    }

    printf("Connection established\n");

    file = open(PATH, O_RDONLY);
    if (file == -1)
    {
        perror("open");
        return 1;
    }

    while ((readFromFile = read(file, buffer, WIND)) > 0)
    {
        if (write(tcpSocket, buffer, readFromFile) < 0)
        {
            perror("write");
            return 1;
        }
    }

    close(file);
    close(tcpSocket);

    return 0;
}