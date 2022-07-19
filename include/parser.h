#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <cinttypes>

struct ParserError {
    Token* token = nullptr;

    ParserError() = default;
    ParserError(Token* token) : token(token) { }
};

class Parser {
public:
    Parser(Token* tokens);
    Parser(Token* tokens, const char* filepath);
    ~Parser();
    void parse();

    Token* peek(int index = 0);
    Token* advance();

    ParserError parser_error(Token* token, const char* msg);
    Token*      consume(int type, const char* msg);
    bool        match(int type);
    bool        check(int type);
    bool        is_end();
private:
    Ast* default_ast(Ast* ast);
    void init(Token* tokens, const char* filepath);
    void synchronize();

    Ast_Decleration*         parse_decleration();
    Ast_Statement*           parse_statement();
    Ast_Function*            parse_function();
    Ast_VarDecleration*      parse_variable_decleration();
    Ast_ExpressionStatement* parse_expression_statement();
    Ast_Expression*          parse_expression();
private:
    Token* tokens = nullptr;
    uint32_t current = 0;
    const char* filepath = nullptr;
    Ast_TranslationUnit* unit = nullptr;
};

#endif //!PARSER_H