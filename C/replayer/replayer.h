//
// Created by valery on 20/09/2019.
//

#ifndef TTY_REPLAY_TTYPLAY_H
#define TTY_REPLAY_TTYPLAY_H

#include "sys/time.h"

typedef struct header {
    struct timeval tv;
    int len;
} Header;

#endif //TTY_REPLAY_TTYPLAY_H
