/**
 * Copyright (C) 2023 Strahinja Marinkovic - All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License as
 * published by the Free Software Foundation.
 *
 * You should have received a copy of the MIT License along with
 * this program. If not, see https://opensource.org/license/mit/
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void fatal_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    fprintf(stderr, "\033[0;31mfatal error: \033[0m");
    vprintf(fmt, args);

    va_end(args);
    exit(EXIT_FAILURE);
}

void report_warning(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    fprintf(stdout, "\033[1;33mwarning: \033[0m");    
    vprintf(fmt, args);

    va_end(args);
}

void report_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    fprintf(stderr, "\033[0;31merror: \033[0m");    
    vprintf(fmt, args);

    va_end(args);
}