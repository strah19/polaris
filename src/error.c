/**
* @file error.c
* @author strah19
* @date July 12, 2022
* @version 1.0
*
* @section LICENSE
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the MIT License as published
* by the Free Software Foundation.
*
* @section DESCRIPTION
*
* The basic error outline. Has the ability to shutdown the compiler
* or just report an error or warning.
*/

#include "error.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void fatal_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    printf("\033[0;31mfatal error: \033[0m");
    vprintf(fmt, args);

    va_end(args);
    exit(EXIT_FAILURE);
}

void report_warning(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    printf("\033[1;33mwarning: \033[0m");    
    vprintf(fmt, args);

    va_end(args);
}

void report_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    printf("\033[0;31merror: \033[0m");    
    vprintf(fmt, args);

    va_end(args);
}