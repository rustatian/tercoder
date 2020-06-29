//
// Created by valery on 17/09/2019.
//

#ifndef SCRIPT_LINUX_RECORDER_H

#include "pty_master_open.h"
#include "ptyFork.h"
#include "tty_raw.h"
#include "sys/time.h"

typedef struct header {
    struct timeval tv;
    int len;
} Header;

#define SCRIPT_LINUX_RECORDER_H

#endif //SCRIPT_LINUX_RECORDER_H
