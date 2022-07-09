#include "debug.h"
#include "opcodes.h"

void disassemble_chunk(BytecodeChunk* chunk, const char* name) {
    printf("----- %s -----\n", name);
    for (int i = 0; i < chunk->count;) {
        i = disassemble_instruction(chunk, i);
    }
}

int disassemble_instruction(BytecodeChunk* chunk, int off) {
    printf ("%04d ", off);

    if (off > 0 && chunk->line[off] == chunk->line[off - 1])
        printf("   | ");
    else
        printf("%4d ", chunk->line[off]);

    uint8_t instruction = chunk->code[off];
    switch (instruction) {
    case OP_RETURN:   return simple_instruction("OP_RETURN",   off);
    case OP_NEGATE:   return simple_instruction("OP_NEGATE",   off);
    case OP_PLUS:     return simple_instruction("OP_PLUS",     off);
    case OP_MINUS:    return simple_instruction("OP_MINUS",    off);
    case OP_MULTIPLY: return simple_instruction("OP_MULTIPLY", off);
    case OP_DIVIDE:   return simple_instruction("OP_DIVDE",    off);
    case OP_CONSTANT: return constant_instruction(chunk,       off);
    default:
        printf("Unknown opcode %d\n", instruction);
        return off + 1;
    }
}

int simple_instruction(const char* name, int off) {
    printf("%s\n", name);
    return off + 1;
}

int constant_instruction(BytecodeChunk* chunk, int off) {
    uint8_t constant_address = chunk->code[off + 1];
    printf("OP_CONSTANT ");
    printf("%04d '", constant_address);
    print_value(chunk->constants.values[constant_address]);
    printf("'\n");
    return off + 2;
}