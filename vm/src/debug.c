/**
* @file debug.c
* @author strah19
* @date July 12, 2022
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
* Helper functions to view what the virtual machine is actually doing.
*/

#include "debug.h"
#include "opcodes.h"

static int debug_simple_instruction(const char* name, int off);
static int debug_constant_instruction(Bytecode* bytecode, int off);
static int debug_call_instruction(Bytecode* bytecode, int off);
static int debug_if_instruction(Bytecode* bytecode, int off);
static int debug_jmp_instruction(Bytecode* bytecode, int off);

void debug_disassemble_bytecode(Bytecode* bytecode, const char* name) {
    printf("----- %s -----\n", name);
    for (int i = 0; i < bytecode->count;) 
        i = debug_disassemble_instruction(bytecode, i);
}

int debug_disassemble_instruction(Bytecode* bytecode, int off) {
    printf ("%04d ", off);

    if (off > 0 && bytecode->line[off] == bytecode->line[off - 1])
        printf("   | ");
    else
        printf("%4d ", bytecode->line[off]);

    uint8_t instruction = bytecode->code[off];
    switch (instruction) {
    case OP_RETURN:    return debug_simple_instruction("OP_RETURN",    off);
    case OP_ADD:      return debug_simple_instruction("OP_ADD",      off);
    case OP_MIN:     return debug_simple_instruction("OP_MINUS",     off);
    case OP_MUL:  return debug_simple_instruction("OP_MULTIPLY",  off);
    case OP_DIV:    return debug_simple_instruction("OP_DIVDE",     off);
    case OP_MOD:    return debug_simple_instruction("OP_MODULO",    off);
    case OP_AND:       return debug_simple_instruction("OP_AND",       off);
    case OP_OR:        return debug_simple_instruction("OP_OR",        off);
    case OP_EQL:     return debug_simple_instruction("OP_EQUAL",     off);
    case OP_NEQ: return debug_simple_instruction("OP_NOT_EQUAL", off);
    case OP_LT:        return debug_simple_instruction("OP_LT",        off);
    case OP_GT:        return debug_simple_instruction("OP_GT",        off);
    case OP_LTE:       return debug_simple_instruction("OP_LTE",       off);
    case OP_GTE:       return debug_simple_instruction("OP_GTE",       off);
    case OP_BNQ:       return debug_if_instruction(bytecode, off);
    case OP_PRINT:     return debug_simple_instruction("OP_PRINT",     off);
    case OP_GET:       return debug_simple_instruction("OP_GET",       off);
    case OP_SET:       return debug_simple_instruction("OP_SET",       off);
    case OP_CONST:  return debug_constant_instruction(bytecode,     off);
    case OP_RTS:       return debug_simple_instruction("OP_RTS",       off);
    case OP_JMP:       return debug_jmp_instruction(bytecode,       off);
    case OP_CALL:       return debug_call_instruction(bytecode,       off);
    case OP_FUNC_START:       return debug_simple_instruction("OP_FUNC_START",       off);
    case OP_FUNC_END:       return debug_simple_instruction("OP_FUNC_END",       off);
    default:
        printf("Unknown opcode %d\n", instruction);
        return off + 1;
    }
}

int debug_simple_instruction(const char* name, int off) {
    printf("%s\n", name);
    return off + 1;
}

int debug_constant_instruction(Bytecode* bytecode, int off) {
    uint8_t constant_address = bytecode->code[off + 1];
    printf("OP_CONSTANT ");
    printf("%04d '", constant_address);
    value_print(bytecode->constants.values[constant_address], false);
    printf("'\n");
    return off + 2;
}

int debug_call_instruction(Bytecode* bytecode, int off) {
    uint8_t fn_low = bytecode->code[off + 1];
    uint8_t fn_high = bytecode->code[off + 2];

    printf("OP_CALL ");
    printf("%04d '", ((fn_high >> 8) | fn_low));
    printf("'\n");
    return off + 3;
}

int debug_jmp_instruction(Bytecode* bytecode, int off) {
    uint8_t fn_low = bytecode->code[off + 1];
    uint8_t fn_high = bytecode->code[off + 2];

    printf("OP_JMP ");
    printf("%04d '", ((fn_high >> 8) | fn_low));
    printf("'\n");
    return off + 3;
}

int debug_if_instruction(Bytecode* bytecode, int off) {
    uint8_t fn_low = bytecode->code[off + 1];
    uint8_t fn_high = bytecode->code[off + 2];

    printf("OP_BNQ ");
    printf("%04d '", ((fn_high >> 8) | fn_low));
    printf("'\n");
    return off + 3;
}

void debug_disassemble_stack(Value* stack, Value* top) {
    if (stack != top) {
        printf("stack: ");
        for (Value* i = stack; i < top; i++) {
            printf ("[ ");
            value_print(*i, false);
            printf(" (%d) ", i->type);
            printf(" ]");
        }
        printf("\n");
    }
}