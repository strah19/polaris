#ifndef OPCODES_H
#define OPCODES_H

enum Opcode {
    OP_CONST,
    OP_ADD,
    OP_MIN,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EQL,
    OP_NEQ,
    OP_LTE,
    OP_GTE,
    OP_LT,
    OP_GT,
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_BOR,
    OP_BAN,
    OP_LSF,
    OP_RSF,
    OP_NEGATE,
    OP_PRINT,
    OP_STORE,
    OP_GSTORE,
    OP_LOAD,
    OP_GLOAD,
    OP_JMP,
    OP_JMPT,
    OP_JMPN,
    OP_RET,
    OP_CALL,
    OP_HALT
};

#endif //!OPCODES_H