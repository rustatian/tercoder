//
// Created by Valery Piashchynski on 1/10/19.
//

#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include "../include/pty_master.hpp"

#define MAX_SNAME 1000

pid_t ptyFork(int *masterFd, char *slaveName, size_t snLen, const struct termios *slaveTermios, const struct winsize *slaveWS) {
    int mfd, slaveFd, savedErrno;
    pid_t childPid;
    char slname[MAX_SNAME];

    mfd = ptyMasterOpen(slname, MAX_SNAME);
    if (mfd == -1) {
        return -1;
    }

    if (slaveName != nullptr) { // this is slave name of the caller
        if (strlen(slname) < snLen) {
            strncpy(slaveName, slname, snLen);
        } else { // slave name was too small
            close(mfd);
            errno = EOVERFLOW;
            return -1;
        }
    }

    // get terminal info
    ioctl(STDIN_FILENO, TIOCGWINSZ, &slaveWS);
    // set serminal info (to the master)
    ioctl(mfd, TIOCSWINSZ, &slaveWS);

    childPid = fork();
    if (childPid == -1) {
        savedErrno = errno;
        close(mfd);
        errno = savedErrno;
        return -1;
    }


    // from this point there is 2 processes, child and parent --> childPid = fork();
    // child PID will be always 0, so, at this point we are set masterFd to *masterFd
    // and return PID for child (zero) from first process
    // BUT, as far as parent pid is not equal to 0 in that process we continue execution (child PID is equal to 0 in his process)
    if (childPid != 0) {
        *masterFd = mfd;
        return childPid;
    }


    // for child execution continues
    if (setsid() == -1) {
        printf("error in setsid");
        _exit(1);
    }


    close(mfd);

    slaveFd = open(slname, O_RDWR);
    if (slaveFd == -1) {
        printf("error in open slname");
        _exit(1);
    }

#ifdef TIOCSCTTY // <----------- for BSD
    if (ioctl(slaveFd, TIOCSCTTY, 0) == -1) {
        printf("error in ioctl");
        _exit(1);
    }
#endif
    if (slaveTermios != nullptr){

        if (tcsetattr(slaveFd, TCSANOW, slaveTermios) == -1) {
            printf("error tcsetattr");
            _exit(1);
        }
    }
//    tcgetattr(STDIN_FILENO, &tt)
//
//
//    ioctl(fd_termios, TIOCGWINSZ, &ws);
//    ioctl(fdm, TIOCSWINSZ, &ws);

    if (slaveWS != nullptr) {
//        The ioctl(fd, TIOCNOTTY) operation can be used to remove a process’s association
//        with its controlling terminal, specified via the file descriptor fd.
        if (ioctl(slaveFd, TIOCSCTTY, slaveWS) == -1) {
            if (tcsetattr(slaveFd, TCSANOW, slaveTermios) == -1) {
                printf("error ioctl(slaveFd, TIOCSCTTY, slaveWS)");
                exit(1);
            }
        }
    }

    // duplicate pty slave to be child's stdin, stdout and stderr

    if (dup2(slaveFd, STDIN_FILENO) != STDIN_FILENO) {
        printf("error dup2 STDIN_FILENO");
        exit(1);
    }

    if (dup2(slaveFd, STDOUT_FILENO) != STDOUT_FILENO) {
        printf("error dup2 STDOUT_FILENO");
        exit(1);
    }

    if (dup2(slaveFd, STDERR_FILENO) != STDERR_FILENO) {
        printf("error dup2 STDERR_FILENO");
        exit(1);
    }

    if(slaveFd > STDERR_FILENO) {
        close(slaveFd);
    }

    return 0;

}