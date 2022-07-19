/**
 * @file parser.cpp
 * @author strah19
 * @brief 
 * @version 0.1
 * @date 2022-07-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "parser.h"
#include "error.h"

#define AST_NEW(type, ...) \
    static_cast<type*>(default_ast(new type(__VA_ARGS__)))

Ast* Parser::default_ast(Ast* ast) {
    ast->line = peek()->line;
    ast->file = filepath;

    return ast;
}

Parser::Parser(Token* tokens) {
    init(tokens, NULL);
}

Parser::Parser(Token* tokens, const char* filepath) {
    init(tokens, filepath);
}

void Parser::init(Token* tokens, const char* filepath) {
    this->tokens = tokens;
    this->filepath = filepath;
    unit = AST_NEW(Ast_TranslationUnit);
}

Parser::~Parser() {
    AST_DELETE(unit);
}

void Parser::parse() {
    while (!is_end()) {
        auto decleration = parse_decleration();

        if (decleration)
            unit->declerations.push_back(decleration);
    }
}

Token* Parser::peek(int index) {
    return &tokens[current + index];
}

Token* Parser::advance() {
    return ((!is_end()) ? &tokens[current++] : &tokens[current]);
}

ParserError Parser::parser_error(Token* token, const char* msg) {
    if (filepath)
        report_error("In file '%s', near '%.*s' on line %d, '%s'.\n", filepath, token->size, token->start, token->line, msg);
    else 
        report_error("Near '%.*s' on line %d, '%s'.\n", token->size, token->start, token->line, msg);

    return ParserError(token);
}

Token* Parser::consume(int type, const char* msg) {
    if (check(type)) return advance();
    throw parser_error(peek(), msg);
}

bool Parser::match(int type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(int type) {
    return (peek()->type == type);
}

bool Parser::is_end() {
    return (tokens[current].type == T_EOF);
}

void Parser::synchronize() {
    advance();
    while (!is_end()) {
        if (peek(-1)->type == T_SEMICOLON) return;
        advance();
    }   
}

Ast_Decleration* Parser::parse_decleration() {
    try {
        if (peek()->type == T_IDENTIFIER && peek(1)->type == T_COLON) {
            if (peek(2)->type == T_FUNC)
                return parse_function();
            return parse_variable_decleration();
        }
        return parse_statement();
    }
    catch (ParserError error) {
        synchronize();
        return nullptr;
    }
}

Ast_Statement* Parser::parse_statement() {
    return parse_expression_statement();
}

Ast_ExpressionStatement* Parser::parse_expression_statement() {
    auto expression = parse_expression();
    consume(T_SEMICOLON, "Expected ';' after expression statement");
    return AST_NEW(Ast_ExpressionStatement, expression);
}

Ast_Function* Parser::parse_function() {
    return nullptr;
}

Ast_VarDecleration* Parser::parse_variable_decleration() {
    return nullptr;
}

Ast_Expression* Parser::parse_expression() {
    return nullptr;
}