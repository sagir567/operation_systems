#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>


#define BUFFER_SIZE 1024

void clientChat(const char *ip, const char *port)
{
    struct sockaddr_in server_addr;
    int clientSocket;
    char buffer[BUFFER_SIZE];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0)
    {
        perror("clientChat ERROR:socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    if (inet_pton(AF_INET, ip, &(server_addr.sin_addr)) <= 0)
    {
        perror("clientChat ERROR:inet_pton");
        exit(1);
    }

    if (connect(clientSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("clientChat ERROR:connect");
        exit(1);
    }

    struct pollfd fds[2];
    fds[0].fd = clientSocket;
    fds[0].events = POLLIN;

    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;

    while (1)
    {
        int events = poll(fds, 2, -1);

        if (events < 0)
        {
            perror("clientChat ERROR:poll");
            exit(1);
        }

        if (fds[0].revents & POLLIN)
        {
            ssize_t recv_len = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

            if (recv_len < 0)
            {
                perror("clientChat ERROR:recv");
                exit(1);
            }
            else if (recv_len == 0)
            {
                printf("disconnecting from server...!!\n");
                break;
            }

            buffer[recv_len] = '\0';
            printf("Server: %s\n", buffer);
        }

        if (fds[1].revents & POLLIN)
        {
            if (fgets(buffer, sizeof(buffer), stdin) == NULL)
                break;

            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n')
                buffer[len - 1] = '\0';

            if (send(clientSocket, buffer, strlen(buffer), 0) < 0)
            {
                perror("send");
                exit(1);
            }
        }

        if (fds[0].revents & (POLLHUP | POLLERR))
        {
            printf("Server disconnected!!\n");
            break;
        }
    }

    close(clientSocket);
}

void serverChat(const char *port)
{
    struct sockaddr_in server_addr, client_addr;
    int serverSocket, clientSocket, reuse = 1;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("SERVER CHAT ERROR:setsockopt");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("SERVER CHAT ERROR:bind");
        exit(1);
    }

    if (listen(serverSocket, 5) < 0)
    {
        perror("SERVER CHAT ERROR:listen");
        exit(1);
    }

    while (1)
    {

        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&client_addr, &client_len)) < 0)
        {
            perror("SERVER CHAT ERROR:accept");
            exit(1);
        }
        else
        {
            printf("connection esteblished \n");
        }

        struct pollfd fds[2];
        fds[0].fd = clientSocket;
        fds[0].events = POLLIN;

        fds[1].fd = STDIN_FILENO;
        fds[1].events = POLLIN;

        while (1)
        {
            int events;

            if ((events = poll(fds, 2, -1)) < 0)
            {
                perror("SERVER CHAT ERROR:poll");
                exit(1);
            }

            if (fds[0].revents & POLLIN)
            {
                int num_of_bytes;

                if ((num_of_bytes = recv(clientSocket, buffer, sizeof(buffer), 0)) < 0)
                {
                    perror("SERVER CHAT ERROR:recv");
                    exit(1);
                }
                else if (num_of_bytes == 0)
                {
                    printf("Client disconnected\n");
                    break;
                }

                printf("Client: %s\n", buffer);

                if (strcmp(buffer, "exit") == 0)
                {
                    printf("terminating chat...\n");
                    close(clientSocket);
                    close(serverSocket);
                    return;
                }
            }

            if (fds[1].revents & POLLIN)
            {
                fgets(buffer, sizeof(buffer), stdin);

                buffer[strlen(buffer) - 1] = '\0';

                if (send(clientSocket, buffer, strlen(buffer), 0) < 0)
                {
                    perror("SERVER CHAT ERROR:send");
                    exit(1);
                }
            }

            if (fds[0].revents & POLLHUP)
            {
                printf("Connection closed by client\n");
                break;
            }

            memset(buffer, 0, sizeof(buffer));
        }

        close(clientSocket);
    }

    close(serverSocket);
}