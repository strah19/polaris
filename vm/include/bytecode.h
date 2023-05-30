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