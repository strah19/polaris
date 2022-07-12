#ifndef VM_H
#define VM_H

#include "bytecode.h"
#include "opcodes.h"

#define DEBUG_EXECUTION
#define MAX_STACK 256

typedef struct {
    BytecodeChunk* chunk;
    uint8_t* ip;    // Instruction pointer
    Value stack[MAX_STACK];
    Value* top;
} VM;

typedef enum {
    VM_OK,
    VM_COMPILER_ERROR,
    VM_RUNTIME_ERROR
} VMResults;

extern void init_vm();

extern void free_vm();

extern VMResults run_vm(BytecodeChunk* chunk);

extern void push(Value value);

extern Value pop();

#endif // !VM_H