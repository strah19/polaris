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

bool IS_TRUE(Value a) {
    if (IS_FLOAT(a) && AS_FLOAT(a)) return true;
    else if (IS_INT(a) && AS_INT(a)) return true;
    else if (IS_BOOLEAN(a) && AS_BOOLEAN(a)) return true;
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

    debug_disassemble_bytecode(bytecode, "Assignment Program");
    printf("\n\n");

    while (vm.bytecode) {
        vm.ip = bytecode->start_address;
        while (run) {
            uint32_t instruction = bytecode->code[vm.ip];

            debug_disassemble_stack(vm.stack, vm.top);
            debug_disassemble_instruction(vm.bytecode, vm.ip);

            switch (instruction) {
            case OP_CONST: {
                vm_push(vm.bytecode->constants.values[bytecode->code[++vm.ip]]); //Pushes the constant onto the stack.
                break;
            }
            case OP_PRINT: {
                value_print(vm_pop(), true);
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
                int address = vm_pop().int_value;
                Value val = vm_pop();
                if (address > vm.data.capacity)
                    return vm_runtime_error("Virtual machine cannot address to %d.\n", address);
                vm.data.values[address] = val;
                break;
            }
            case OP_LOAD: {
                int32_t offset = vm.bytecode->code[++vm.ip];
                printf("%d\n", (vm.fp - 1) + offset);
                vm_push(vm.stack[(vm.fp - 1) + offset]);
                break;
            }
            case OP_GLOAD: {
                int address = vm_pop().int_value;
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
            case OP_RET: {
                Value ret = vm_pop();

                vm.top = vm.stack + vm.fp;
                vm.ip = vm_pop().int_value;
                vm.fp = vm_pop().int_value;
                int32_t args = vm_pop().int_value;
                vm.top -= args;
                vm_push(ret);
                printf("TOP OF STACK %d.\n", vm.top - vm.stack);
                printf("IP %d.\n", vm.ip);
                printf("FP %d.\n", vm.fp);

                break;
            }
            case OP_JMP: {
                vm.ip = vm.bytecode->code[vm.ip + 1];
                break;
            }
            case OP_JMPT: {
                Value value = vm_pop();
                if (IS_TRUE(value)) vm.ip = vm.bytecode->code[vm.ip + 1];
                else vm.ip += 2;
                break;
            }
            case OP_JMPN: {
                Value value = vm_pop();
                if (!IS_TRUE(value)) vm.ip = vm.bytecode->code[vm.ip + 1];
                else vm.ip += 2;
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