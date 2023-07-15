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

#include "debug.h"
#include "opcodes.h"

static int debug_simple_instruction(const char* name, int off);
static int debug_constant_instruction(Bytecode* bytecode, int off);
static int debug_call_instruction(Bytecode* bytecode, int off);

static int debug_address_opcode(Bytecode* bytecode, const char* name, int off);

static bool is_runtime = false;

void debug_disassemble_bytecode(Bytecode* bytecode, const char* name) {
    printf("----- %s -----\n", name);
    printf("IP: %04d.\n", bytecode->start_address);
    for (int i = 0; i < bytecode->count;) 
        i = debug_disassemble_instruction(bytecode, i, false);
}

int debug_disassemble_instruction(Bytecode* bytecode, int off, bool runtime) {
    is_runtime = runtime;
    printf ("%04d ", off);

    if (off > 0 && bytecode->line[off] == bytecode->line[off - 1])
        printf("   | ");
    else
        printf("%4d ", bytecode->line[off]);

    uint8_t instruction = bytecode->code[off];
    switch (instruction) {
    case OP_HALT:   return debug_simple_instruction("OP_HALT",    off);
    case OP_ADD:    return debug_simple_instruction("OP_ADD",      off);
    case OP_MIN:    return debug_simple_instruction("OP_MINUS",     off);
    case OP_MUL:    return debug_simple_instruction("OP_MULTIPLY",  off);
    case OP_DIV:    return debug_simple_instruction("OP_DIVDE",     off);
    case OP_MOD:    return debug_simple_instruction("OP_MODULO",    off);
    case OP_AND:    return debug_simple_instruction("OP_AND",       off);
    case OP_OR:     return debug_simple_instruction("OP_OR",        off);
    case OP_EQL:    return debug_simple_instruction("OP_EQUAL",     off);
    case OP_NEQ:    return debug_simple_instruction("OP_NOT_EQUAL", off);
    case OP_LT:     return debug_simple_instruction("OP_LT",        off);
    case OP_GT:     return debug_simple_instruction("OP_GT",        off);
    case OP_LTE:    return debug_simple_instruction("OP_LTE",       off);
    case OP_GTE:    return debug_simple_instruction("OP_GTE",       off);
    case OP_NEGATE: return debug_simple_instruction("OP_NEGATE",       off);
    case OP_PRINT:  return debug_simple_instruction("OP_PRINT",     off);
    case OP_GLOAD:  return debug_address_opcode(bytecode, "OP_GLOAD", off);
    case OP_GSTORE: return debug_address_opcode(bytecode, "OP_GSTORE", off);
    case OP_LOAD:   return debug_simple_instruction("OP_LOAD",     off + 1);
    case OP_STORE:  return debug_simple_instruction("OP_STORE",     off + 1);
    case OP_CALL:   return debug_call_instruction(bytecode,     off);
    case OP_RET:    return debug_simple_instruction("OP_RET",     off);
    case OP_RETV:   return debug_simple_instruction("OP_RETV",     off);
    case OP_CONST:  return debug_constant_instruction(bytecode,     off);
    case OP_JMP:    return debug_address_opcode(bytecode, "OP_JMP", off);
    case OP_JMPN:   return debug_address_opcode(bytecode, "OP_JMPN", off);
    case OP_CAST:   return debug_simple_instruction("OP_CAST", off);
    case OP_PUSH:   return debug_simple_instruction("OP_PUSH", off);
    default:
        printf("Unknown opcode %d\n", instruction);
        return off + 1;
    }
}

int debug_simple_instruction(const char* name, int off) {
    printf("%s\n", name);
    return off + 1;
}

int debug_address_opcode(Bytecode* bytecode, const char* name, int off) {
    uint8_t address = bytecode->code[off + 1];
    printf("%s ", name);
    printf("%04d\n", address);
    return off + 2;
}

int debug_constant_instruction(Bytecode* bytecode, int off) {
    uint8_t constant_address = bytecode->code[off + 1];
    printf("OP_CONSTANT ");
    printf("%04d ", constant_address);
    if (is_runtime) value_print(bytecode->constants.values[constant_address], false);
    else printf("\n");
    return off + 2;
}

int debug_call_instruction(Bytecode* bytecode, int off) {
    uint32_t address = bytecode->code[off + 1];
    uint32_t args = bytecode->code[off + 2];

    printf("OP_CALL ");
    printf("%04d %04d\n", address, args);
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