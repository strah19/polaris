/**
* @file mem.c
* @author strah19
* @date July 13, 2022
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
* Helper functions for custom dynamic arrays.
*/

#include "mem.h"

void* reallocate(void* pointer, size_t new_size) {
    if (new_size == 0) {
        free(pointer);
        return NULL;
    }

    void* new = realloc(pointer, new_size);
    if (!new) exit(1);
    return new;
}