//
// Created by Valery Piashchynski on 1/10/19.
//

#include "../include/tty_set_raw.hpp"
#include "termios.h"
#include <cstdlib>

int ttySetRaw(int fd, struct termios *prevTermios) {
    struct termios t{};

    if (tcgetattr(fd, &t) == -1) {
        return -1;
    }

    if (prevTermios != nullptr) {
        *prevTermios = t;
    }

    t.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);
    t.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | IGNCR | INLCR | INPCK | ISTRIP | IXON | PARMRK);
    t.c_oflag &= ~OPOST; //disable all output processing

    t.c_cc[VMIN] = 1; //character at time input mode
    t.c_cc[VTIME] = 0; // with blocking

    if (tcsetattr(fd, TCSAFLUSH, &t) == -1) {
        return -1;
    }

    return 0;

}