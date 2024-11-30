#include <stdbool.h>
#include "soct.h"

bool request(int sock, char *msg, struct sockaddr_in *server, size_t serverLength);
int socketConnect(void);

int main(void) {
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    int sock = socketConnect();

    /* вызов службы  */
    if (request(sock, "Hello", &server, sizeof server)) {
        printf("Сообщение 1 передано\n");
    } else {
        perror("Ошибка передачи сообщения 1");
        exit(1);
    }
    /* вызов службы  */
    if (request(sock, "", &server, sizeof server)) {
        printf("Сообщение 2 передано\n");
    } else {
        perror("Ошибка передачи сообщения 2");
        exit(1);
    }
/* закрытие соединения  */
    close(sock);
    exit(0);
}

int socketConnect(void) {
    int sock;
/* создание сокета  */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Create socket failed");

        return sock;
    }

    return sock;
}

/* функция приема-передачи  */
bool request(int sock, char *msg, struct sockaddr_in *server, size_t serverLength)
{
    size_t messageSize = strlen(msg);
    unsigned int netSize = htonl(messageSize);

    // в отличие от потоковых сокетов, мы должны все сообщение сначала собрать,
    // а потом отправить за один вызов. Иначе сервер потом не разберется от какой строки размер пришел
    size_t bufferSize = sizeof netSize + messageSize;
    void *buffer = malloc(bufferSize);

    unsigned int *netSizeRef = (unsigned int * )buffer;
    char *messageRef = (char *)(buffer + sizeof netSize);

    *netSizeRef = netSize;
    memcpy(messageRef, msg, messageSize);

    if (sendto(sock, buffer, bufferSize, 0, (struct sockaddr *) server, serverLength) < 0) {
        free(buffer);
        return false;
    }

    free(buffer);

    return true;
}                                                             
