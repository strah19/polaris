#ifndef OPCODES_H
#define OPCODES_H

typedef enum {
    OP_RETURN,        // 1 byte : opcode
    OP_CONSTANT,      // 2 bytes : opcode, address
    OP_NEGATE,        // 1 byte : opcode  
    OP_PLUS,          // 1 byte : opcode
    OP_MINUS,         // 1 byte : opcode
    OP_MULTIPLY,      // 1 byte : opcode
    OP_DIVIDE         // 1 byte : opcode
} Opcodes;

#endif // !OPCODES_H