#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "vm.h"

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_BITWISE_OR,
    PREC_BITWISE_XOR,
    PREC_BITWISE_AND,
    PREC_EQUAL,
    PREC_COMPARE,
    PREC_BITWISE_SHIFT,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_PRIMARY
} Precedence;

typedef struct {
    Token current;
    Token previous;
    int errors;
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

extern bool parser_match(TokenType type);

extern bool parser_check(TokenType type);

extern void parser_consume(TokenType type, const char* msg);

extern void parser_error_at_current(const char* msg);

extern void parser_error(Token* token, const char* msg);

extern int parser_get_errors();

extern void parse_expression();

extern void parse_decleration();

extern void parse_statement();

extern void parse_print_statement();

extern void parser_synchronize();

#endif // !PARSER_H