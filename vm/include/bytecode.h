#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdint.h>
#include <stdio.h>
#include "value.h"

struct BytecodeChunk {
    int capacity;
    int count;
    uint8_t* code;
    int* line;
    ValueArray constants;
    struct BytecodeChunk* next;
};

typedef struct BytecodeChunk BytecodeChunk;

extern void init_chunk(BytecodeChunk* chunk);

extern void write_chunk(uint8_t code, BytecodeChunk* chunk, int line);

extern void pop_chunk(BytecodeChunk* chunk);

extern int add_constant(Value value, BytecodeChunk* chunk);

extern void free_chunk(BytecodeChunk* chunk);

extern void append_chunk(BytecodeChunk* chunk, BytecodeChunk* append);

extern BytecodeChunk* get_head_chunk(BytecodeChunk* chunk);

#endif // !BYTECODE_H