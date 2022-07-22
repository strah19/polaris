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
#include <stdlib.h>

#define AST_NEW(type, ...) \
    static_cast<type*>(default_ast(new type(__VA_ARGS__)))

static Precedence PRECEDENCE [T_OK] = {
    PREC_PRIMARY
};

Ast* Parser::default_ast(Ast* ast) {
    ast->line = peek()->line;
    ast->file = filepath;

    PRECEDENCE[T_PLUS] =          PREC_TERM;
    PRECEDENCE[T_MINUS] =         PREC_TERM;
    PRECEDENCE[T_STAR] =          PREC_FACTOR;
    PRECEDENCE[T_SLASH] =         PREC_FACTOR;
    PRECEDENCE[T_PERCENT] =       PREC_FACTOR;
    PRECEDENCE[T_COMPARE_EQUAL] = PREC_EQUAL;
    PRECEDENCE[T_NOT_EQUAL] =     PREC_EQUAL;
    PRECEDENCE[T_LARROW] =        PREC_COMPARE;
    PRECEDENCE[T_RARROW] =        PREC_COMPARE;
    PRECEDENCE[T_LTE] =           PREC_COMPARE;
    PRECEDENCE[T_GTE] =           PREC_COMPARE;
    PRECEDENCE[T_AND] =           PREC_AND;
    PRECEDENCE[T_OR] =            PREC_OR;
    PRECEDENCE[T_AMPERSAND] =     PREC_BITWISE_AND;
    PRECEDENCE[T_LINE] =          PREC_BITWISE_OR;
    PRECEDENCE[T_CARET] =         PREC_BITWISE_XOR;
    PRECEDENCE[T_LSHIFT] =        PREC_BITWISE_SHIFT;
    PRECEDENCE[T_RSHIFT] =        PREC_BITWISE_SHIFT;

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
    delete unit;
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
    errors = true;
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

Ast_Expression* Parser::parse_expression(Precedence precedence) {
    Token* left = advance();
    Ast_Expression* expression;
    if (is_unary(left)) expression = parse_unary_expression();
    else if (is_primary(left)) expression = parse_primary_expression();
    else throw parser_error(left, "Expected unary or primary expression"); 

   while (peek()->type != T_EOF && precedence < PRECEDENCE[peek()->type]) {
        advance();
        expression = parse_binary_expression(expression);
    }
    return expression;
}

Ast_Expression* Parser::parse_unary_expression() {
    int op = peek(-1)->type;
    Ast_Expression* expression = parse_expression();

    switch (op) {
    case T_MINUS:       return AST_NEW(Ast_UnaryExpression, expression, AST_UNARY_MINUS);
    case T_EXCLAMATION: return AST_NEW(Ast_UnaryExpression, expression, AST_UNARY_NOT);
    case T_NOT:         return AST_NEW(Ast_UnaryExpression, expression, AST_UNARY_BIT_NOT);
    }
    throw parser_error(peek(-1), "Expected unary expression");
}

Ast_Expression* Parser::parse_primary_expression() {
    Ast_PrimaryExpression* primary = AST_NEW(Ast_PrimaryExpression);

    switch (peek(-1)->type) {
    case T_INT_CONST: {
        primary->type_value = AST_TYPE_INT;
        primary->int_const = atoi(peek(-1)->start);
        break;
    }
    case T_FLOAT_CONST: {
        primary->type_value = AST_TYPE_FLOAT;
        primary->float_const - strtof(peek(-1)->start, NULL);
        break;
    }
    case T_LPAR: {
        ("\n");
        primary->type_value = AST_TYPE_NESTED;
        primary->nested = parse_expression();
        consume(T_RPAR, "Expected ')' to close off nested expression");
        break;
    }
    }

    return primary;
}

Ast_Expression* Parser::parse_binary_expression(Ast_Expression* left) {
    int op = peek(-1)->type;  
    Ast_Expression* right = parse_expression(PRECEDENCE[op]);

    switch (op) {
    case T_PLUS:          return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_ADD, right);
    case T_MINUS:         return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_SUB, right);
    case T_STAR:          return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_MULTIPLICATIVE, right);
    case T_SLASH:         return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_DIVISION, right);
    case T_PERCENT:       return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_MODULO, right);
    case T_COMPARE_EQUAL: return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_COMPARITIVE_EQUAL, right);
    case T_NOT_EQUAL:     return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_COMPARITIVE_NOT_EQUAL, right);
    case T_LARROW:        return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_LT, right);
    case T_RARROW:        return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_GT, right);
    case T_LTE:           return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_LTE, right);
    case T_GTE:           return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_GTE, right);
    case T_AND:           return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_AND, right);
    case T_OR:            return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_OR, right);
    case T_AMPERSAND:     return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_BIT_AND, right);
    case T_LINE:          return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_BIT_OR, right);
    case T_CARET:         return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_BIT_XOR, right);
    case T_LSHIFT:        return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_LSHIFT, right);
    case T_RSHIFT:        return AST_NEW(Ast_BinaryExpression, left, AST_OPERATOR_RSHIFT, right);
    }
    throw parser_error(peek(-1), "Expected binary type operator");
}

bool Parser::is_unary(Token* token) {
    return (token->type == T_MINUS || token->type == T_EXCLAMATION || token->type == T_NOT);
}

bool Parser::is_primary(Token* token) {
    return (token->type == T_INT_CONST || token->type == T_FLOAT_CONST || token->type == T_LPAR);
}