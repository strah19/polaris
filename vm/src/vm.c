/**
* @file vm.c
* @author strah19
* @date July 13, 2022
* @version 1.0
*
* @section LICENSE
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the MIT License as published
* by the Free Software Foundation.
*
* @section DESCRIPTION
*
* Sets up the virtual machine and stack. Also runs the bytecode.
*/

#include "vm.h"
#include "debug.h"

#include <stdbool.h>

#define BINARY(op) \
    { Value b = vm_pop(); \
    Value a = vm_pop(); \
    if (IS_FLOAT(a) && IS_FLOAT(b)) vm_push(FLOAT_VALUE(AS_FLOAT(a) op AS_FLOAT(b))); \
    else if (IS_INT(a) && IS_INT(b)) vm_push(INT_VALUE(AS_INT(a) op AS_INT(b))); \
    else printf("Operands must be numbers!\n"); \
    }\

static VM vm;

void vm_init() {
    vm.top = vm.stack;
}

void vm_free() {

}

void vm_push(Value value) {
    *vm.top = value;
    vm.top++;
}

Value vm_pop() {
    return *(--vm.top);
}

VMResults vm_run(Bytecode* bytecode) {
    vm.bytecode = bytecode;
    bool run = true;

    while (vm.bytecode) {
        vm.ip = vm.bytecode->code;

        while (run) {
            uint8_t instruction = *vm.ip;

        #ifdef DEBUG_EXECUTION
            debug_disassemble_stack(vm.stack, vm.top);
            debug_disassemble_instruction(vm.bytecode, (int) (vm.ip - vm.bytecode->code));
        #endif

            switch (instruction) {
            case OP_RETURN: run = false; break;
            case OP_CONSTANT: 
                vm_push(vm.bytecode->constants.values[*(++vm.ip)]);
                break;

            case OP_NEGATE: {
                if (!IS_NUMBER(vm_peek())) {
                    printf("Can only negate numbers.\n");
                    return VM_RUNTIME_ERROR;
                }
                if (IS_FLOAT(vm_peek())) vm_push(FLOAT_VALUE(-AS_FLOAT(vm_pop()))); 
                else vm_push(INT_VALUE(-AS_INT(vm_pop()))); 
                
                break;
            }
            case OP_NOT: {
                if (!IS_NUMBER(vm_peek())) {
                    printf("Can only not numbers.\n");
                    return VM_RUNTIME_ERROR;
                }
                if (IS_FLOAT(vm_peek())) vm_push(FLOAT_VALUE(!AS_FLOAT(vm_pop()))); 
                else vm_push(INT_VALUE(!AS_INT(vm_pop()))); 
                
                break;
            }

            case OP_PLUS:      BINARY(+);  break;
            case OP_MINUS:     BINARY(-);  break;
            case OP_MULTIPLY:  BINARY(*);  break;
            case OP_DIVIDE:    BINARY(/);  break;
            case OP_EQUAL:     BINARY(==); break;
            case OP_NOT_EQUAL: BINARY(!=); break;
            case OP_LT:        BINARY(<);  break;
            case OP_GT:        BINARY(>);  break;
            case OP_LTE:       BINARY(<=); break;
            case OP_GTE:       BINARY(>=); break;
            case OP_AND:       BINARY(&&); break;
            case OP_OR:        BINARY(||); break;
            }
            vm.ip++;
        }

        vm.bytecode = vm.bytecode->next;
        if (vm.bytecode) 
            run = true;
        else 
            return VM_OK;
    }
    return VM_OK;
}

void vm_reset_stack() {
    vm.top = vm.stack;
}

Value vm_peek() {
    return vm.top[-1];
}