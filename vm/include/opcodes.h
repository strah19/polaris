#ifndef OPCODES_H
#define OPCODES_H

typedef enum {
    OP_RETURN,        // 1 byte  : opcode
    OP_CONSTANT,      // 2 bytes : opcode, address
    OP_NEGATE,        // 1 byte  : opcode  
    OP_PLUS,          // 1 byte  : opcode
    OP_MINUS,         // 1 byte  : opcode
    OP_MULTIPLY,      // 1 byte  : opcode
    OP_DIVIDE,        // 1 byte  : opcode
    OP_MODULO,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_LT,
    OP_GT,
    OP_LTE,
    OP_GTE,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_BIT_AND,
    OP_BIT_OR,
    OP_BIT_NOT,
    OP_BIT_XOR,
    OP_LSHIFT,
    OP_RSHIFT,
    OP_PRINT
} Opcodes;

#endif // !OPCODES_H