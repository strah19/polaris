#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "vm.h"

typedef struct {
    Token current;
    Token previous;
    bool error_found;
    bool panic;
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_AND,
    PREC_OR,
    PREC_EQUAL,
    PREC_COMPARE,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_PRIMARY
} Precedence;

extern void init_parser();

extern void advance_parser();

extern void consume(TokenType type, const char* msg);

extern void parser_error_at_current(const char* msg);

extern void parser_error(Token* token, const char* msg);

extern bool errors();

typedef void (*ParseFn)();

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParserRule;

extern void expression();

extern Bytecode* current_compiling_chunk();

extern void set_current_bytecode(Bytecode* chunk);

extern void emit_bytecode(uint8_t code);

extern void emit_return();

extern void append_new_chunk();

#endif // !PARSER_H