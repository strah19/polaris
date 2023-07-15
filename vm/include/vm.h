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

#ifndef VM_H
#define VM_H

#include "bytecode.h"

#define MAX_STACK 512
#define INITIAL_REFERENCE_SIZE 512

typedef struct {
    Bytecode* bytecode;
    uint32_t ip;
    int32_t fp;
    Value stack[MAX_STACK];
    Values data;
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