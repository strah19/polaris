#include "bytecode.h"
#include <stdlib.h>
#include "mem.h"

void init_chunk(BytecodeChunk* chunk) {
    chunk->capacity = 0;
    chunk->count = 0;
    chunk->line = NULL;
    chunk->code = NULL;
    chunk->next = NULL;
    init_value_array(&chunk->constants);
}

void write_chunk(uint8_t code, BytecodeChunk* chunk, int line) {
    if (chunk->capacity < chunk->count + 1) {
        chunk->capacity = NEW_CAPACITY(chunk->capacity);
        chunk->code = REALLOC(uint8_t,  chunk->code, chunk->capacity);
        chunk->line = REALLOC(int, chunk->line, chunk->capacity);
    }

    chunk->code[chunk->count++] = code;
    chunk->line[chunk->count - 1] = line;
}

void pop_chunk(BytecodeChunk* chunk) {
    chunk->count--;
}

// Will return the address of the constant
int add_constant(Value value, BytecodeChunk* chunk) {
    write_value_array(value, &chunk->constants);
    return chunk->constants.count - 1;
}

void free_chunk(BytecodeChunk* chunk) {
    FREE(uint8_t, chunk->code);
    FREE(int, chunk->line);
    free_value_array(&chunk->constants);
    init_chunk(chunk);
}

void append_chunk(BytecodeChunk* chunk, BytecodeChunk* append) {
    chunk->next = append;
}

BytecodeChunk* get_head_chunk(BytecodeChunk* chunk) {
    BytecodeChunk* head = chunk;
    while (head->next) head = head->next;
    return head;
}