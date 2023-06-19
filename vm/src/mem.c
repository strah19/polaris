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