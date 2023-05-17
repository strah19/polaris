#ifndef VM_H
#define VM_H

#include "bytecode.h"

#define MAX_STACK 256

typedef struct {
    Bytecode* bytecode;
    uint8_t* ip;   
    Value stack[MAX_STACK];
    Value* top;
} VM;

extern void vm_init();

extern bool vm_run(Bytecode* bytecode);

extern void vm_push(Value value);

extern Value vm_pop();

extern void vm_reset_stack();

extern Value vm_peek(int off);

extern bool vm_runtime_error(const char* fmt, ...);

extern void vm_free();

#endif