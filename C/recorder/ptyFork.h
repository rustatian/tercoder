//
// Created by valery on 16/09/2019.
//

#ifndef SCRIPT_LINUX_PTYFORK_H
#include "sys/types.h"
#define SCRIPT_LINUX_PTYFORK_H

#endif //SCRIPT_LINUX_PTYFORK_H

pid_t ptyFork(int *masterFd, char *slaveName,
        size_t snLen, const struct
        termios *slaveTermios,
        const struct winsize *slaveWS);