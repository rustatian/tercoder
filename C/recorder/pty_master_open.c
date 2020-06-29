//
// Created by valery on 15/09/2019.
//
#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "pty_master_open.h"
#include <string.h>

int ptyMasterOpen(char *slaveName, size_t snLen) {
    int masterFd, savedErrno;
    char *p;

    // O_NOCTTY --> don't make that terminal controlling
    masterFd = posix_openpt(O_RDWR | O_NOCTTY); // returns a master FD
    if (masterFd == 1) {
        return -1;
    }

    if ((grantpt(masterFd) == -1)) {
        savedErrno = errno;
        close(masterFd); // might change errno
        errno = savedErrno;
        return -1;
    }

    if (unlockpt(masterFd) == -1) { // unlock slave pty
        savedErrno = errno;
        close(masterFd); // might change errno
        errno = savedErrno;
        return -1;
    }

    p = ptsname(masterFd); //get slave pty name
    if (p == NULL) {
        savedErrno = errno;
        close(masterFd); // might change errno
        errno = savedErrno;
        return -1;
    }

    if (strlen(p) < snLen) {
        strncpy(slaveName, p, snLen);
    } else { // return and error if buffer is too small
        close(masterFd);
        errno = EOVERFLOW; //overflow error
        return -1;
    }
    return masterFd;
}