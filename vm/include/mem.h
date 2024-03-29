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

#ifndef MEM_H
#define MEM_H

#include <stdlib.h>
#include <stdio.h>

#define NEW_CAPACITY(old_capacity) (old_capacity < 8) ? 8 : (old_capacity * 2)

#define REALLOC(type, pointer, new_size) (type*) reallocate(pointer, sizeof(type) * new_size)

#define FREE(type, pointer) (type*) reallocate(pointer, 0)

#define ALLOC(type) (type*) malloc(sizeof(type))

#define ALLOC_ARRAY(type, size) (type*) malloc(sizeof(type) * size)

#define ALLOC_STR(size) (char*) malloc(size)

extern void* reallocate(void* pointer, size_t new_size);

#endif // !MEM_H