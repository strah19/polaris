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
    vm_push(a op b); }\

static VM vm;

/**
 * @brief Sets up the stack.
 * 
 */
void vm_init() {
    vm.top = vm.stack;
}

/**
 * @brief Frees any memory from the virtual machine.
 * 
 */
void vm_free() {

}

/**
 * @brief Pushs value onto the stack.
 * 
 * @param value 
 */
void vm_push(Value value) {
    *vm.top = value;
    vm.top++;
}

/**
 * @brief Pops value from stack.
 * 
 * @return Value 
 */
Value vm_pop() {
    return *(--vm.top);
}

/**
 * @brief Runs a piece of bytecode.
 * 
 * @param bytecode 
 * @return VMResults 
 */
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
            case OP_NEGATE: 
                vm_push(-vm_pop());
                break;
            case OP_PLUS: 
                BINARY(+);
                break;
            case OP_MINUS: 
                BINARY(-);
                break;
            case OP_MULTIPLY: 
                BINARY(*);
                break;
            case OP_DIVIDE: 
                BINARY(/);
                break;
            }
            vm.ip++;
        }

        vm.bytecode = vm.bytecode->next;
        if (vm.bytecode) 
            run = true;
        else 
            return VM_OK;
    }
}