#ifndef VM_H
#define VM_H

#include "bytecode.h"

#define DEBUG_EXECUTION
#define MAX_STACK 256

typedef struct {
    BytecodeChunk* chunk;
    uint8_t* ip;    // Instruction pointer
    Value stack[MAX_STACK];
    Value* top;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILER_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpreterResults;

extern void init_vm();

extern void free_vm();

extern InterpreterResults interpret(BytecodeChunk* chunk);

extern InterpreterResults run_vm();

extern void push(Value value);

extern Value pop();

#endif // !VM_H