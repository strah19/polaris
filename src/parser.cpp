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

static Precedence PRECEDENCE [T_OK] = { PREC_PRIMARY };

void Scope::add(const String& name, const Symbol& sym) {
    definitions[name] = sym;
}

bool Scope::in_scope(const String& name) {
    return (definitions.find(name) != definitions.end());
}

Symbol Scope::get(const String& name) {
    Scope* current = this;
    while (current) {
        if (current->in_scope(name)) return current->definitions[name];
        current = current->previous;
    }
    return { AST_TYPE_NONE };
}

bool Scope::in_any(const String& name) {
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

    PRECEDENCE[T_PLUS]          = PREC_TERM;
    PRECEDENCE[T_MINUS]         = PREC_TERM;
    PRECEDENCE[T_STAR]          = PREC_FACTOR;
    PRECEDENCE[T_SLASH]         = PREC_FACTOR;
    PRECEDENCE[T_PERCENT]       = PREC_FACTOR;
    PRECEDENCE[T_COMPARE_EQUAL] = PREC_EQUAL;
    PRECEDENCE[T_NOT_EQUAL]     = PREC_EQUAL;
    PRECEDENCE[T_LARROW]        = PREC_COMPARE;
    PRECEDENCE[T_RARROW]        = PREC_COMPARE;
    PRECEDENCE[T_LTE]           = PREC_COMPARE;
    PRECEDENCE[T_GTE]           = PREC_COMPARE;
    PRECEDENCE[T_AND]           = PREC_AND;
    PRECEDENCE[T_OR]            = PREC_OR;
    PRECEDENCE[T_AMPERSAND]     = PREC_BITWISE_AND;
    PRECEDENCE[T_LINE]          = PREC_BITWISE_OR;
    PRECEDENCE[T_CARET]         = PREC_BITWISE_XOR;
    PRECEDENCE[T_LSHIFT]        = PREC_BITWISE_SHIFT;
    PRECEDENCE[T_RSHIFT]        = PREC_BITWISE_SHIFT;
    PRECEDENCE[T_EQUAL]         = PREC_ASSIGNMENT;
    PRECEDENCE[T_PLUS_EQUAL]    = PREC_ASSIGNMENT;
    PRECEDENCE[T_MINUS_EQUAL]   = PREC_ASSIGNMENT;
    PRECEDENCE[T_STAR_EQUAL]    = PREC_ASSIGNMENT;
    PRECEDENCE[T_SLASH_EQUAL]   = PREC_ASSIGNMENT;

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
    auto expression = parse_expression();
    if (expression->type != AST_ASSIGNMENT)
        parser_warning(peek(), "Expression result is unused");
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

    if (match(T_COLON)) {
        AstDataType var_type = parse_type();
        Ast_Expression* expression = nullptr;
        if (match(T_EQUAL)) {
            expression = parse_expression();
            AstDataType found_type = search_expression_for_type(start_token, expression);
            if (found_type != var_type) 
                throw parser_error(start_token, "Expression does not match type in decleration");
        }
        consume(T_SEMICOLON, "Expected ';' in variable decleration");
        current_scope->add(id, { var_type });
        return AST_NEW(Ast_VarDecleration, id, expression, var_type, AST_SPECIFIER_NONE);
    }
    consume(T_COLON_EQUAL, "Expected ':' or ':=' in variable decleration");  
    Ast_Expression* expression = parse_expression();
    consume(T_SEMICOLON, "Expected ';' in variable decleration");
    AstDataType var_type = search_expression_for_type(start_token, expression);
    current_scope->add(id, { var_type });
    return AST_NEW(Ast_VarDecleration, id, expression, var_type, AST_SPECIFIER_NONE);
}

Ast_IfStatement* Parser::parse_if() {
    Ast_Expression* conditional = parse_expression();
    consume(T_LCURLY, "Expected '{' in if statement");
    Ast_Scope* scope = parse_scope();
    return AST_NEW(Ast_IfStatement, conditional, scope);
}

Ast_Expression* Parser::parse_expression(Precedence precedence) {
    Token* left = advance();
    Ast_Expression* expression;
    if (is_unary(left)) expression = parse_unary_expression();
    else if (is_primary(left)) expression = parse_primary_expression();
    else throw parser_error(left, "Expected unary or primary expression"); 

   while (peek()->type != T_EOF && precedence < PRECEDENCE[peek()->type]) {
        advance();
        if (is_equal(peek(-1))) expression = parse_assignment_expression(expression, convert_to_equal(peek(-1)->type));
        else expression = parse_binary_expression(expression);
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

Ast_Expression* Parser::parse_assignment_expression(Ast_Expression* expression, AstEqualType equal) {
    Ast_Expression* assign = parse_expression(PREC_ASSIGNMENT);
    if (is_equal(peek()) && AST_CAST(Ast_PrimaryExpression, assign)->prim_type != AST_PRIM_ID)
        throw parser_error(peek(-2), "Lvalue required as left operand of assignment");     
    check_types(search_expression_for_type(peek(-1), assign), search_expression_for_type(peek(-1), expression));

    return AST_NEW(Ast_Assignment, assign, expression, equal);
}

Ast_Expression* Parser::parse_primary_expression() {
    Ast_PrimaryExpression* primary = AST_NEW(Ast_PrimaryExpression);

    switch (peek(-1)->type) {
    case T_INT_CONST: {
        primary->prim_type = AST_PRIM_DATA;
        primary->type_value = AST_TYPE_INT;
        primary->int_const = atoi(peek(-1)->start);
        break;
    }
    case T_FLOAT_CONST: {
        primary->prim_type = AST_PRIM_DATA;
        primary->type_value = AST_TYPE_FLOAT;
        primary->float_const = strtof(peek(-1)->start, NULL);
        break;
    }
    case T_TRUE: {
        primary->prim_type = AST_PRIM_DATA;
        primary->type_value = AST_TYPE_BOOLEAN;
        primary->boolean = true;
        break;
    }
    case T_FALSE: {
        primary->prim_type = AST_PRIM_DATA;
        primary->type_value = AST_TYPE_BOOLEAN;
        primary->boolean = false;
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
        char* id = (char*) peek(-1)->start;
        id[peek(-1)->size] = '\0';
        Symbol symbol = current_scope->get(std::string(id));

        if (symbol.type == AST_TYPE_NONE) 
            throw parser_error(peek(-1), "Undefined variable");

        primary->type_value = symbol.type;
        primary->ident = (const char*) id;
        break;
    }
    default: throw parser_error(peek(-1), "Expected a primary expression");
    }

    return primary;
}

Ast_Expression* Parser::parse_binary_expression(Ast_Expression* left) {
    TokenType op = peek(-1)->type;  
    Ast_Expression* right = parse_expression(PRECEDENCE[op]);

    check_types(search_expression_for_type(peek(-1), left), search_expression_for_type(peek(-1), right), convert_to_op(op));

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

void Parser::check_types(AstDataType left, AstDataType right, AstOperatorType op) {
    if (op == AST_OPERATOR_AND || op == AST_OPERATOR_OR) return;
    if (left == right) return;
    if (!check_either(left, right, AST_TYPE_INT))
        throw parser_error(peek(-1), "Type does not match with integer");
    else if (!check_either(left, right, AST_TYPE_BOOLEAN))
        throw parser_error(peek(-1), "Type does not match with boolean");
    else if (!check_either(left, right, AST_TYPE_FLOAT))
        throw parser_error(peek(-1), "Type does not match with float");
}

bool Parser::check_either(AstDataType left, AstDataType right, AstDataType type) {
    return ((is_type(left, type) && is_type(right, type)) || (!is_type(left, type) && !is_type(right, type)));
}

bool Parser::ignore_type(AstDataType left, AstDataType right, AstDataType type) {
    return (is_type(left, type) || is_type(right, type));
}

bool Parser::is_type(AstDataType prim, AstDataType type) {
    return (prim == type);
}

AstOperatorType Parser::convert_to_op(TokenType type) {
   switch (type) {
    case T_PLUS:          return AST_OPERATOR_ADD;
    case T_MINUS:         return AST_OPERATOR_SUB;
    case T_STAR:          return AST_OPERATOR_MULTIPLICATIVE;
    case T_SLASH:         return AST_OPERATOR_DIVISION;
    case T_PERCENT:       return AST_OPERATOR_MODULO;
    case T_COMPARE_EQUAL: return AST_OPERATOR_COMPARITIVE_EQUAL;
    case T_NOT_EQUAL:     return AST_OPERATOR_COMPARITIVE_NOT_EQUAL;
    case T_LARROW:        return AST_OPERATOR_LT;
    case T_RARROW:        return AST_OPERATOR_GT;
    case T_LTE:           return AST_OPERATOR_LTE;
    case T_GTE:           return AST_OPERATOR_GTE;
    case T_AND:           return AST_OPERATOR_AND;
    case T_OR:            return AST_OPERATOR_OR;
    case T_AMPERSAND:     return AST_OPERATOR_BIT_AND;
    case T_LINE:          return AST_OPERATOR_BIT_OR;
    case T_CARET:         return AST_OPERATOR_BIT_XOR;
    case T_LSHIFT:        return AST_OPERATOR_LSHIFT;
    case T_RSHIFT:        return AST_OPERATOR_RSHIFT;
    default:              return AST_OPERATOR_NONE;
    }
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
        if (primary->prim_type == AST_PRIM_NESTED)
            return search_expression_for_type(token, primary->nested);
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

bool Parser::is_equal(Token* token) {
    return (token->type == T_EQUAL || token->type == T_PLUS_EQUAL || token->type == T_MINUS_EQUAL || 
            token->type == T_STAR_EQUAL || token->type == T_SLASH_EQUAL);
}

AstEqualType Parser::convert_to_equal(TokenType type) {
    switch (type) {
    case T_PLUS_EQUAL:  return AST_EQUAL_PLUS;
    case T_MINUS_EQUAL: return AST_EQUAL_MINUS;
    case T_STAR_EQUAL:  return AST_EQUAL_MULTIPLY;
    case T_SLASH_EQUAL: return AST_EQUAL_DIVIDE;
    default: return AST_EQUAL;
    }
}