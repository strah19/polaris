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
    OP_BIN_AND,
    OP_BIN_OR,
    OP_BIN_NOT,
    OP_LSHIFT,
    OP_RSHIFT
} Opcodes;

#endif // !OPCODES_H