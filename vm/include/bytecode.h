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

#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdint.h>
#include <stdio.h>

#include "value.h"

struct Bytecode {
    int capacity;
    int count;
    uint32_t* code;
    int* line;
    Values constants;
    struct Bytecode* next;
    int start_address;
};

typedef struct Bytecode Bytecode;

extern void bytecode_init(Bytecode* bytecode);

extern void bytecode_write(uint32_t code, int line, Bytecode* bytecode);

extern void bytecode_pop(Bytecode* bytecode);

extern int  bytecode_add_constant(Value value, Bytecode* bytecode);

extern void bytecode_free(Bytecode* bytecode);

extern void bytecode_append(Bytecode* bytecode, Bytecode* append);

#endif //!BYTECODE_H