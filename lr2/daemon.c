#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

static char *const PID_FILE_NAME = "/var/run/lr2_daemon.pid";
static char *const LOG_FILE_NAME = "/var/log/lr2_daemon.log";

int main() {
    int pid;
    if ((pid = fork()) == -1) {
        perror("Can't fork");
        exit(1);
    } else if (pid == 0) {
        /* child */
        setsid();

        FILE *file = fopen(PID_FILE_NAME, "r+");
        FILE *log = fopen(LOG_FILE_NAME, "a");
        fprintf(log, "child:PID=%d,PPID=%d\n", getpid(), getppid());

        if (file != 0) {
            fclose(file);
            fprintf(log, "Daemon already running\n");
            exit(-1);
        }

        file = fopen(PID_FILE_NAME, "w");

        if (file == 0) {
            fprintf(log, "Cannot open pid file: %s\n", strerror(errno));
            exit(-1);
        }

        fprintf(file, "%d", getpid());

        fclose(file);

        fprintf(log, "Daemon ready\n");

        fflush(log);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
        while (1) {
            fprintf(log, "Working\n");
            fflush(log);

            sleep(1);
        }
#pragma clang diagnostic pop

    } else {
        /* parent */
        printf("Parent:childPID=%d, PID=%d\n, PPID=%d\n",
               pid, getpid(), getppid());
        exit(0);
    }
}
