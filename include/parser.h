#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "vm.h"

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

typedef struct {
    Token current;
    Token previous;
    bool error_found;
    bool panic;
} Parser;

typedef void (*ParseFn)();

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParserRule;

extern void parser_init();

extern void parser_advance();

extern void parser_consume(TokenType type, const char* msg);

extern void parser_error_at_current(const char* msg);

extern void parser_error(Token* token, const char* msg);

extern bool parser_get_errors();

extern void parser_expression();


#endif // !PARSER_H