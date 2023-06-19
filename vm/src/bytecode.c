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

#include "bytecode.h"
#include "mem.h"
#include <stdlib.h>

void bytecode_init(Bytecode* bytecode) {
    bytecode->capacity = 0;
    bytecode->count = 0;
    bytecode->line = NULL;
    bytecode->code = NULL;
    bytecode->next = NULL;
    bytecode->start_address = 0;
    value_init(&bytecode->constants);
}

void bytecode_write(uint32_t code, int line, Bytecode* bytecode) {
    if (bytecode->capacity < bytecode->count + 1) {
        bytecode->capacity = NEW_CAPACITY(bytecode->capacity);
        bytecode->code = REALLOC(uint32_t,  bytecode->code, bytecode->capacity);
        bytecode->line = REALLOC(int, bytecode->line, bytecode->capacity);
    }

    bytecode->code[bytecode->count++] = code;
    bytecode->line[bytecode->count - 1] = line;
}

void bytecode_pop(Bytecode* bytecode) {
    bytecode->count--;
}

int bytecode_add_constant(Value value, Bytecode* bytecode) {
    value_write(value, &bytecode->constants);
    return bytecode->constants.count - 1;
}

void bytecode_free(Bytecode* bytecode) {
    FREE(uint8_t, bytecode->code);
    FREE(int, bytecode->line);
    value_free(&bytecode->constants);

    if (bytecode->next) {
        bytecode_free(bytecode->next);
        free (bytecode->next);
    }
    
    bytecode_init(bytecode);
}

void bytecode_append(Bytecode* bytecode, Bytecode* append) {
    bytecode->next = append;
}