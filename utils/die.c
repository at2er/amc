#include "die.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void die(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);

    exit(1);
}
