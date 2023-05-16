

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
#if !defined(_XOPEN_SOURCE) && !defined(_POSIX_C_SOURCE)
#if _STDC_VERSION_ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif
#endif

// Defines
#define BUFFER_SIZE 1024
#define SIZE 1024 * 1024 * 100
#define CHUNK 65536
#define UDP_CHUNK 1280
#define UDS_NAME "/tmp/uds_sock"

void printUsage()
{

    printf("Part A usage:\n");
    printf("Client: ./stnc -c IP PORT\n");
    printf("Server: ./stnc -s PORT\n");

    printf("Part B Usage:\n"
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

char *checkSumMd5(char *data)
{
    unsigned char *md5_digest = NULL;
    EVP_MD_CTX *mdctx;
    char *checksum = NULL;
    unsigned int mdSize = EVP_MD_size(EVP_md5());
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);
    EVP_DigestUpdate(mdctx, data, SIZE);
    md5_digest = (unsigned char *)OPENSSL_malloc(mdSize);
    EVP_DigestFinal_ex(mdctx, md5_digest, &mdSize);
    EVP_MD_CTX_free(mdctx);

    if (!(checksum = (char *)malloc((mdSize * 2 + 1) * sizeof(char))))
    {
        OPENSSL_free(md5_digest);
        return NULL;
    }

    for (uint32_t i = 0; i < mdSize; i++)
        snprintf(checksum + (i * 2), 3, "%02x", md5_digest[i]);

    OPENSSL_free(md5_digest);
    return checksum;
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

void clientPerformance(const char *ip, const char *port, const char *comunnicationType, const char *param)
{
    struct sockaddr_in server_addr;
    int clientSocket;

    char buffer[BUFFER_SIZE] = {0};

    char *data;
    ;
    char communicationType = -1;

    if (!(data = (char *)malloc(SIZE * sizeof(char))))
    {
        perror("malloc");
        exit(1);
    }
    if (strcmp(comunnicationType, "ipv6") == 0)
        communicationType = (strcmp(param, "tcp") == 0) ? 2 : ((strcmp(param, "udp") == 0) ? 3 : -1);
    else if (strcmp(comunnicationType, "ipv4") == 0)
        communicationType = (strcmp(param, "tcp") == 0) ? 0 : ((strcmp(param, "udp") == 0) ? 1 : -1);
    else if (strcmp(comunnicationType, "uds") == 0)
        communicationType = (strcmp(param, "stream") == 0) ? 4 : ((strcmp(param, "dgram") == 0) ? 5 : -1);
    else if (strcmp(comunnicationType, "mmap") == 0)
        communicationType = 6;
    else if (strcmp(comunnicationType, "pipe") == 0)
        communicationType = 7;

    if (communicationType == -1)
    {
        printf("clientPerformanceERROR: Invalid protocol\n");
        printUsage();
        exit(1);
    }

    srand(time(NULL));

    printf("Generating data...\n");

    for (int i = 0; i < SIZE; i++)
        data[i] = rand() % 256;

    printf("data have been generated \n");
    printf("Calculating MD5 hash...\n");

    char *check = checkSumMd5(data);

    if (check == NULL)
    {
        perror("clientPerformanceERROR: malloc");
        free(data);
        exit(1);
    }

    printf("MD5 checksum hash calculated\n");

    strcpy(buffer, comunnicationType);
    strcat(buffer, " ");
    strcat(buffer, param);
    strcat(buffer, " ");
    strcat(buffer, check);

    free(check);

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("clientPerformanceERROR: socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(clientSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("clientPerformanceERROR: connect");
        exit(1);
    }

    printf("Connected to server\n");
    printf("Sending data...\n");

    if (send(clientSocket, buffer, (strlen(buffer) + 1), 0) < 0)
    {
        perror("send");
        exit(1);
    }

    printf("Data sent\n");
    printf("Receiving data...\n");

    if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0)
    {
        perror("clientPerformanceERROR: recv");
        exit(1);
    }

    printf("Data received\n");

    struct timeval start, end;

    switch (communicationType)
    {
    case 0:
    {
        printf("IPv4 TCP\n");

        struct sockaddr_in server_addr2;
        int clientSocket2;

        clientSocket2 = socket(AF_INET, SOCK_STREAM, 0);

        if (clientSocket2 < 0)
        {
            perror("clientPerformanceERROR: socket");
            exit(1);
        }

        memset(&server_addr2, 0, sizeof(server_addr2));

        server_addr2.sin_family = AF_INET;
        server_addr2.sin_port = htons(atoi(port) + 1);
        server_addr2.sin_addr.s_addr = server_addr.sin_addr.s_addr;

        if (connect(clientSocket2, (struct sockaddr *)&server_addr2, sizeof(server_addr2)) < 0)
        {
            perror("clientPerformanceERROR: connect");
            exit(1);
        }

        printf("Connected to server\n");

        int bytsSented = 0;

        gettimeofday(&start, NULL);

        while (bytsSented < SIZE)
        {
            int chunk_size = (SIZE - bytsSented < CHUNK) ? SIZE - bytsSented : CHUNK;
            int bytes = send(clientSocket2, data + bytsSented, chunk_size, 0);
            gettimeofday(&end, NULL);

            if (bytes < 0)
            {
                perror("clientPerformanceERROR: send");
                exit(1);
            }

            bytsSented += bytes;
        }

        printf("Sent %d bytes\n", bytsSented);

        close(clientSocket2);

        break;
    }

    case 1:
    {
        printf("IPv4 UDP\n");

        struct sockaddr_in server_addr2;
        int clientSocket2;

        if ((clientSocket2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            perror("clientPerformanceERROR: socket");
            exit(1);
        }

        memset(&server_addr2, 0, sizeof(server_addr2));

        server_addr2.sin_family = AF_INET;
        server_addr2.sin_port = htons(atoi(port) + 1);
        server_addr2.sin_addr.s_addr = server_addr.sin_addr.s_addr;

        int bytsSented = 0;

        gettimeofday(&start, NULL);

        while (bytsSented < SIZE)
        {
            int chunk_size = (SIZE - bytsSented < UDP_CHUNK) ? SIZE - bytsSented : UDP_CHUNK;
            int bytes = sendto(clientSocket2, data + bytsSented, chunk_size, 0, (struct sockaddr *)&server_addr2, sizeof(server_addr2));
            gettimeofday(&end, NULL);

            if (bytes < 0)
            {
                perror("clientPerformanceERROR: send");
                exit(1);
            }

            bytsSented += bytes;
        }

        printf("Sent %d bytes\n", bytsSented);

        close(clientSocket2);

        break;
    }

    case 2:
    {
        printf("IPv6 TCP\n");

        struct sockaddr_in6 server_addr2;
        int clientSocket2;

        if ((clientSocket2 = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
        {
            perror("clientPerformanceERROR: socket");
            exit(1);
        }

        memset(&server_addr2, 0, sizeof(server_addr2));

        server_addr2.sin6_family = AF_INET6;
        server_addr2.sin6_port = htons(atoi(port) + 1);

        if (inet_pton(AF_INET6, "::1", &server_addr2.sin6_addr) < 0)
        {
            perror("clientPerformanceERROR: inet_pton");
            exit(1);
        }

        if (connect(clientSocket2, (struct sockaddr *)&server_addr2, sizeof(server_addr2)) < 0)
        {
            perror("clientPerformanceERROR: connect");
            exit(1);
        }

        printf("Connected to server\n");

        int bytesSented = 0;

        gettimeofday(&start, NULL);

        while (bytesSented < SIZE)
        {
            int chunk_size = (SIZE - bytesSented < CHUNK) ? SIZE - bytesSented
                                                          : CHUNK;
            int bytes = send(clientSocket2, data + bytesSented, chunk_size, 0);
            gettimeofday(&end, NULL);

            if (bytes < 0)
            {
                perror("clientPerformanceERROR: send");
                exit(1);
            }

            bytesSented += bytes;
        }

        printf("Sent %d bytes\n", bytesSented);

        close(clientSocket2);

        break;
    }

    case 3:
    {
        printf("IPv6 UDP\n");

        struct sockaddr_in6 server_addr2;
        int clientSocket2;

        if ((clientSocket2 = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        {
            perror("clientPerformanceERROR: socket");
            exit(1);
        }

        memset(&server_addr2, 0, sizeof(server_addr2));

        server_addr2.sin6_family = AF_INET6;
        server_addr2.sin6_port = htons(atoi(port) + 1);

        if (inet_pton(AF_INET6, "::1", &server_addr2.sin6_addr) < 0)
        {
            perror("clientPerformanceERROR: inet_pton");
            exit(1);
        }

        int bytesSented = 0;

        gettimeofday(&start, NULL);

        while (bytesSented < SIZE)
        {
            int chunk_size = (SIZE - bytesSented < UDP_CHUNK) ? SIZE - bytesSented
                                                              : UDP_CHUNK;
            int bytes = sendto(clientSocket2, data + bytesSented, chunk_size, 0, (struct sockaddr *)&server_addr2, sizeof(server_addr2));
            gettimeofday(&end, NULL);

            if (bytes < 0)
            {
                perror("clientPerformanceERROR: send");
                exit(1);
            }

            bytesSented += bytes;
        }

        printf("Sent %d bytes\n", bytesSented);

        close(clientSocket2);

        break;
    }

    case 4:
    {
        printf("UDS Stream\n");

        struct sockaddr_un server_addr2;
        int clientSocket2;

        if ((clientSocket2 = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        {
            perror("clientPerformanceERROR: socket");
            exit(1);
        }

        memset(&server_addr2, 0, sizeof(server_addr2));

        server_addr2.sun_family = AF_UNIX;
        strcpy(server_addr2.sun_path, UDS_NAME);

        if (connect(clientSocket2, (struct sockaddr *)&server_addr2, sizeof(server_addr2)) < 0)
        {
            perror("clientPerformanceERROR: connect");
            exit(1);
        }

        printf("Connected to server\n");

        int bytesSented = 0;

        gettimeofday(&start, NULL);

        while (bytesSented < SIZE)
        {
            int chunk_size = (SIZE - bytesSented < CHUNK) ? SIZE - bytesSented
                                                          : CHUNK;
            int bytes = send(clientSocket2, data + bytesSented, chunk_size, 0);
            gettimeofday(&end, NULL);

            if (bytes < 0)
            {
                perror("clientPerformanceERROR: send");
                exit(1);
            }

            bytesSented += bytes;
        }

        printf("Sent %d bytes\n", bytesSented);

        close(clientSocket2);

        break;
    }

    case 5:
    {
        printf("UDS Datagram\n");

        struct sockaddr_un server_addr2;
        int clientSocket2;

        if ((clientSocket2 = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
        {
            perror("clientPerformanceERROR: socket");
            exit(1);
        }

        memset(&server_addr2, 0, sizeof(server_addr2));

        server_addr2.sun_family = AF_UNIX;
        strcpy(server_addr2.sun_path, UDS_NAME);

        int bytesSented = 0;

        gettimeofday(&start, NULL);

        while (bytesSented < SIZE)
        {
            int chunk_size = (SIZE - bytesSented < CHUNK) ? SIZE - bytesSented : CHUNK;
            int bytes = sendto(clientSocket2, data + bytesSented, chunk_size, 0, (struct sockaddr *)&server_addr2, sizeof(server_addr2));
            gettimeofday(&end, NULL);

            if (bytes < 0)
            {
                perror("clientPerformanceERROR: send");
                exit(1);
            }

            bytesSented += bytes;
        }

        printf("Sent %d bytes\n", bytesSented);

        close(clientSocket2);

        break;
    }

    case 6:
    {
        printf("MMAP\n");

        char *ptr = MAP_FAILED;

        int fd = open(param, O_RDWR | O_CREAT, 0666);

        if (fd < 0)
        {
            perror("clientPerformanceERROR: open");
            exit(1);
        }

        ftruncate(fd, SIZE);

        ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if (ptr == MAP_FAILED)
        {
            perror("clientPerformanceERROR: mmap");
            exit(1);
        }

        send(clientSocket, &fd, sizeof(int), 0);

        gettimeofday(&start, NULL);
        memcpy(ptr, data, SIZE);
        gettimeofday(&end, NULL);

        munmap(ptr, SIZE);
        close(fd);
        break;
    }

    case 7:
    {
        unlink(param);
        printf("FIFO\n");

        if (mkfifo(param, 0666) == -1)
        {
            perror("clientPerformanceERROR: mkfifo");
            exit(1);
        }

        int fd = open(param, O_WRONLY, 0666);

        if (fd < 0)
        {
            perror("clientPerformanceERROR: open");
            exit(1);
        }

        send(clientSocket, &fd, sizeof(int), 0);

        int bytesSented = 0;

        gettimeofday(&start, NULL);
        while (bytesSented < SIZE)
        {
            int chunk_size = (SIZE - bytesSented < CHUNK) ? SIZE - bytesSented
                                                          : CHUNK;
            int bytes = write(fd, data + bytesSented, chunk_size);
            gettimeofday(&end, NULL);

            bytesSented += bytes;
        }

        gettimeofday(&end, NULL);
        close(fd);

        break;
    }

    default:
        break;
    }

    free(data);

    float time_taken = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;

    printf("Elpsed time: %f ms\n", time_taken);
    printf("bandwidth: %f MB/s\n", (SIZE / time_taken * 1000.0 / 1024.0 / 1024.0));

    close(clientSocket);
}

void generateFile()
{
    FILE *file;
    file = fopen("sample.txt", "w");
    if (fseek(file, SIZE, SEEK_SET))
        perror("clientPerformanceERROR: ERROR:");
    fputs("this is the end of the file", file);
    fclose(file);
}

void serverPerformance(const char *port, bool quiet)
{
    struct sockaddr_in server_addr, client_addr;

    int clientSocket, bytes_received = 0, reuse = 1;
    int clientlen = sizeof(client_addr);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        perror("socket");
        exit(1);
    }
    memset(&client_addr, 0, sizeof(client_addr));
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    char *data = malloc(SIZE * sizeof(char));

    if (data == NULL)
    {
        perror("malloc");
        exit(1);
    }

    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
    {
        perror("clientPerformanceERROR: ERROR:");
        exit(1);
    }

    if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("clientPerformanceERROR: ERROR:");
        exit(1);
    }

    if (listen(serverSocket, 1) < 0)
    {
        perror("clientPerformanceERROR: ERROR:");
        exit(1);
    }

    if (!quiet)
        printf("Listening on port %s\n", port);

    while (1)
    {
        clientSocket = accept(serverSocket, (struct sockaddr *)&client_addr, (socklen_t *)&clientlen);

        if (clientSocket < 0)
        {
            perror("accept");
            exit(1);
        }

        if (!quiet)
            printf("connection esteblish\n");

        char buffer[256];

        bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytes_received < 0)
        {
            perror("ERROR:faild to receive data");
            exit(1);
        }

        if (!quiet)
            printf("%s\n", buffer);

        char type[128], param[128], param3[128];
        char *token = strtok(buffer, " ");

        strcpy(type, token);
        token = strtok(NULL, " ");
        strcpy(param, token);
        token = strtok(NULL, " ");
        strcpy(param3, token);

        int flag = -1;
        if (strcmp(type, "ipv4") == 0)
        {
            if (strcmp(param, "tcp") == 0)
                flag = 0;
            else if (strcmp(param, "udp") == 0)
                flag = 1;
        }
        else if (strcmp(type, "ipv6") == 0)
        {
            if (strcmp(param, "tcp") == 0)
                flag = 2;
            else if (strcmp(param, "udp") == 0)
                flag = 3;
        }
        else if (strcmp(type, "uds") == 0)
        {
            if (strcmp(param, "stream") == 0)
                flag = 4;
            else if (strcmp(param, "dgram") == 0)
                flag = 5;
        }
        else if (strcmp(type, "mmap") == 0)
            flag = 6;
        else if (strcmp(type, "pipe") == 0)
            flag = 7;

        if (flag == -1)
        {
            printf("Invalid protocol\n");
            close(clientSocket);
            continue;
        }

        struct timeval start, end;

        int bytes_received2 = 0;

        switch (flag)
        {
        case 0:
        {
            struct sockaddr_in server_addr2, client_addr2;
            int clientSocket2;
            int clientlen2 = sizeof(client_addr2);
            int serverSocket2 = socket(AF_INET, SOCK_STREAM, 0);

            if (serverSocket2 < 0)
            {
                perror("ERROR:faild to create socket");
                exit(1);
            }

            memset(&server_addr2, 0, sizeof(server_addr2));
            memset(&client_addr2, 0, sizeof(client_addr2));

            server_addr2.sin_family = AF_INET;
            server_addr2.sin_port = htons(atoi(port) + 1);
            server_addr2.sin_addr.s_addr = INADDR_ANY;

            if (setsockopt(serverSocket2, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
            {
                perror("setsockopt");
                exit(1);
            }

            if (bind(serverSocket2, (struct sockaddr *)&server_addr2, sizeof(server_addr2)) < 0)
            {
                perror("bind clientPerformanceERROR: ERROR:");
                exit(1);
            }

            if (listen(serverSocket2, 1) < 0)
            {
                perror("listen clientPerformanceERROR: ERROR:");
                exit(1);
            }

            send(clientSocket, "ACK", 3, 0);


            if (clientSocket2 = accept(serverSocket2, (struct sockaddr *)&client_addr2, (socklen_t *)&clientlen2)< 0)
            {
                perror("accept");
                exit(1);
            }

            close(serverSocket2);

            if (!quiet)
                printf("Accepted Estblishe\n");

            gettimeofday(&start, NULL);

            while (bytes_received2 < SIZE)
            {
                int chunk_size = SIZE - bytes_received2 < CHUNK ? SIZE - bytes_received2 : CHUNK;

                int bytes = recv(clientSocket2, data + bytes_received2, chunk_size, 0);
                gettimeofday(&end, NULL);

                if (bytes < 0)
                {
                    perror("recv clientPerformanceERROR: ERROR:");
                    exit(1);
                }

                bytes_received2 += bytes;
            }

            close(clientSocket2);

            break;
        }

        case 1:
        {
            struct sockaddr_in server_addr2, client_addr2;
            int clientlen2 = sizeof(client_addr2);

            int serverSocket2 = socket(AF_INET, SOCK_DGRAM, 0);

            if (serverSocket2 < 0)
            {
                perror("socket filior ");
                exit(1);
            }

            memset(&server_addr2, 0, sizeof(server_addr2));
            memset(&client_addr2, 0, sizeof(client_addr2));

            server_addr2.sin_family = AF_INET;
            server_addr2.sin_port = htons(atoi(port) + 1);
            server_addr2.sin_addr.s_addr = INADDR_ANY;

            if (setsockopt(serverSocket2, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
            {
                perror("setsockopt clientPerformanceERROR:");
                exit(1);
            }

            if (bind(serverSocket2, (struct sockaddr *)&server_addr2, sizeof(server_addr2)) < 0)
            {
                perror("bind clientPerformanceERROR:");
                exit(1);
            }

            if (!quiet)
                printf("Waiting for the client to send data\n");

            struct pollfd fds[1];

            fds[0].fd = serverSocket2;
            fds[0].events = POLLIN;

            send(clientSocket, "ACK", 3, 0);

            gettimeofday(&start, NULL);

            while (bytes_received2 < SIZE)
            {
                int ret = poll(fds, 1, 1000);

                if (ret < 0)
                {
                    perror("poll");
                    exit(1);
                }

                else if (ret == 0)
                {
                    if (!quiet)
                        printf("Timeout\n");

                    break;
                }

                if (fds[0].revents & POLLIN)
                {
                    int chunk_size = SIZE - bytes_received2 < UDP_CHUNK ? SIZE - bytes_received2 : UDP_CHUNK;

                    int bytes = recvfrom(serverSocket2, data + bytes_received2, chunk_size, 0, (struct sockaddr *)&client_addr2, (socklen_t *)&clientlen2);
                    gettimeofday(&end, NULL);

                    if (bytes < 0)
                    {
                        perror("recvfrom clientPerformanceERROR:");
                        exit(1);
                    }

                    bytes_received2 += bytes;
                }
            }

            close(serverSocket2);

            break;
        }

        case 2:
        {
            struct sockaddr_in6 server_addr2, client_addr2;
            int clientSocket2;
            int clientlen2 = sizeof(client_addr2);
            int serverSocket2 = socket(AF_INET6, SOCK_STREAM, 0);

            if (serverSocket2 < 0)
            {
                perror("socket clientPerformanceERROR:");
                exit(1);
            }

            memset(&server_addr2, 0, sizeof(server_addr2));
            memset(&client_addr2, 0, sizeof(client_addr2));

            server_addr2.sin6_family = AF_INET6;
            server_addr2.sin6_port = htons(atoi(port) + 1);
            server_addr2.sin6_addr = in6addr_any;

            if (setsockopt(serverSocket2, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
            {
                perror("setsockopt clientPerformanceERROR:");
                exit(1);
            }

            if (bind(serverSocket2, (struct sockaddr *)&server_addr2, sizeof(server_addr2)) < 0)
            {
                perror("bind clientPerformanceERROR:");
                exit(1);
            }

            if (listen(serverSocket2, 1) < 0)
            {
                perror("listen clientPerformanceERROR:");
                exit(1);
            }

            send(clientSocket, "ACK", 3, 0);


            if (clientSocket2 = accept(serverSocket2, (struct sockaddr *)&client_addr2, (socklen_t *)&clientlen2) < 0)
            {
                perror("accept clientPerformanceERROR:");
                exit(1);
            }

            close(serverSocket2);

            if (!quiet)
                printf("Accepted esteblished\n");

            gettimeofday(&start, NULL);

            while (bytes_received2 < SIZE)
            {
                int chunk_size = SIZE - bytes_received2 < CHUNK ? SIZE - bytes_received2 : CHUNK;

                int bytes = recv(clientSocket2, data + bytes_received2, chunk_size, 0);
                gettimeofday(&end, NULL);

                if (bytes < 0)
                {
                    perror("recv clientPerformanceERROR:");
                    exit(1);
                }

                bytes_received2 += bytes;
            }

            close(clientSocket2);

            break;
        }

        case 3:
        {
            struct sockaddr_in6 server_addr2, client_addr2;
            int clientlen2 = sizeof(client_addr2);

            int serverSocket2 = socket(AF_INET6, SOCK_DGRAM, 0);

            if (serverSocket2 < 0)
            {
                perror("socket clientPerformanceERROR:");
                exit(1);
            }

            memset(&server_addr2, 0, sizeof(server_addr2));
            memset(&client_addr2, 0, sizeof(client_addr2));

            server_addr2.sin6_family = AF_INET6;
            server_addr2.sin6_port = htons(atoi(port) + 1);
            server_addr2.sin6_addr = in6addr_any;

            if (setsockopt(serverSocket2, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
            {
                perror("setsockopt clientPerformanceERROR:");
                exit(1);
            }

            if (bind(serverSocket2, (struct sockaddr *)&server_addr2, sizeof(server_addr2)) < 0)
            {
                perror("bind clientPerformanceERROR:");
                exit(1);
            }

            if (!quiet)
                printf("Waiting for data \n");

            struct pollfd fds[1];

            fds[0].fd = serverSocket2;
            fds[0].events = POLLIN;

            send(clientSocket, "ACK", 3, 0);

            gettimeofday(&start, NULL);

            while (bytes_received2 < SIZE)
            {
                int ret = poll(fds, 1, 1000);

                if (ret < 0)
                {
                    perror("poll");
                    exit(1);
                }

                else if (ret == 0)
                {
                    if (!quiet)
                        printf("Timeout\n");

                    break;
                }

                if (fds[0].revents & POLLIN)
                {
                    int chunk_size = SIZE - bytes_received2 < UDP_CHUNK ? SIZE - bytes_received2 : UDP_CHUNK;

                    int bytes = recvfrom(serverSocket2, data + bytes_received2, chunk_size, 0, (struct sockaddr *)&client_addr2, (socklen_t *)&clientlen2);
                    gettimeofday(&end, NULL);

                    if (bytes < 0)
                    {
                        perror("recvfrom error");
                        exit(1);
                    }

                    bytes_received2 += bytes;
                }
            }

            close(serverSocket2);

            break;
        }

        case 4:
        {
            struct sockaddr_un server_addr2, client_addr2;
            int clientSocket2;

            int serverSocket2 = socket(AF_UNIX, SOCK_STREAM, 0);

            if (serverSocket2 < 0)
            {
                perror("socket error");
                exit(1);
            }

            memset(&server_addr2, 0, sizeof(server_addr2));
            memset(&client_addr2, 0, sizeof(client_addr2));

            server_addr2.sun_family = AF_UNIX;
            strcpy(server_addr2.sun_path, UDS_NAME);
            unlink(UDS_NAME);

            int len = sizeof(server_addr2.sun_family) + strlen(server_addr2.sun_path);

            if (bind(serverSocket2, (struct sockaddr *)&server_addr2, len) < 0)
            {
                perror("bind error");
                exit(1);
            }

            if (listen(serverSocket2, 1) < 0)
            {
                perror("listen error");
                exit(1);
            }

            send(clientSocket, "ACK", 3, 0);

            clientSocket2 = accept(serverSocket2, (struct sockaddr *)&client_addr2, (socklen_t *)&len);

            if (clientSocket2 < 0)
            {
                perror("accept error");
                exit(1);
            }

            close(serverSocket2);

            if (!quiet)
                printf("Accepted connection\n");

            gettimeofday(&start, NULL);

            while (bytes_received2 < SIZE)
            {
                int chunk_size = SIZE - bytes_received2 < CHUNK ? SIZE - bytes_received2 : CHUNK;

                int bytes = recv(clientSocket2, data + bytes_received2, chunk_size, 0);
                gettimeofday(&end, NULL);

                if (bytes < 0)
                {
                    perror("recv error");
                    exit(1);
                }

                bytes_received2 += bytes;
            }

            close(clientSocket2);

            break;
        }

        case 5:
        {
            struct sockaddr_un server_addr2, client_addr2;

            int serverSocket2 = socket(AF_UNIX, SOCK_DGRAM, 0);

            if (serverSocket2 < 0)
            {
                perror("socket error");
                exit(1);
            }

            memset(&server_addr2, 0, sizeof(server_addr2));
            memset(&client_addr2, 0, sizeof(client_addr2));

            server_addr2.sun_family = AF_UNIX;
            strcpy(server_addr2.sun_path, UDS_NAME);
            unlink(UDS_NAME);

            int len = sizeof(server_addr2.sun_family) + strlen(server_addr2.sun_path);
            int len2 = 0;

            if (bind(serverSocket2, (struct sockaddr *)&server_addr2, len) < 0)
            {
                perror("bind error");
                exit(1);
            }

            send(clientSocket, "ACK", 3, 0);

            struct pollfd fds[1];

            fds[0].fd = serverSocket2;
            fds[0].events = POLLIN;

            gettimeofday(&start, NULL);

            while (bytes_received2 < SIZE)
            {
                int ret = poll(fds, 1, 1000);

                if (ret < 0)
                {
                    perror("poll error");
                    exit(1);
                }

                else if (ret == 0)
                {
                    if (!quiet)
                        printf("Timeout\n");

                    break;
                }

                if (fds[0].revents & POLLIN)
                {
                    int chunk_size = SIZE - bytes_received2 < CHUNK ? SIZE - bytes_received2 : CHUNK;

                    int bytes = recvfrom(serverSocket2, data + bytes_received2, chunk_size, 0, (struct sockaddr *)&client_addr2, (socklen_t *)&len2);
                    gettimeofday(&end, NULL);

                    if (bytes < 0)
                    {
                        perror("recvfrom error");
                        exit(1);
                    }

                    bytes_received2 += bytes;
                }
            }

            close(serverSocket2);

            break;
        }

        case 6:
        {
            int tmp = 0;
            send(clientSocket, "ACK", 3, 0);
            recv(clientSocket, &tmp, sizeof(tmp), 0);

            sleep(1);

            int fd = open(param, O_RDONLY);
            if (fd < 0)
            {
                perror("open field");
                exit(1);
            }

            char *mmap_data = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, fd, 0);

            if (mmap_data == MAP_FAILED)
            {
                perror("mmap error");
                exit(1);
            }

            gettimeofday(&start, NULL);
            memcpy(data, mmap_data, SIZE);
            gettimeofday(&end, NULL);

            munmap(mmap_data, SIZE);

            close(fd);

            unlink(param);

            bytes_received2 = SIZE;

            break;
        }

        case 7:
        {
            int tmp = 0;
            send(clientSocket, "ACK", 3, 0);
            sleep(1);

            printf("Waiting for file...\n");

            int fd = open(param, O_RDONLY);

            if (fd < 0)
            {
                perror("open filed");
                exit(1);
            }

            gettimeofday(&start, NULL);

            while (bytes_received2 < SIZE)
            {
                int chunk_size = SIZE - bytes_received2 < CHUNK ? SIZE - bytes_received2 : CHUNK;

                int bytes = read(fd, data + bytes_received2, chunk_size);
                gettimeofday(&end, NULL);

                if (bytes < 0)
                {
                    perror("file to read");
                    exit(1);
                }

                bytes_received2 += bytes;
            }

            close(fd);

            unlink(param);

            break;
        }

        default:
            break;
        }

        float time_taken = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;

        char *md5sum = checkSumMd5(data);

        if (!quiet)
        {
            printf(" bytes Received: %d\n", bytes_received2);
            printf("Time taken: %f ms\n", time_taken);
            printf("bandwidth: %f MB/s\n", (bytes_received2 / time_taken) * 1000.0 / (1024.0 * 1024.0));

            if (strcmp(md5sum, param3) != 0)
            {
                printf("ERROR: input data is corupted\n");
            }

            else
            {
                printf("MD5 OK\n");
            }
        }

        else
        {
            if (strcmp(md5sum, param3) == 0)
            {
                if (flag < 6)
                {
                    printf("%s_%s,%d\n", type, param, (int)time_taken);
                }

                else
                {
                    printf("%s,%d\n", type, (int)time_taken);
                }
            }

            else
            {
                printf("ERROR: input data is corupted: %d/%d bytes (%0.2f%%), ", bytes_received2, SIZE, (float)bytes_received2 / SIZE * 100.0);

                if (flag < 6)
                {
                    printf("%s_%s,%d\n", type, param, (int)time_taken);
                }

                else
                {
                    printf("%s,%d\n", type, (int)time_taken);
                }
            }
        }

        free(md5sum);

        close(clientSocket);
    }

    close(serverSocket);
}

int main(int argc, char *argv[])
{
    bool isClient = (strncmp(argv[1], "-c", 2) == 0);
    bool isServer = (strncmp(argv[1], "-s", 2) == 0);

    if (argc < 2)
    {
        printUsage(argv[0]);
        return 1;
    }
    else if (isClient)
    {
        if (argc < 4)
        {
            printUsage(argv[0]);
            return 1;
        }

        switch (argc)
        {
        case 4:
            clientChat(argv[2], argv[3]);
            break;

        case 7:
            if (strncmp(argv[4], "-p", 2) == 0)
            {
                clientPerformance(argv[2], argv[3], argv[5], argv[6]);
                break;
            }

            printUsage(argv[0]);
            return 1;

        default:
            printUsage(argv[0]);
            return 1;
        }
    }
    else if (isServer)
    {
        if (argc < 3)
        {
            printUsage(argv[0]);
            return 1;
        }

        switch (argc)
        {
        case 3:
            serverChat(argv[2]);
            break;

        case 4:
            if (strncmp(argv[3], "-p", 2) == 0)
            {
                serverPerformance(argv[2], false);
                break;
            }

            printUsage(argv[0]);
            return 1;

        case 5:
            if (strncmp(argv[3], "-p", 2) == 0 && strncmp(argv[4], "-q", 2) == 0)
            {
                serverPerformance(argv[2], true);
                break;
            }

            printUsage(argv[0]);
            return 1;

        default:
            printUsage(argv[0]);
            return 1;
        }
    }
    else
    {
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}