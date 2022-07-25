/**
 * @file cpp
 * @author strah19
 * @brief Will break dowm the source code into tokens for the parser.
 * @version 0.1
 * @date 2022-07-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "lexer.h"
#include "error.h"
#include <string.h>
#include <stdio.h>

#define NESTED_COMMENTS 255

Lexer::Lexer(const char* source) {
    current = source;
    start = source;
    line = 1;
}

Tokens Lexer::run() {
    Tokens tokens;
    while (true) {
        tokens.push_back(scan());
        if (tokens.back().type == T_EOF) return tokens;

        if (tokens.back().type == T_ERROR)
            fatal_error("'%.*s' on line %d.\n", tokens.back().size, tokens.back().start, tokens.back().line);
    }
}

void Lexer::log(Tokens& tokens) {
    for (auto& token : tokens) 
        printf("token (%d) '%.*s' on line %d.\n", token.type, token.size, token.start, token.line);
}

Token Lexer::scan() {
    Token error = skip_whitespaces();
    if (error.type != T_OK)
        return error;

    start = current;
    if (is_eof()) return init_token(T_EOF);

    char first = advance();
    if (first == '0' && peek() == 'b') return binary();
    if (is_digit(first)) return number();
    if (is_alpha(first)) return identifier();

    return single_character(first);
}

bool Lexer::is_digit(char c) {
    return (c >= '0' && c <= '9');
}

bool Lexer::is_alpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
}

Token Lexer::number() {
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

Token Lexer::binary() {
    advance();
    while (peek() == '0' || peek() == '1')
        advance();
    return init_token(T_BINARY_CONST);
}

Token Lexer::identifier() {
    while ((is_alpha(peek()) || is_digit(peek())) && !is_eof()) 
        advance();
    return init_token(keywords());
}

Token Lexer::skip_whitespaces() {
    while (true) {
        char c = peek();
        switch (c) {
        case '\t':
        case ' ':
        case '\r':
            advance();
            break;
        case '\n': 
            line++;
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
                        line++;      

                    if (nested > NESTED_COMMENTS)
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

Token Lexer::single_character(char single_character_tokens) {
    switch (single_character_tokens) {
    case '(': return init_token(T_LPAR    );
    case ')': return init_token(T_RPAR    );
    case '{': return init_token(T_LCURLY  );
    case '}': return init_token(T_RCURLY  );
    case '=': return init_token((check('=')) ? T_COMPARE_EQUAL : T_EQUAL);
    case '!': return init_token((check('=')) ? T_NOT_EQUAL     : T_EXCLAMATION);
    case '+': return init_token((check('=')) ? T_PLUS_EQUAL    : T_PLUS);
    case '-': return init_token((check('=')) ? T_MINUS_EQUAL   : T_MINUS);
    case '*': return init_token((check('=')) ? T_STAR_EQUAL    : T_STAR);
    case '/': return init_token((check('=')) ? T_SLASH_EQUAL   : T_SLASH);
    case '|': return init_token(T_LINE    );
    case '&': return init_token(T_AMPERSAND);
    case '#': return init_token(T_POUND   );
    case ';': return init_token(T_SEMICOLON);
    case '<': return init_token((check('=')) ? T_LTE : check('<') ? T_LSHIFT : T_LARROW);
    case '>': return init_token((check('=')) ? T_GTE : check('>') ? T_RSHIFT : T_RARROW);
    case '[': return init_token(T_LBRACKET);
    case ']': return init_token(T_RBRACKET);
    case ':': return init_token((check('=')) ? T_COLON_EQUAL : T_COLON);
    case '%': return init_token(T_PERCENT );
    case '^': return init_token(T_CARET   );
    case '~': return init_token(T_NOT     );
    case '"': return string();
    default:  return error_token("Unknown character's found in lexer");
    }
}

TokenType Lexer::keywords() {
    switch (*start) {
    case 'i': return (match("f",      1) ? T_IF      : 
                     (match("nt",     2) ? T_INT     : T_IDENTIFIER));
    case 'e': return (match("lse",    3) ? T_ELSE    : 
                     (match("lif",    3) ? T_ELIF    : T_IDENTIFIER));
    case 's': return (match("tring",  5) ? T_STRING  : T_IDENTIFIER);
    case 'b': return (match("oolean", 6) ? T_BOOLEAN : 
                     (match("reak",   4) ? T_BREAK   : T_IDENTIFIER));
    case 'f': return (match("loat",   4) ? T_FLOAT   : 
                     (match("or",     2) ? T_FOR     : 
                     (match("unc",    3) ? T_FUNC    :
                     (match("alse",   4) ? T_FALSE   : T_IDENTIFIER))));
    case 'w': return (match("hile",   4) ? T_WHILE   : T_IDENTIFIER);
    case 'r': return (match("eturn",  5) ? T_RETURN  : T_IDENTIFIER);
    case 'c' :return (match("har",    3) ? T_CHAR    : 
                     (match("ast",    3) ? T_CAST    : T_IDENTIFIER));
    case 'a': return (match("nd",     2) ? T_AND     : T_IDENTIFIER);
    case 'o': return (match("r",      1) ? T_OR      : T_IDENTIFIER);
    case 't': return (match("rue",    3) ? T_TRUE    : T_IDENTIFIER);
    case 'p': return (match("rint",   4) ? T_PRINT   : T_IDENTIFIER);
    }

    return T_IDENTIFIER;
}

bool Lexer::match(const char* keyword, int size) {
    if (size + 1 != current - start) return false;
    int i = 1;  // The index starts at one because we already verified the first character in the switch statement in 'keywords'.
    while ((start + i) < current) {
        if (start[i] != keyword[i - 1]) return false;
        i++;
    }
    return true;
}

Token Lexer::string() {
    while (peek() != '"' && !is_eof()) {
        if (peek() == '\n') 
            line++;
        advance();
    }
    if (is_eof()) return error_token("Unterminating string");
    advance();
    return init_token(T_STRING_CONST);
}

Token Lexer::init_token(TokenType token_type) {
    Token token;

    token.type = token_type;
    token.line = line;
    token.start = start;
    token.size = (int) (current - start);

    return token;
}

Token Lexer::error_token(const char* msg) {
    Token token;

    token.type = T_ERROR;
    token.line = line;
    token.start = msg;
    token.size = (int) strlen(msg);

    return token;
}

bool Lexer::is_eof() {
    return (*current == '\0');
}

char Lexer::advance() {
    current++;
    return current[-1];
}

char Lexer::advance_by(int by) {
    char c;
    while (by > 0) {
        c = advance();
        by--;
    }
    return c;
}

char Lexer::peek() {
    return (*current);
}

char Lexer::peek_ahead() {
    if (is_eof()) return '\0'; // Nothing left to get
    return current[1];
}

bool Lexer::check(char expected) {
    if (is_eof()) return false;
    if (peek() != expected) return false;
    advance();
    return true;
}