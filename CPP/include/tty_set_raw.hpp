//
// Created by Valery Piashchynski on 1/10/19.
//

#ifndef RECORDER_TTY_SET_RAW_H
#define RECORDER_TTY_SET_RAW_H

#include "termios.h"
#include <cstdlib>


#endif //RECORDER_TTY_SET_RAW_H

int ttySetRaw(int fd, struct termios *prevTermios);