#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>

#define THREADS 128

int pipesTask[THREADS][2];
int pipesResult[THREADS][2];
int pids[THREADS];

struct task {
    unsigned long long from;
    unsigned long long to;
};

unsigned long long squareSum(unsigned long long from, unsigned long long to);

int main(int argc, char **argv)
{
    char *endptr;
    int numThreads = argc > 1 ? (int)strtol(argv[1], &endptr, 10) : THREADS;
    unsigned long long to = argc > 2 ? strtoull(argv[2], &endptr, 10) : 10000;
    unsigned long long batchSize = argc > 3 ? strtoull(argv[3], &endptr, 10) : 100;

    if (numThreads > THREADS) {
        numThreads = THREADS;
    }

    if (numThreads == 1) {
        printf("%llu\n", squareSum(0, to - 1));
        exit(0);
    }

    for (int i = 0; i < numThreads; ++i) {
        if ((pipe(pipesTask[i]) < 0)) { //Create pipe
            perror("Error creating pipe");
            exit(1);
        }
        if ((pipe2(pipesResult[i], O_NONBLOCK) < 0)) { //Create pipe
            perror("Error creating pipe2");
            exit(1);
        }

        int pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        }

        if (pid > 0) {    //Parent process
            pids[i] = pid;
            close(pipesTask[i][0]);
            close(pipesResult[i][1]);
        } else {     //Child process
            struct task t = {0, 0};
            int in = dup(pipesTask[i][0]);
            int out = dup(pipesResult[i][1]);
            close(pipesTask[i][1]);
            close(pipesResult[i][0]);

            while (1) {
                size_t bytesRead = read(in, &t, sizeof t);

                if (bytesRead == sizeof t) {

                    if (t.from == 0 && t.to == 0) {
                        break;
                    }

                    printf("Child %d received %llu-%llu\n", i, t.from, t.to);

                    unsigned long long result = squareSum(t.from, t.to);

                    size_t bytes = write(out, &result, sizeof result);
                    if (bytes != sizeof result) {
                        printf("Bytes written %zu, but expected %zu in child loop\n", bytes, sizeof t);
                        perror("Write failed");
                        exit(2);
                    }
                } else if (bytesRead == 0) {
                    continue;
                } else {
                    printf("Bytes read %zu, but expected %zu in init loop. Thread %d\n", bytesRead, sizeof t, i);
                    perror("Read failed");
                    exit(3);
                }

            }

            unsigned long long result = -1;
            size_t bytes = write(out, &result, sizeof result);
            if (bytes != sizeof result) {
                printf("Bytes written %zu, but expected %zu at child exit. Thread %d\n", bytes, sizeof t, i);
                perror("Write failed");
                exit(2);
            }

            close(pipesTask[i][0]);
            close(pipesResult[i][1]);

            exit(0);
        }
    }

    unsigned long long result = 0;

    for (int i = 0; i < to / batchSize; ++i) {
        int thread = i % numThreads;
        struct task t = {i * batchSize, (i + 1) * batchSize - 1};
        size_t bytes = write(pipesTask[thread][1], &t, sizeof t);

        if (bytes != sizeof t) {
            printf("Bytes written %zu, but expected %zu in init loop\n", bytes, sizeof t);
            perror("Write failed");
            exit(2);
        }

        unsigned long long tmpResult = 0;
        bytes = read(pipesResult[thread][0], &tmpResult, sizeof tmpResult);

        if (bytes == sizeof tmpResult) {
            result += tmpResult;
            printf("Received %llu, total %llu\n", tmpResult, result);
        } else if (bytes == 0) {
            continue;
        } else if (bytes == -1 && errno == EAGAIN) {
            continue;
        } else {
            printf("Bytes read %zu, but expected %zu in init loop. Thread %d\n", bytes, sizeof tmpResult, i);
            perror("Read failed");
            exit(3);
        }
    }

    for (int i = 0; i < numThreads; ++i) {
        struct task t = {0, 0};
        size_t bytes = write(pipesTask[i][1], &t, sizeof t);
        if (bytes != sizeof t) {
            printf("Bytes written %zu, but expected %zu in finalize loop\n", bytes, sizeof t);
            perror("Write failed");
            exit(2);
        }
    }

    int alive = numThreads;
    while (alive > 0) {
        alive = 0;
        for (int i = 0; i < numThreads; ++i) {
            if (pids[i] == 0) {
                continue;
            }

            alive++;

            unsigned long long tmpResult = 0;
            size_t bytes = read(pipesResult[i][0], &tmpResult, sizeof tmpResult);

            if (bytes == sizeof tmpResult) {
                if (tmpResult == -1) {
                    int status = 0;
                    int pid = waitpid(pids[i], &status, 0);

                    if (status != 0) {
                        printf("Waiting child failed %d (%d)\n", i, pid);
                        perror("Waitpid failed");

                        exit(status);
                    }

                    pids[i] = 0;
                    close(pipesTask[i][1]);
                    close(pipesResult[i][0]);
                    continue;
                }

                result += tmpResult;
                printf("Final received %llu, total %llu\n", tmpResult, result);
            } else if (bytes == 0) {
                continue;
            } else if (bytes == -1 && errno == EAGAIN) {
                continue;
            } else {
                printf("Bytes read %zu, but expected %zu in finish loop\n", bytes, sizeof tmpResult);
                perror("Read failed");
                exit(4);
            }
        }
    }
    printf("%llu\n", result);
    exit(0);
}

unsigned long long squareSum(unsigned long long from, unsigned long long to) {
    unsigned long long result = 0;

    for (unsigned long long i = from; i <= to; ++i) {
        result += i * i;
    }
    
    return result;
}
