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
    else if (IS_BOOLEAN(a) && IS_BOOLEAN(b)) vm_push(BOOLEAN_VALUE(AS_BOOLEAN(a) op AS_BOOLEAN(b))); \
    else return vm_runtime_error("Operands must be numbers in '%s' operation and must match.\n", #op); \
    }\

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
    value_init(&vm.globals);
}

bool vm_run(Bytecode* bytecode) {
    vm.bytecode = bytecode;
    bool run = true;

    while (vm.bytecode) {
        vm.ip = &vm.bytecode->code[bytecode->start_address];
        while (run) {
            uint8_t instruction = *vm.ip;

           // debug_disassemble_stack(vm.stack, vm.top);
           // debug_disassemble_instruction(vm.bytecode, (int) (vm.ip - vm.bytecode->code));

            switch (instruction) {
            case OP_RETURN: {
                run = false;
                break;
            }
            case OP_CONST: {
                vm_push(vm.bytecode->constants.values[*(++vm.ip)]); //Pushes the constant onto the stack.
                break;
            }
            case OP_PRINT: {
                value_print(vm_pop(), true);
                break;
            }
            case OP_ADD: {
                Value a = vm_peek(0);
                Value b = vm_peek(1);
                if (IS_STRING(a) && IS_STRING(b)) {
                    vm_pop();
                    vm_pop();
                    strcat(AS_STRING(b)->chars, AS_STRING(a)->chars);
                    vm_push(b);
                }
                else
                    BINARY(+);
                break;
            }
            case OP_DEF_GLOBAL: {
                Value val = vm_pop();
                value_write(val, &vm.globals);
                break;
            }
            case OP_SET_GLOBAL: {
                int address = vm_pop().int_value;
                Value val = vm_pop();
                if (address > vm.globals.count)
                    return vm_runtime_error("Virtual machine cannot address to %d.\n", address);
                vm.globals.values[address] = val;
                break;
            }
            case OP_GET_GLOBAL: {
                int address = vm_pop().int_value;
                if (address > vm.globals.count)
                    return vm_runtime_error("Virtual machine cannot address to %d.\n", address);
                vm_push(vm.globals.values[address]); //Expects an int value on the stack to be the address.
                break;
            }
            case OP_JMP: {
                int jump_address = vm_pop().int_value;
                vm.ip = vm.bytecode->code + jump_address;
                break;
            }
            case OP_RTS: {
                Value return_value = vm_pop();
                int jump_address = vm_pop().int_value;
                vm.ip = vm.bytecode->code + jump_address;
                vm_push(return_value);                
                break;
            }
            case OP_CALL: {
                int jump_address = vm_pop().int_value;
                vm_push(INT_VALUE(vm.ip - vm.bytecode->code));
                vm.ip = vm.bytecode->code + jump_address - 1;
                break;
            }
            case OP_MIN: BINARY(-); break;
            case OP_MUL: BINARY(*); break;
            case OP_DIV: BINARY(/); break;
            case OP_MOD: INT_BINARY(%); break;
            case OP_EQL: BINARY(==); break;
            case OP_NEQ: BINARY(!=); break;
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
        else
            return true;
    }

    return true;
}

void vm_free() {
    value_free(&vm.globals);
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