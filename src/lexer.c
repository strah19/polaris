/**
* @file lexer.c
* @author strah19
* @date July 16, 2022
* @version 1.0
*
* @section LICENSE
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the MIT License as published
* by the Free Software Foundation.
*
* @section DESCRIPTION
*
* Very simple lexer, does one token at a time.
*/

#include "lexer.h"
#include <string.h>
#include <stdio.h>

#define MAX_NESTED 255

static Lexer lexer;

static Token init_token(int token_type);
static Token error_token(const char* msg);
static char  advance();
static char  advance_by(int by);
static char  peek();
static bool  is_eof();
static Token single_character(char single_character_tokens);
static int   keywords();
static bool  check(char expected);
static char  peek_ahead();
static Token skip_whitespaces();
static Token string();
static Token identifier();
static bool  is_digit(char c);
static bool  is_alpha(char c);
static Token number();
static Token binary();
static bool  match(const char* keyword, int size);

void lexer_init(const char* source) {
    lexer.current = source;
    lexer.start = source;
    lexer.line = 1;
}

Token lexer_scan() {
    Token error = skip_whitespaces();
    if (error.type != T_OK)
        return error;

    lexer.start = lexer.current;
    if (is_eof()) return init_token(T_EOF);

    char first = advance(lexer);
    if (first == '0' && peek() == 'b') return binary();
    if (is_digit(first)) return number();
    if (is_alpha(first)) return identifier();

    return single_character(first);
}

bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

bool is_alpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
}

Token number() {
    while (is_digit(peek()) && !is_eof()) 
        advance();
    
    if (peek() == '.' && is_digit(peek_ahead())) {
        advance();

        while (is_digit(peek()) && !is_eof()) 
            advance();
        return init_token(T_FLOAT_CONST);
    }
    return init_token(T_INT_CONST);
}

Token binary() {
    advance();
    while (peek() == '0' || peek() == '1')
        advance();
    return init_token(T_BINARY_CONST);
}

Token identifier() {
    while ((is_alpha(peek()) || is_digit(peek())) && !is_eof()) 
        advance();
    return init_token(keywords());
}

Token skip_whitespaces() {
    while (true) {
        char c = peek();
        switch (c) {
        case '\t':
        case ' ':
        case '\r':
            advance();
            break;
        case '\n': 
            lexer.line++;
            advance();
            break;
        case '/':
            if (peek_ahead() == '/') {
                while (peek() != '\n' && !is_eof()) {
                    advance();
                }
            } else 
                return init_token(T_OK);
            break;
        case '<':
            if (peek_ahead() == '/') {
                int nested = 1;
                advance_by(2);
                while (!is_eof() && nested > 0) {
                    if (peek() == '/' && peek_ahead() == '>')
                        nested--;
                    else if (peek() == '<' && peek_ahead() == '/')
                        nested++;                              
                    else if (peek() == '\n')
                        lexer.line++;      

                    if (nested > MAX_NESTED)
                        return error_token("Nested comment has reached limit");         

                    advance();
                }
                advance();
            } else
                return init_token(T_OK);
            break;
        default:
            return init_token(T_OK);
        }
    }
}

Token single_character(char single_character_tokens) {
    switch (single_character_tokens) {
    case '(': return init_token(T_LPAR    );
    case ')': return init_token(T_RPAR    );
    case '{': return init_token(T_LCURLY  );
    case '}': return init_token(T_RCURLY  );
    case '=': return init_token((check('=')) ? T_COMPARE_EQUAL : T_EQUAL);
    case '!': return init_token((check('=')) ? T_NOT_EQUAL : T_EXCLAMATION);
    case '+': return init_token(T_PLUS    );
    case '-': return init_token(T_MINUS   );
    case '*': return init_token(T_STAR    );
    case '/': return init_token(T_SLASH   );
    case '|': return init_token(T_LINE    );
    case '&': return init_token(T_AMPERSAND);
    case '#': return init_token(T_POUND   );
    case ';': return init_token(T_SEMICOLON);
    case '<': return init_token((check('=')) ? T_LTE : check('<') ? T_LSHIFT : T_LARROW);
    case '>': return init_token((check('=')) ? T_GTE : check('>') ? T_RSHIFT : T_RARROW);
    case '[': return init_token(T_LBRACKET);
    case ']': return init_token(T_RBRACKET);
    case ':': return init_token(T_COLON   );
    case '%': return init_token(T_PERCENT );
    case '^': return init_token(T_CARET   );
    case '~': return init_token(T_NOT     );
    case '"': return string();
    default:  return error_token("Unknown character's found in lexer");
    }
}

int keywords() {
    switch (*lexer.start) {
    case 'i': return (match("f",      1) ? T_IF      : 
                     (match("nt",     2) ? T_INT     : T_IDENTIFIER));
    case 'e': return (match("lse",    3) ? T_ELSE    : 
                     (match("lif",    3) ? T_ELIF    : T_IDENTIFIER));
    case 's': return (match("tring",  5) ? T_STRING  : T_IDENTIFIER);
    case 'b': return (match("oolean", 6) ? T_BOOLEAN : 
                     (match("reak",   4) ? T_BREAK   : T_IDENTIFIER));
    case 'f': return (match("loat",   4) ? T_FLOAT   : 
                     (match("or",     2) ? T_FOR     : 
                     (match("unc",    3) ? T_FUNC    : T_IDENTIFIER)));
    case 'w': return (match("hile",   4) ? T_WHILE   : T_IDENTIFIER);
    case 'r': return (match("eturn",  5) ? T_RETURN  : T_IDENTIFIER);
    case 'c' :return (match("har",    3) ? T_CHAR    : T_IDENTIFIER);
    case 'a': return (match("nd",     2) ? T_AND     : T_IDENTIFIER);
    case 'o': return (match("r",      1) ? T_OR      : T_IDENTIFIER);
    }

    return T_IDENTIFIER;
}

bool match(const char* keyword, int size) {
    if (size + 1 != lexer.current - lexer.start) return false;
    int i = 1;  // The index starts at one because we already verified the first character in the switch statement in 'keywords'.
    while ((lexer.start + i) < lexer.current) {
        if (lexer.start[i] != keyword[i - 1]) return false;
        i++;
    }
    return true;
}

Token string() {
    while (peek() != '"' && !is_eof()) {
        if (peek() == '\n') 
            lexer.line++;
        advance();
    }
    if (is_eof()) return error_token("Unterminating string");
    advance();
    return init_token(T_STRING_CONST);
}

Token init_token(int token_type) {
    Token token;

    token.type = token_type;
    token.line = lexer.line;
    token.start = lexer.start;
    token.size = (int) (lexer.current - lexer.start);

    return token;
}

Token error_token(const char* msg) {
    Token token;

    token.type = T_ERROR;
    token.line = lexer.line;
    token.start = msg;
    token.size = (int) strlen(msg);

    return token;
}

bool is_eof() {
    return (*lexer.current == '\0');
}

char advance() {
    lexer.current++;
    return lexer.current[-1];
}

char advance_by(int by) {
    char c;
    while (by > 0) {
        c = advance();
        by--;
    }
    return c;
}

char peek() {
    return (*lexer.current);
}

char peek_ahead() {
    if (is_eof()) return '\0'; // Nothing left to get
    return lexer.current[1];
}

bool check(char expected) {
    if (is_eof()) return false;
    if (peek() != expected) return false;
    advance();
    return true;
}