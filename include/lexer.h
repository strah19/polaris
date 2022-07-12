#ifndef LEXER_H
#define LEXER_H

#include "common.h"

typedef enum {
    // Single character tokens
    T_PLUS, T_MINUS, T_STAR, T_SLASH, T_LPAR, T_RPAR, T_LCURLY, T_RCURLY,
    T_SEMICOLON, T_COLON, T_EXCLAMATION, T_AMPERSAND, T_POUND, T_EQUAL,
    T_LARROW, T_RARROW, T_LINE, T_LBRACKET, T_RBRACKET,

    // Multi-character tokens
    T_NOT_EQUAL, T_LTE, T_GTE, T_COMPARE_EQUAL,

    // Keywords
    T_IF, T_ELIF, T_ELSE, T_FOR, T_WHILE, T_BREAK, T_RETURN, T_INT, T_BOOLEAN,
    T_CHAR, T_STRING, T_FLOAT, T_FUNC,

    T_IDENTIFIER, T_INT_CONST, T_FLOAT_CONST, T_STRING_CONST, T_CHAR_CONST,
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

extern void init_lexer(const char* source);

extern Token scan_token();

extern Token init_token(int token_type);

extern Token error_token(const char* msg);

extern char advance();

extern char advance_by(int by);

extern char peek();

extern bool is_eof();

extern Token single_character(char single_character_tokens);

extern int keywords();

extern bool check(char expected);

extern char peek_ahead();

extern Token skip_whitespaces();

extern Token string();

extern Token identifier();

extern bool is_digit(char c);

extern bool is_alpha(char c);

extern Token number();

extern bool match(const char* keyword, int size);

#endif // !LEXER_H