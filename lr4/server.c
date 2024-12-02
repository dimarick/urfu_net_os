/*файл server.c                      ***************/
/*пpоцесс-сеpвеp   */
#include <stdbool.h>
#include <errno.h>
#include "soct.h"
#include "sockets.c"

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
        int nsock = accept(sock, (struct sockaddr *)&client, &sizeOfClient);
        if (nsock < 0) {
            perror("Failed to accept");
            exit(1);
        }

        if (inet_ntop(client.sin_family, &client.sin_addr, ip, sizeof ip) == 0) {
            perror("Failed to parse client address");
            continue;
        }

        int port = ntohs(client.sin_port);
        printf("Accepted client %s:%d\n", ip, port);

/* обращение к циклу чтение-запись */
        if (!serve(nsock)) {
            perror("Failed to serve");
        }
/* закрытие текущей связи */
        close(nsock);
    }
}

/* функция, реализующая специальную обработку IPC */
int serverBind(void) {
    int sock;                    /* дескриптор сокета */
    struct sockaddr_in server;   /* адрес сервера */
/* создание сокета  */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
/* переход к прослушиванию приходящих связей  */
    if (listen(sock, 5) < 0) {
        perror("Cannot listen socket");

        return -1;
    }

    return sock;
}

/*функция пpиема-пеpедачи           */
/* Выводит все строки, переданные клиентом */
bool serve(int sock)     /* дескриптор сокета  */
{
    unsigned int lbufres;          /* целое число в формате сети  */
    int lbuf;
    char buf[BUFFER_SIZE + 1];
    ssize_t ret;
    for (;;) {
        if ((ret = reads(sock, &lbufres, sizeof lbufres)) < 0) {
            perror("Failed to read");
            return false;
        }

        // сокет закрыт удаленной стороной
        if (ret == 0) {
            break;
        }

        lbuf = ntohs(lbufres);
        if (lbuf > BUFFER_SIZE) {
            // Нельзя ронять сервер длинной строкой, завершим обработку только текущего запроса
            errno = EOVERFLOW;
            printf("Server: request too large: %d, but limit is %d\n", lbuf, BUFFER_SIZE);
            return false;
        }

        // пустая строка ничем не хуже любой другой
        if (lbuf == 0) {
            printf("Server: message=''\n");
            break;
        }

        // если строка не пустая, то читаем ее
        if ((ret = reads(sock, &buf, lbuf)) < 0) {
            perror("Failed to read");
            return false;
        }

        // Дописывание null-байта теперь тут. Используем ret, а не lbuf,
        // пользователь мог отослать меньше данных, чем заявил
        buf[ret] = '\0';

        printf("Server: message='%s'\n", buf);
    }

    return true;
}
