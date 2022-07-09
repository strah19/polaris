#include "lexer.h"

#define MAX_NESTED 255

Lexer init_lexer(const char* source) {
    Lexer lexer;

    lexer.current = source;
    lexer.start = source;
    lexer.line = 1;

    return lexer;
}

Token scan_token(Lexer* lexer) {
    Token error = skip_whitespaces(lexer);
    if (error.type != T_OK)
        return error;

    lexer->start = lexer->current;
    if (is_eof(lexer)) return init_token(T_EOF, lexer);

    char first = advance(lexer);
    if (is_digit(first)) return number(lexer);
    if (is_alpha(first)) return identifier(lexer);

    return single_character(first, lexer);
}

bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

bool is_alpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
}

Token number(Lexer* lexer) {
    while (is_digit(peek(lexer)) && !is_eof(lexer)) 
        advance(lexer);
    
    if (peek(lexer) == '.' && is_digit(peek_ahead(lexer))) {
        advance(lexer);

        while (is_digit(peek(lexer)) && !is_eof(lexer)) 
            advance(lexer);
        return init_token(T_FLOAT_CONST, lexer);
    }
    return init_token(T_INT_CONST, lexer);
}

Token identifier(Lexer* lexer) {
    while ((is_alpha(peek(lexer)) || is_digit(peek(lexer))) && !is_eof(lexer)) 
        advance(lexer);
    return init_token(keywords(lexer), lexer);
}

Token skip_whitespaces(Lexer* lexer) {
    while (true) {
        char c = peek(lexer);
        switch (c) {
        case '\t':
        case ' ':
        case '\r':
            advance(lexer);
            break;
        case '\n': 
            lexer->line++;
            advance(lexer);
            break;
        case '/':
            if (peek_ahead(lexer) == '/') {
                while (peek(lexer) != '\n' && !is_eof(lexer)) {
                    advance(lexer);
                }
            } else 
                return init_token(T_OK, lexer);
            break;
        case '<':
            if (peek_ahead(lexer) == '/') {
                int nested = 1;
                advance_by(2, lexer);
                while (!is_eof(lexer) && nested > 0) {
                    if (peek(lexer) == '/' && peek_ahead(lexer) == '>')
                        nested--;
                    else if (peek(lexer) == '<' && peek_ahead(lexer) == '/')
                        nested++;                              
                    else if (peek(lexer) == '\n')
                        lexer->line++;      

                    if (nested > MAX_NESTED)
                        return error_token("Nested comment has reached limit", lexer);         

                    advance(lexer);
                }
                advance(lexer);
            } else
                return init_token(T_OK, lexer);
            break;
        default:
            return init_token(T_OK, lexer);
        }
    }
}

Token single_character(char single_character_tokens, Lexer* lexer) {
    switch (single_character_tokens) {
    case '(': return init_token(T_LPAR,        lexer);
    case ')': return init_token(T_RPAR,        lexer);
    case '{': return init_token(T_LCURLY,      lexer);
    case '}': return init_token(T_RCURLY,      lexer);
    case '=': return init_token((check('=', lexer)) ? T_COMPARE_EQUAL : T_EQUAL, lexer);
    case '!': return init_token((check('=', lexer)) ? T_NOT_EQUAL : T_EXCLAMATION, lexer);
    case '+': return init_token(T_PLUS,        lexer);
    case '-': return init_token(T_MINUS,       lexer);
    case '*': return init_token(T_STAR,        lexer);
    case '/': return init_token(T_SLASH,       lexer);
    case '|': return init_token(T_LINE,        lexer);
    case '&': return init_token(T_AMPERSAND,   lexer);
    case '#': return init_token(T_POUND,       lexer);
    case ';': return init_token(T_SEMICOLON,   lexer);
    case '<': return init_token((check('=', lexer)) ? T_LTE : T_LARROW, lexer);
    case '>': return init_token((check('=', lexer)) ? T_GTE : T_RARROW, lexer);
    case '[': return init_token(T_LBRACKET,    lexer);
    case ']': return init_token(T_RBRACKET,    lexer);
    case '"': return string(lexer);
    default:  return error_token("Unknown character's found in lexer", lexer);
    }
}

int keywords(Lexer* lexer) {
    switch (*lexer->start) {
    case 'i': {
        return (match("f", 1, lexer) ? T_IF : 
               (match("nt", 2, lexer) ? T_INT : T_IDENTIFIER));
    }
    }

    return T_IDENTIFIER;
}

bool match(const char* keyword, int size, Lexer* lexer) {
    if (size + 1 != lexer->current - lexer->start) return false;
    int i = 1;  // The index starts at one because we already verified the first character in the switch statement in 'keywords'.
    while ((lexer->start + i) < lexer->current) {
        if (lexer->start[i] != keyword[i - 1]) return false;
        i++;
    }
    return true;
}

Token string(Lexer* lexer) {
    while (peek(lexer) != '"' && !is_eof(lexer)) {
        if (peek(lexer) == '\n') 
            lexer->line++;
        advance(lexer);
    }
    if (is_eof(lexer)) return error_token("Unterminating string", lexer);
    advance(lexer);
    return init_token(T_STRING_CONST, lexer);
}

Token init_token(int token_type, Lexer* lexer) {
    Token token;

    token.type = token_type;
    token.line = lexer->line;
    token.start = lexer->start;
    token.size = (int) (lexer->current - lexer->start);

    return token;
}

Token error_token(const char* msg, Lexer* lexer) {
    Token token;

    token.type = T_ERROR;
    token.line = lexer->line;
    token.start = msg;
    token.size = (int) strlen(msg);

    return token;
}

bool is_eof(Lexer* lexer) {
    return (*lexer->current == '\0');
}

char advance(Lexer* lexer) {
    lexer->current++;
    return lexer->current[-1];
}

char advance_by(int by, Lexer* lexer) {
    char c;
    while (by > 0) {
        c = advance(lexer);
        by--;
    }
    return c;
}

char peek(Lexer* lexer) {
    return (*lexer->current);
}

char peek_ahead(Lexer* lexer) {
    if (is_eof(lexer)) return '\0'; // Nothing left to get
    return lexer->current[1];
}

bool check(char expected, Lexer* lexer) {
    if (is_eof(lexer)) return false;
    if (peek(lexer) != expected) return false;
    advance(lexer);
    return true;
}