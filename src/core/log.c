#include "core/log.h"

#include <stdarg.h>
#include <stdio.h>

void log_error(const char* message, ...) {
    va_list args;
    va_start(args, message);

    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");

    va_end(args);
}
