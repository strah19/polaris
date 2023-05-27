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
    OP_PRINT,
    OP_SET,
    OP_GET,
    OP_JMP,
    OP_RTS,
    OP_RTS_VALUE,
    OP_FUNC_START,
    OP_FUNC_END,
    OP_CALL,
    OP_RETURN
};

#endif //!OPCODES_H