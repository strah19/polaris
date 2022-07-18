#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

typedef enum {
    // Single character tokens
    T_PLUS, T_MINUS, T_STAR, T_SLASH, T_LPAR, T_RPAR, T_LCURLY, T_RCURLY,
    T_SEMICOLON, T_COLON, T_EXCLAMATION, T_AMPERSAND, T_POUND, T_EQUAL,
    T_LARROW, T_RARROW, T_LINE, T_LBRACKET, T_RBRACKET, T_PERCENT, T_NOT,
    T_CARET,

    // Multi-character tokens
    T_NOT_EQUAL, T_LTE, T_GTE, T_COMPARE_EQUAL, T_LSHIFT, T_RSHIFT,

    // Keywords
    T_IF, T_ELIF, T_ELSE, T_FOR, T_WHILE, T_BREAK, T_RETURN, T_INT, T_BOOLEAN,
    T_CHAR, T_STRING, T_FLOAT, T_FUNC, T_AND, T_OR, T_TRUE, T_FALSE,

    T_IDENTIFIER, T_INT_CONST, T_FLOAT_CONST, T_BINARY_CONST, T_STRING_CONST, T_CHAR_CONST,
    T_EOF, T_ERROR, T_OK
} TokenType;

typedef struct {
    const char* start;
    const char* current;
    int line;
} Lexer;

typedef struct {
    int type;

    const char* start;
    int line;
    int size;
} Token;

extern void lexer_init(const char* source);

extern Token lexer_scan();

#endif // !LEXER_H