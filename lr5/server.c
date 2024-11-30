#include <stdbool.h>
#include <errno.h>
#include "soct.h"

#include "string.h"

static const int BUFFER_SIZE = 1024;

int serverBind(void);

bool serve(int);

int main(void) {
/*вызов функции, выполняющей специальную обработку IPC  */
    int sock = serverBind();

    if (sock < 0) {
        perror("Cannot bind");
        exit(1);
    }

    printf("Server is ready for connections\n");

    /* цикл по запросам на связь  */
    struct sockaddr_in client;
    socklen_t sizeOfClient;
    char ip[50];

    for (;;) {
        char buf[BUFFER_SIZE + 1 + sizeof (unsigned int)];
        ssize_t result = recvfrom(sock, buf, sizeof buf - 1, 0, (struct sockaddr *)&client, &sizeOfClient);
        if (result < 0) {
            perror("Failed to recv");
            exit(1);
        }

        if (inet_ntop(client.sin_family, &client.sin_addr, ip, sizeof ip) == 0) {
            perror("Failed to parse client address");
            continue;
        }
        if (result < sizeof (unsigned int)) {
            perror("Client bad request");
            exit(1);
        }

        int port = ntohs(client.sin_port);
        printf("Accepted client %s:%d\n", ip, port);

        unsigned int *messageNetLength = (unsigned int *)&buf;
        unsigned int messageLength = htonl(*messageNetLength);

        if (messageLength >= sizeof buf) {
            errno = EOVERFLOW;
            perror("Failed to process message");
            continue;
        }

        if (messageLength != (result - sizeof (unsigned int))) {
            printf("Inconsistent message length: %d declared by client, %d received", messageLength, (int)result);
            continue;
        }

        char *message = buf + sizeof (unsigned int);
        message[messageLength] = '\0';

        printf("Server: message='%s'\n", message);
    }
}

/* функция, реализующая специальную обработку IPC */
int serverBind(void) {
    int sock;                    /* дескриптор сокета */
    struct sockaddr_in server;   /* адрес сервера */
/* создание сокета  */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Create socket failed");

        return sock;
    }
/* назначение адреса сокету  */
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    if (bind(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Cannot bind to port");

        return -1;
    }

    return sock;
}
