//
// Created by Valery Piashchynski on 1/10/19.
//

#ifndef RECORDER_PTY_FORK_HPP
//#include "sys/types.h"
#define RECORDER_PTY_FORK_HPP

pid_t ptyFork(int *masterFd, char *slaveName, size_t snLen, const struct termios *slaveTermios, const struct winsize *slaveWS);

#endif //RECORDER_PTY_FORK_HPP

