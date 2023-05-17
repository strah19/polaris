#include "vm.h"
#include "opcodes.h"
#include "value.h"
#include <stdarg.h>

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
}

bool vm_run(Bytecode* bytecode) {
    vm.bytecode = bytecode;
    bool run = true;

    while (vm.bytecode) {
        vm.ip = vm.bytecode->code;
        while (run) {
            uint8_t instruction = *vm.ip;

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
            case OP_ADD: BINARY(+); break;
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