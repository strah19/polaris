#ifndef LEXER_H
#define LEXER_H

#include <vector>

struct Token;
using Tokens = std::vector<Token>;

enum TokenType {
    // Single character tokens
    T_PLUS, T_MINUS, T_STAR, T_SLASH, T_LPAR, T_RPAR, T_LCURLY, T_RCURLY,
    T_SEMICOLON, T_COLON, T_EXCLAMATION, T_AMPERSAND, T_POUND, T_EQUAL,
    T_LARROW, T_RARROW, T_LINE, T_LBRACKET, T_RBRACKET, T_PERCENT, T_NOT,
    T_CARET,

    // Multi-character tokens
    T_NOT_EQUAL, T_LTE, T_GTE, T_COMPARE_EQUAL, T_LSHIFT, T_RSHIFT,

    // Keywords
    T_IF, T_ELIF, T_ELSE, T_FOR, T_WHILE, T_BREAK, T_RETURN, T_INT, T_BOOLEAN,
    T_CHAR, T_STRING, T_FLOAT, T_FUNC, T_AND, T_OR, T_TRUE, T_FALSE, T_PRINT,

    T_IDENTIFIER, T_INT_CONST, T_FLOAT_CONST, T_BINARY_CONST, T_STRING_CONST, T_CHAR_CONST,
    T_EOF, T_ERROR, T_OK
};

class Lexer {
public:
    Lexer(const char* source);
    Tokens run();
    static void log(Tokens& tokens);

    inline const int lines() const { return line; }
private:
    Token scan();
    Token init_token(int token_type);
    Token error_token(const char* msg);

    char  advance();
    char  advance_by(int by);
    char  peek();
    bool  check(char expected);
    char  peek_ahead();
    bool  match(const char* keyword, int size);

    bool  is_eof();
    bool  is_digit(char c);
    bool  is_alpha(char c);

    Token single_character(char single_character_tokens);
    int   keywords();
    Token skip_whitespaces();

    Token string();
    Token identifier();
    Token number();
    Token binary();
private:
    const char* start;
    const char* current;
    int line;
};

struct Token {
    int type;

    const char* start;
    int line;
    int size;
};

#endif // !LEXER_H