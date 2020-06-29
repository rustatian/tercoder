//
// Created by valery on 17/09/2019.
//

#include "recorder.h"
#include <fcntl.h>
#include <termios.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <stddef.h>
#include "string.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "paths.h"

#define BUF_SIZE 256
#define MAX_SNAME 1000

struct termios ttyOrigin;

static void ttyReset(void) {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &ttyOrigin) == -1) {
        printf("error while setting origin");
        _exit(1);
    }
}

int main(int argc, char *argv[]) {
    char slaveName[MAX_SNAME];
    char *shell;
    int masterFd, recorderFd;
    struct winsize ws;
    fd_set inFds;
    char buf[BUF_SIZE];
    ssize_t numRead;
    pid_t childPid;

    if (tcgetattr(STDIN_FILENO, &ttyOrigin) == -1) {
        printf("error on tcgetattr");
        exit(1);
    }
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0) {
        printf("error on ioctl");
        exit(1);
    }

    childPid = ptyFork(&masterFd, slaveName, MAX_SNAME, &ttyOrigin, &ws);
    if (childPid == -1) {
        printf("error on ptyFork");
        exit(1);
    }

    if (childPid == 0) { // child execute a shell on pty slave, this is the controlling process
        shell = getenv("SHELL");
        if (shell == NULL || *shell == '\0') {
            shell = _PATH_BSHELL;
        }

        execlp(shell, shell, (char *) NULL);
    }

    recorderFd = open((argc > 1) ? argv[1] : "recorderscript", O_WRONLY | O_CREAT | O_TRUNC,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if (recorderFd == -1) {
        printf("error on recorderScript");
        exit(1);
    }

    ttySetRaw(STDIN_FILENO, &ttyOrigin);

    if (atexit(ttyReset) != 0) {
        printf("atexit");
        exit(1);
    }

    for (;;) {
        FD_ZERO(&inFds);
        FD_SET(STDIN_FILENO, &inFds);
        FD_SET(masterFd, &inFds);

        //The nfds argument must be set one greater than the highest file descriptor
        //number included in any of the three file descriptor sets. This argument allows
        //select() to be more efficient, since the kernel then knows not to check whether file
        //descriptor numbers higher than this value are part of each file descriptor set.
        if (select(masterFd + 1, &inFds, NULL, NULL, NULL) == -1) {
            printf("select");
            exit(1);
        }

        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        ioctl(masterFd, TIOCSWINSZ, &ws);

        Header header;

        // this is the user stdin
        if (FD_ISSET(STDIN_FILENO, &inFds)) { // stdin --> pty
            numRead = read(STDIN_FILENO, buf, BUF_SIZE);
            if (numRead <= 0) {
                printf("success");
                exit(0);
            }

            if (write(masterFd, buf, numRead) != numRead) {
                printf("fatal, partial");
                exit(1);
            }
        }


        // this is stdout from terminal
        if (FD_ISSET(masterFd, &inFds)) {
            numRead = read(masterFd, buf, BUF_SIZE);
            if (numRead <= 0) {
                if (tcsetattr(masterFd, TCSAFLUSH, &ttyOrigin) == -1) {
                    printf("failed to flush");
                    exit(1);
                }
                printf("success");
                exit(0);
            }

//            gettimeofday(&header.tv, NULL);


            if (write(STDOUT_FILENO, buf, numRead) != numRead) {
                printf("fatal, partial write");
                exit(1);
            }

            // write times to file
//            if (write_header(recorderFd, &header) != 1) {
//                printf("fatal error when write FD");
//                exit(1);
//            }

            // write info to file
            if (write(recorderFd, buf, numRead) != numRead) {
                printf("fatal, partial (recorderFd)");
                exit(1);
            }
        }
    }
}


































