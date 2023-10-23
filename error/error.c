#include "error.h"

#include <stdio.h>
#include <stdarg.h>

void error(const char *msg){
    fprintf(stderr, "Error: %s\n", msg);
}

void errorf(const char *format, ...){
    va_list args;
    va_start(args, format);

    char msg[1024];
    vsnprintf(&msg[0], 1024, format, args);

    va_end(args);

    error(&msg[0]);
}