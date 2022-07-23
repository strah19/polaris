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

void Scope::add(const std::string& name, DefinitionType type) {
    definitions[name] = type;
}

bool Scope::in_scope(const std::string& name) {
    return (definitions.find(name) != definitions.end());
}

bool Scope::in_any(const std::string& name) {
    Scope* current = this;
    while (current) {
        if (current->in_scope(name)) return true;
        current = current->previous;
    }
    return false;
}

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

    current_scope = &main_scope;
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

void Parser::parser_warning(Token* token, const char* msg) {
    if (show_warnings) {
        if (filepath)
            report_warning("In file '%s', near '%.*s' on line %d, '%s'.\n", filepath, token->size, token->start, token->line, msg);
        else 
            report_warning("Near '%.*s' on line %d, '%s'.\n", token->size, token->start, token->line, msg);
    }
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
        if (peek()->type == T_IDENTIFIER && (peek(1)->type == T_COLON || peek(1)->type == T_COLON_EQUAL)) {
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
    if (match(T_LCURLY))  return parse_scope();
    else if (match(T_IF)) return parse_if();
    return parse_expression_statement();
}

Ast_Scope* Parser::parse_scope() {
    Ast_Scope* scope = AST_NEW(Ast_Scope);

    Scope* previous_scope = current_scope;
    current_scope = new Scope;
    current_scope->previous = previous_scope;

    while (!check(T_RCURLY) && !is_end()) {
        scope->declerations.push_back(parse_decleration());
    }

    delete current_scope;
    current_scope = previous_scope;

    consume(T_RCURLY, "Expected '}' to end scope");
    return scope;
}

Ast_ExpressionStatement* Parser::parse_expression_statement() {
    parser_warning(peek(), "Expression result is unused");
    auto expression = parse_expression();
    consume(T_SEMICOLON, "Expected ';' after expression statement");
    return AST_NEW(Ast_ExpressionStatement, expression);
}

Ast_Function* Parser::parse_function() {
    return nullptr;
}

Ast_VarDecleration* Parser::parse_variable_decleration() {
    consume(T_IDENTIFIER, "Expected an identifier in variable decleration");
    Token* start_token = peek(-1);
    char* id = (char*) start_token->start;
    id[start_token->size] = '\0';

    if (current_scope->in_any(id))
        throw parser_error(peek(-1), "Redecleration of variable");
    current_scope->add(id, DEF_VAR);

    if (match(T_COLON)) {
        AstDataType var_type = parse_type();
        Ast_Expression* expression = nullptr;
        if (match(T_EQUAL))
            expression = parse_expression(PREC_NONE, var_type);
        consume(T_SEMICOLON, "Expected ';' in variable decleration");
        return AST_NEW(Ast_VarDecleration, id, expression, var_type, AST_SPECIFIER_NONE);
    }
    consume(T_COLON_EQUAL, "Expected ':' or ':=' in variable decleration");  
    Ast_Expression* expression = parse_expression();
    consume(T_SEMICOLON, "Expected ';' in variable decleration");
    AstDataType type = search_expression_for_type(start_token, expression);
    return AST_NEW(Ast_VarDecleration, id, expression, type, AST_SPECIFIER_NONE);
}

Ast_IfStatement* Parser::parse_if() {
    return nullptr;
}

Ast_Expression* Parser::parse_expression(Precedence precedence, AstDataType expected_type) {
    Token* left = advance();
    Ast_Expression* expression;
    if (is_unary(left)) expression = parse_unary_expression();
    else if (is_primary(left)) expression = parse_primary_expression(expected_type);
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
    default: throw parser_error(peek(-1), "Expected unary expression");
    }
}

Ast_Expression* Parser::parse_primary_expression(AstDataType expected_type) {
    Ast_PrimaryExpression* primary = AST_NEW(Ast_PrimaryExpression);
    bool check_expected_type = (expected_type == AST_TYPE_NONE) ? false : true;

    switch (peek(-1)->type) {
    case T_INT_CONST: {
        primary->prim_type = AST_PRIM_DATA;
        primary->type_value = AST_TYPE_INT;
        primary->int_const = atoi(peek(-1)->start);
        if (check_expected_type && !is_type(primary, expected_type))
            throw parser_error(peek(-1), "Integer is not allowed in this expression");
        break;
    }
    case T_FLOAT_CONST: {
        primary->prim_type = AST_PRIM_DATA;
        primary->type_value = AST_TYPE_FLOAT;
        primary->float_const = strtof(peek(-1)->start, NULL);
        if (check_expected_type && !is_type(primary, expected_type))
            throw parser_error(peek(-1), "Float is not allowed in this expression");
        break;
    }
    case T_TRUE: {
        primary->prim_type = AST_PRIM_DATA;
        primary->type_value = AST_TYPE_BOOLEAN;
        primary->boolean = true;
        if (check_expected_type && !is_type(primary, expected_type))
            throw parser_error(peek(-1), "Boolean is not allowed in this expression");
        break;
    }
    case T_FALSE: {
        primary->prim_type = AST_PRIM_DATA;
        primary->type_value = AST_TYPE_BOOLEAN;
        primary->boolean = false;
        if (check_expected_type && !is_type(primary, expected_type))
            throw parser_error(peek(-1), "Boolean is not allowed in this expression");
        break;
    }
    case T_LPAR: {
        primary->prim_type = AST_PRIM_NESTED;
        primary->nested = parse_expression();
        consume(T_RPAR, "Expected ')' to close off nested expression");
        break;
    }
    case T_IDENTIFIER: {
        primary->prim_type = AST_PRIM_ID;

        break;
    }
    default: throw parser_error(peek(-1), "Expected a primary expression");
    }

    return primary;
}

Ast_Expression* Parser::parse_binary_expression(Ast_Expression* left) {
    int op = peek(-1)->type;  
    Ast_Expression* right = parse_expression(PRECEDENCE[op]);

    if (right->type == AST_PRIMARY && left->type == AST_PRIMARY)
        check_types(AST_CAST(Ast_PrimaryExpression, left), AST_CAST(Ast_PrimaryExpression, right));

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
    default: throw parser_error(peek(-1), "Expected binary type operator");
    }
}

void Parser::check_types(Ast_PrimaryExpression* left, Ast_PrimaryExpression* right) {
    if ((left->prim_type == AST_PRIM_DATA && right->prim_type == AST_PRIM_DATA) ||
        (left->prim_type == AST_PRIM_ID   && right->prim_type == AST_PRIM_ID)) {
        if (left->type_value == right->type_value) return;
        if (!check_either(left, right, AST_TYPE_INT))
            throw parser_error(peek(-1), "Type does not match with integer");
        else if (!check_either(left, right, AST_TYPE_BOOLEAN))
            throw parser_error(peek(-1), "Type does not match with boolean");
        else if (!check_either(left, right, AST_TYPE_FLOAT))
            throw parser_error(peek(-1), "Type does not match with float");
    }
}

bool Parser::check_either(Ast_PrimaryExpression* left, Ast_PrimaryExpression* right, AstDataType type) {
    return ((is_type(left, type) && is_type(right, type)) || (!is_type(left, type) && !is_type(right, type)));
}

bool Parser::ignore_type(Ast_PrimaryExpression* left, Ast_PrimaryExpression* right, AstDataType type) {
    return (is_type(left, type) || is_type(right, type));
}

bool Parser::is_type(Ast_PrimaryExpression* prim, AstDataType type) {
    return (prim->type_value == type);
}

AstDataType Parser::parse_type() {
    AstDataType var_type = AST_TYPE_NONE;

    switch (peek()->type) {
    case T_INT:     var_type = AST_TYPE_INT;     break;
    case T_FLOAT:   var_type = AST_TYPE_FLOAT;   break;
    case T_BOOLEAN: var_type = AST_TYPE_BOOLEAN; break;
    default: throw parser_error(peek(), "Unknown type");
    }
    match(peek()->type);
    return var_type;
}

AstDataType Parser::search_expression_for_type(Token* token, Ast_Expression* expression) {
    switch (expression->type) {
    case AST_UNARY: {
        return search_expression_for_type(token, AST_CAST(Ast_UnaryExpression, expression)->next);
    }
    case AST_BINARY: {
        Ast_BinaryExpression* binary = AST_CAST(Ast_BinaryExpression, expression);
        AstDataType type = search_expression_for_type(token, binary->left);
        if (type == AST_TYPE_NONE) {
            type = search_expression_for_type(token, binary->right);
            if (type == AST_TYPE_NONE)
                break;
        }
        return type;
    }
    case AST_PRIMARY: {
        Ast_PrimaryExpression* primary = AST_CAST(Ast_PrimaryExpression, expression);
        return (primary->prim_type == AST_PRIM_DATA || primary->prim_type == AST_PRIM_ID) ? primary->type_value : AST_TYPE_NONE;
    }
    default: break;
    }
    throw parser_error(token, "Unknown type found in expression");
}

bool Parser::is_unary(Token* token) {
    return (token->type == T_MINUS || token->type == T_EXCLAMATION || token->type == T_NOT);
}

bool Parser::is_primary(Token* token) {
    return (token->type == T_INT_CONST || token->type == T_FLOAT_CONST || token->type == T_LPAR || 
            token->type == T_TRUE || token->type == T_FALSE || token->type == T_IDENTIFIER);
}