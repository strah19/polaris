#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdint.h>
#include <stdio.h>
#include "value.h"

typedef struct {
    int capacity;
    int count;
    uint8_t* code;
    int* line;
    ValueArray constants;
} BytecodeChunk;

extern void init_chunk(BytecodeChunk* chunk);

extern void write_chunk(uint8_t code, BytecodeChunk* chunk, int line);

extern int add_constant(Value value, BytecodeChunk* chunk);

extern void free_chunk(BytecodeChunk* chunk);

#endif // !BYTECODE_H