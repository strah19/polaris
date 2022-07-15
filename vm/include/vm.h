#ifndef VM_H
#define VM_H

#include "bytecode.h"
#include "opcodes.h"

#define DEBUG_EXECUTION
#define MAX_STACK 256

typedef struct {
    Bytecode* bytecode;
    uint8_t* ip;   
    Value stack[MAX_STACK];
    Value* top;
} VM;

typedef enum {
    VM_OK,
    VM_COMPILER_ERROR,
    VM_RUNTIME_ERROR
} VMResults;

extern void vm_init();

extern void vm_free();

extern VMResults vm_run(Bytecode* bytecode);

extern void vm_push(Value value);

extern Value vm_pop();

#endif // !VM_H