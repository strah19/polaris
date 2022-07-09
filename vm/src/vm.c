#include "vm.h"
#include "bytecode.h"
#include "opcodes.h"
#include "debug.h"
#include <stdbool.h>

#define BINARY(op) \
    { Value a = pop(); \
    Value b = pop(); \
    push(a op b); }\

static VM vm;

void init_vm() {
    vm.top = vm.stack;
}

void free_vm() {

}

void push(Value value) {
    *vm.top = value;
    vm.top++;
}

Value pop() {
    return *(--vm.top);
}

InterpreterResults interpret(BytecodeChunk* chunk) {
    vm.chunk = chunk;
    vm.ip = chunk->code;

    return run_vm();
}

InterpreterResults run_vm() {
    while (true) {
        uint8_t instruction = *vm.ip;

    #ifdef DEBUG_EXECUTION
        if (vm.stack != vm.top) {
            printf("stack: ");
            for (Value* i = vm.stack; i < vm.top; i++) {
                printf ("[ ");
                print_value(*i);
                printf(" ]");
            }
            printf("\n");
        }
        disassemble_instruction(vm.chunk, (int) (vm.ip - vm.chunk->code));
    #endif

        switch (instruction) {
        case OP_RETURN: return INTERPRET_OK;
        case OP_CONSTANT: 
            push(vm.chunk->constants.values[*(++vm.ip)]);
            break;
        case OP_NEGATE: 
            push(-pop());
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
}