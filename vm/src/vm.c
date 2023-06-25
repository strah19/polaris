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

#include "vm.h"
#include "opcodes.h"
#include "debug.h"
#include "value.h"
#include <stdarg.h>
#include <string.h>

#define BINARY(op) \
    { Value b = vm_pop(); \
    Value a = vm_pop(); \
    if (IS_FLOAT(a) && IS_FLOAT(b)) vm_push(FLOAT_VALUE(AS_FLOAT(a) op AS_FLOAT(b))); \
    else if (IS_INT(a) && IS_INT(b)) vm_push(INT_VALUE(AS_INT(a) op AS_INT(b))); \
    else if (IS_INT(a) && IS_FLOAT(b)) vm_push(FLOAT_VALUE(AS_INT(a) op AS_FLOAT(b))); \
    else if (IS_FLOAT(a) && IS_INT(b)) vm_push(FLOAT_VALUE(AS_FLOAT(a) op AS_INT(b))); \
    else if (IS_INT(a) && IS_CHAR(b)) vm_push(INT_VALUE(AS_INT(a) op AS_CHAR(b))); \
    else if (IS_CHAR(a) && IS_INT(b)) vm_push(INT_VALUE(AS_CHAR(a) op AS_INT(b))); \
    else if (IS_CHAR(a) && IS_CHAR(b)) vm_push(CHAR_VALUE(AS_CHAR(a) op AS_CHAR(b))); \
    else if (IS_BOOLEAN(a) && IS_BOOLEAN(b)) vm_push(BOOLEAN_VALUE(AS_BOOLEAN(a) op AS_BOOLEAN(b))); \
    else return vm_runtime_error("Operands must be numbers in '%s' operation and must match.\n", #op); \
    }\

bool IS_TRUE(Value a) {
    if (IS_FLOAT(a) && AS_FLOAT(a)) return true;
    else if (IS_INT(a) && AS_INT(a)) return true;
    else if (IS_BOOLEAN(a) && AS_BOOLEAN(a)) return true;
    else if (IS_CHAR(a) && AS_CHAR(a)) return true;
    else return false;
}

#define INT_BINARY(op) \
    { Value b = vm_pop(); \
    Value a = vm_pop(); \
    if (IS_INT(a) && IS_INT(b)) vm_push(INT_VALUE(AS_INT(a) op AS_INT(b))); \
    else if (IS_BOOLEAN(a) && IS_BOOLEAN(b)) vm_push(BOOLEAN_VALUE(AS_BOOLEAN(a) op AS_BOOLEAN(b))); \
    else return vm_runtime_error("Operands must be integers, booleans, or binaries for '%s' operation.\n", #op); \
    }\

static VM vm;

void vm_init() {
    vm.top = vm.stack;
    vm.fp = 0;
    value_init(&vm.data);
    value_allocate(&vm.data, INITIAL_REFERENCE_SIZE);
}

bool vm_run(Bytecode* bytecode) {
    vm.bytecode = bytecode;
    bool run = true;
    bool skip = false;
    bool ret = false;

#ifdef DEBUG_VM
#ifdef DEBUG_LOG_VM
    freopen("vmlog.txt", "a+", stdout);
#endif
    debug_disassemble_bytecode(bytecode, "Assignment Program");
    printf("\n\n");
#endif

    while (vm.bytecode) {
        vm.ip = bytecode->start_address;
        while (run) {
            uint32_t instruction = bytecode->code[vm.ip];

        #ifdef DEBUG_VM
            debug_disassemble_stack(vm.stack, vm.top);
            debug_disassemble_instruction(vm.bytecode, vm.ip);
       #endif

            switch (instruction) {
            case OP_CONST: {
                vm_push(vm.bytecode->constants.values[bytecode->code[++vm.ip]]); //Pushes the constant onto the stack.
                break;
            }
            case OP_PRINT: {
                value_print(vm_pop(), false);
                break;
            }
            case OP_PUSH: {
                vm_push(INT_VALUE(vm.bytecode->code[++vm.ip]));
                break;
            }
            case OP_HALT: {
                run = false;
                break;
            }
            case OP_STORE: {
                int32_t offset = vm.bytecode->code[++vm.ip];
                vm.stack[(vm.fp - 1) + offset] = vm_pop();
                break;
            }
            case OP_GSTORE: {
                int32_t address = vm.bytecode->code[++vm.ip];
                Value val = vm_pop();
                if (address > vm.data.capacity)
                    return vm_runtime_error("Virtual machine cannot address to %d.\n", address);
                vm.data.values[address] = val;
                break;
            }
            case OP_LOAD: {
                int32_t offset = vm.bytecode->code[++vm.ip];
                vm_push(vm.stack[(vm.fp - 1) + offset]);
                break;
            }
            case OP_GLOAD: {
                int32_t address = vm.bytecode->code[++vm.ip];
                if (address > vm.data.capacity)
                    return vm_runtime_error("Virtual machine cannot address to %d.\n", address);
                vm_push(vm.data.values[address]); //Expects an int value on the stack to be the address.
                break;
            }
            case OP_CALL: {
                int address;
                int num_args;

                vm.ip++;
                address = vm.bytecode->code[vm.ip];
                vm.ip++;
                num_args = vm.bytecode->code[vm.ip];

                vm_push(INT_VALUE(num_args));

                vm_push(INT_VALUE(vm.fp));
                vm_push(INT_VALUE(vm.ip));

                vm.fp = vm.top - vm.stack;
                vm.ip = address;
                continue;
                break;
            }
            case OP_CAST: {
                Value v = vm_pop();
                vm.ip++;
                int old_type = v.type;
                v.type = vm.bytecode->code[vm.ip];
                if (v.type == TYPE_FLOAT && old_type == TYPE_INT) v.float_value = v.int_value;
                else if (v.type == TYPE_INT && old_type == TYPE_FLOAT) v.int_value = v.float_value;
                vm_push(v);
                break;
            }
            case OP_RET: {
                vm.top = vm.stack + vm.fp;
                vm.ip = vm_pop().int_value;
                vm.fp = vm_pop().int_value;
                int32_t args = vm_pop().int_value;
                vm.top -= args;
                break;
            }
            case OP_RETV: {
                Value ret = vm_pop();

                vm.top = vm.stack + vm.fp;
                vm.ip = vm_pop().int_value;
                vm.fp = vm_pop().int_value;
                int32_t args = vm_pop().int_value;
                vm.top -= args;
                vm_push(ret);

                break;
            }
            case OP_JMP: {
                uint32_t skip = vm.bytecode->code[++vm.ip];
                vm.ip = skip - 1;
                break;
            }
            case OP_JMPT: {
                Value value = vm_pop();
                if (IS_TRUE(value)) vm.ip = vm.bytecode->code[vm.ip + 1];
                else vm.ip += 2;
                break;
            }
            case OP_JMPN: {
                Value condition = vm_pop();

                uint32_t skip = vm.bytecode->code[++vm.ip];

                if (!IS_TRUE(condition)) {
                    vm.ip = skip - 1;
                }
                break;
            }
            case OP_NEGATE: {
                Value a = vm_pop();
                if (IS_FLOAT(a)) a.float_value = -a.float_value;
                if (IS_INT(a)) a.int_value = -a.int_value;
                if (IS_BOOLEAN(a)) a.bool_value = -a.bool_value;

                vm_push(a);
                break;
            }
            case OP_ADD: {
                Value v = vm_peek(0);
                if (v.type == TYPE_OBJ && v.obj->type == OBJ_STRING) {
                    Value a = vm_pop();
                    Value b = vm_pop();
                    Value dest;
                    dest.type = TYPE_OBJ;
                    dest.obj = ALLOCATE_OBJ(ObjString, OBJ_STRING);
                    AS_STRING(dest)->chars = ALLOC_STR(AS_STRING(b)->len + AS_STRING(a)->len + 1);
                    strcpy(AS_STRING(dest)->chars, AS_STRING(b)->chars);
                    strcat(AS_STRING(dest)->chars, AS_STRING(a)->chars);
                    vm_push(dest);
                }
                else
                    BINARY(+); 
                break;
            } 
            case OP_MIN: BINARY(-); break;
            case OP_MUL: BINARY(*); break;
            case OP_DIV: BINARY(/); break;
            case OP_MOD: INT_BINARY(%); break;
            case OP_EQL: {
                Value v = vm_peek(0);
                if (v.type == TYPE_OBJ && v.obj->type == OBJ_STRING) {
                    bool dest = (strcmp(AS_STRING(vm_pop())->chars, AS_STRING(vm_pop())->chars) == 0) ? true : false;
                    vm_push(BOOLEAN_VALUE(dest));
                }
                else
                    BINARY(==); 
                break;
            }
            case OP_NEQ: {
                Value v = vm_peek(0);
                if (v.type == TYPE_OBJ && v.obj->type == OBJ_STRING) {
                    bool dest = (strcmp(AS_STRING(vm_pop())->chars, AS_STRING(vm_pop())->chars) == 0) ? false : true;
                    vm_push(BOOLEAN_VALUE(dest));
                }
                else
                    BINARY(!=); 
                break;
            }
            case OP_LTE: BINARY(<=); break;
            case OP_GTE: BINARY(>=); break;
            case OP_LT: BINARY(<); break;
            case OP_GT: BINARY(>); break;
            case OP_AND: BINARY(&&); break;
            case OP_OR: BINARY(||); break;
            case OP_XOR: INT_BINARY(^); break;
            case OP_BOR: INT_BINARY(|); break;
            case OP_BAN: INT_BINARY(&); break;
            case OP_LSF: INT_BINARY(<<); break;
            case OP_RSF: INT_BINARY(>>); break;

            default: {
                return vm_runtime_error("Unknown instruction, '%d' found at line %d.\n", instruction, *vm.bytecode->line);
            }
            }

            vm.ip++;
        }
        vm.bytecode = vm.bytecode->next;
        if (vm.bytecode) 
            run = true;
        else {
        #ifdef DEBUG_LOG_VM
        #ifdef __MINGW32__
            freopen("CON", "w", stdout); 
        #endif
        #elif __linux__
            freopen("/dev/tty", "w", stdout);
        #endif
            return true;
        }
    }

    return true;
}

void vm_free() {
    value_free(&vm.data);
}

extern void vm_push(Value value) {
    *vm.top = value;
    vm.top++;
}

extern Value vm_pop() {
    return *(--vm.top);
}

extern void vm_reset_stack() {
    vm.top = vm.stack;
}

extern Value vm_peek(int off) {
    return vm.top[-1 - off];
}

extern bool vm_runtime_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    printf("\033[0;31mruntime error: \033[0m");
    vprintf(fmt, args);

    va_end(args);

    return false;
}