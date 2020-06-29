//
// Created by valery on 16/09/2019.
//

#ifndef SCRIPT_LINUX_ERRORS_H
#define SCRIPT_LINUX_ERRORS_H

#endif //SCRIPT_LINUX_ERRORS_H
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>

void err_exit(const char *format, ...)
{
    va_list argList;

    va_start(argList, format);
    va_end(argList);

    _exit(1);
}
