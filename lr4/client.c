/* Функция "эхо" для сокета TCP в режиме виртуального соедине-
   ния  */

/*файл client.c          ****************************/
#include <stdbool.h>
#include "soct.h"
/*пpоцесс-клиент*/
#include "sockets.c"

bool request(int, char *string);
int socketConnect(void);

int main(void) {
/*вызов функции, выполняющей специальную обpаботку IPC */
    int sock = socketConnect();

    if (sock < 0) {
        perror("Cannot connect");
        exit(1);
    }

    /* вызов службы  */
    if (request(sock, "Hello")) {
        printf("Сообщение 1 передано\n");
    } else {
        perror("Ошибка передачи сообщения 1");
        exit(1);
    }
    /* вызов службы  */
    if (request(sock, "")) {
        printf("Сообщение 2 передано\n");
    } else {
        perror("Ошибка передачи сообщения 2");
        exit(1);
    }
/* закрытие соединения  */
    close(sock);
    exit(0);
}

/*функция, pеализующая специальную обpаботку IPC   */
int socketConnect(void) {
    int sock;
    struct sockaddr_in server;
    unsigned long addr_s;
/* создание сокета  */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Create socket failed");

        return sock;
    }

    printf("Создан сокет, sock=%d\n", sock);
/* подсоединение к серверу */
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    addr_s = inet_addr("127.0.0.1");
    server.sin_addr.s_addr = addr_s;
    server.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *) &(server), sizeof(server)) < 0) {
        perror("Connect failed");

        return -1;
    }

    printf("connected to server %s:%d\n", "127.0.0.1", PORT);

    return sock;
}

/* функция приема-передачи  */
bool request(int sock, char *msg)
{
    size_t bufferSize;
    unsigned int netSize;          /* целое число в формате сети  */
    /* начнем с пересылки размера буфера; обратите внимание на
      то, что число надо передать в сетевом формате  */
    bufferSize = strlen(msg); // +1 можно не делать, 0-байт не передавать, если мы и так передали длину строки.
    netSize = htons(bufferSize);
    if (writes(sock, &netSize, sizeof netSize) < 0) {
        return false;
    }

    if (writes(sock, msg, bufferSize) < 0) {
        return false;
    }

    return true;
}                                                             
