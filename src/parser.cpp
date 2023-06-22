/**
 * Copyright (C) 2023 Strahinja Marinkovic - All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License as
 * published by the Free Software Foundation.
 *
 * You should have received a copy of the MIT License along with
 * this program. If not, see https://opensource.org/license/mit/
 */

#include "parser.h"
#include "error.h"
#include "util.h"
#include "semantic.h"
#include <string>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

#define AST_NEW(type, ...) \
    static_cast<type*>(default_ast(new type(__VA_ARGS__)))

static Precedence PRECEDENCE [T_OK] = { PREC_PRIMARY };

void log_token(Token* token) {
    printf("token (%d) '%.*s' on line %d.\n", token->type, token->size, token->start, token->line);
}

void Scope::add(const char* name, SymbolDefinition defn) {
    Symbol* new_node = enter_symbol(name, defn, &root);
    last = defn;
}

SymbolDefinition Scope::get(const char* name) {
    Scope* current = this;
    while (current) {
        Symbol* node = search_symbol_table(name, current->root);
        if (node) return node->defn;
        current = current->previous;
    }
    return SymbolDefinition();
}

bool Scope::in_any(const char* name) {
    Scope* current = this;
    while (current) {
        if (search_symbol_table(name, current->root)) return true;
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

        if (decleration) {
            unit->declerations.push_back(decleration);
        }
    }

    select_functions();
}

void Parser::select_functions() {
    for (int i = 0; i < unit->declerations.size(); i++) 
        if (unit->declerations[i]->type == AST_FUNCTION)
            function_indices.push_back(i);
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
        if (peek(-1)->type == T_RCURLY)    return;
        advance();
    }   
}

Ast_Decleration* Parser::parse_decleration() {
    try {
        if (peek()->type == T_IDENTIFIER && (peek(1)->type == T_COLON || peek(1)->type == T_COLON_EQUAL)) {
            if (peek(2)->type == T_LPAR && peek(1)->type == T_COLON)
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
    if (match(T_LCURLY))      return parse_scope();
    else if (match(T_IF))     return parse_if();
    else if (match(T_WHILE))  return parse_while();
    else if (match(T_RETURN)) return parse_return();
    else if (match(T_PRINT))  return parse_print_statement();
    else if (match(T_ELIF))   throw parser_error(peek(), "Cannot have 'elif' without an if");
    else if (match(T_ELSE))   throw parser_error(peek(), "Cannot have 'else' without an if");
    return parse_expression_statement();
}

Ast_Scope* Parser::parse_scope() {
    Ast_Scope* scope = AST_NEW(Ast_Scope);

    Scope* previous_scope = current_scope;
    current_scope = new Scope;
    current_scope->previous = previous_scope;

    while (!check(T_RCURLY) && !is_end()) 
        scope->declerations.push_back(parse_decleration());

    delete current_scope;
    current_scope = previous_scope;

    consume(T_RCURLY, "Expected '}' to end scope");
    return scope;
}

Ast_Scope* Parser::parse_function_scope(bool return_needed, Ast_FunctionArgument* args) {
    Ast_Scope* scope = AST_NEW(Ast_Scope);

    Scope* previous_scope = current_scope;
    current_scope = new Scope;
    current_scope->previous = previous_scope;

    return_warning_enabled = true;

    for (int i = 0; i < args->arg_count; i++) {
        if (current_scope->in_any(args->args[i]->ident))
            throw parser_error(peek(), "Redefinition of variable in argument");
        SymbolDefinition sym;
        sym.type = DEF_VAR;
        sym.var.var_type = args->args[i]->type_value;
        sym.var.specifiers = args->args[i]->specifiers;
        current_scope->add(args->args[i]->ident, sym);
    }

    bool expected_return = true;

    while (!check(T_RCURLY) && !is_end()) {
        scope->declerations.push_back(parse_decleration());
        if (scope->declerations.back()->type == AST_RETURN) {
            Ast_ReturnStatement* return_statement = AST_CAST(Ast_ReturnStatement, scope->declerations.back());
            if (return_needed && expected_return)
                expected_return = false;
        }
    }

    if (expected_return && return_needed && return_warning_enabled)
        parser_warning(peek(), "Need return statement in function");

    delete current_scope;
    current_scope = previous_scope;

    consume(T_RCURLY, "Expected '}' to end scope");
    return scope;
}

Ast_ExpressionStatement* Parser::parse_expression_statement() {
    auto expression = parse_expression();
    consume(T_SEMICOLON, "Expected ';' after expression statement");
    return AST_NEW(Ast_ExpressionStatement, expression);
}

Ast_PrintStatement* Parser::parse_print_statement() {
    Vector<Ast_Expression*> expressions;
    expressions.push_back(parse_expression());
    while (match(T_COMMA)) {
        expressions.push_back(parse_expression());
    }
    consume(T_SEMICOLON, "Expected ';' after expression statement");
    return AST_NEW(Ast_PrintStatement, expressions);
}

Ast_Function* Parser::parse_function() {
    Ast_Function* function = AST_NEW(Ast_Function);
    function->ident = parse_identifier("Expected identifier in function decleration");

    SymbolDefinition sym;
    if (current_scope->in_any(function->ident))
        throw parser_error(peek(-1), "Redecleration of function");

    consume(T_COLON, "Expected ':' in function decleration");  
    consume(T_LPAR, "Expected '(' in function decleration");  
    function->args = parse_function_arguments(&sym);
    consume(T_RPAR, "Expected ')' after function arguments");

    if (match(T_POINTER_ARROW)) {
        function->return_type = parse_type();
        sym.func.return_type = function->return_type;
        current_function_return = function->return_type;
    } 
    else
        current_function_return = AST_TYPE_VOID;

    sym.type = DEF_FUN;
    sym.func.function_ptr = function;
    current_scope->add(function->ident, sym);

    consume(T_LCURLY, "Expected '{' before function body");  
    function->scope = parse_function_scope((function->return_type != AST_TYPE_VOID) ? true : false, &function->args);
    
    return function;
}

Ast_FunctionArgument Parser::parse_function_arguments(SymbolDefinition* sym) {
    Ast_FunctionArgument args;
    bool expect_default = false;
    locals.clear();
    int arg_count = 0;
    while (!check(T_RPAR)) {
        const char* id = parse_identifier("Expected identifier in function argument");
        locals.push_back(id);
        consume(T_COLON, "Expected ':' in function argument");  
        AstDataType var_type = parse_type();
        AstSpecifierType spec = parser_specifier();
        sym->func.arg_count++;

        Ast_Expression* expression = nullptr;
        if (match(T_EQUAL)) {
            Token* begin_of_expression = peek();
            expression = parse_expression();
            expect_default = true;
            check_expression_for_default_args(begin_of_expression, expression);
        }
        else if (expect_default) 
            throw parser_error(peek(), "Default value for argument must be at end of function");
        sym->func.default_values[sym->func.arg_count - 1] = expression;

        args.args[args.arg_count++] = AST_NEW(Ast_VarDecleration, id, expression, var_type, spec); 
        if (!check(T_RPAR)) 
            consume(T_COMMA, "Expected ',' between function arguments");
    }
    return args;
}

Ast_ReturnStatement* Parser::parse_return() {
    Ast_Expression* expression = nullptr;
    if (is_unary(peek()) || is_primary(peek()))
        expression = parse_expression();
    consume(T_SEMICOLON, "Expected semicolon at end of return statement");    

    if (!current_scope->previous)
        throw parser_error(peek(), "Return statement may only be in a function");
    return AST_NEW(Ast_ReturnStatement, expression, current_function_return);
}

Ast_VarDecleration* Parser::parse_variable_decleration() {
    const char* id = parse_identifier("Expected identifier in variable decleration");
    Token* start_token = peek(-1);

    if (current_scope->in_any(id))
        throw parser_error(peek(-1), "Redecleration of variable");

    if (match(T_COLON)) {
        AstDataType var_type = parse_type();
        AstSpecifierType spec = parser_specifier();
        Ast_Expression* expression = nullptr;

        if (match(T_EQUAL)) {
            expression = parse_expression();
        }
        consume(T_SEMICOLON, "Expected ';' in variable decleration");
        SymbolDefinition sym;
        sym.type = DEF_VAR;
        sym.var.var_type = var_type;
        sym.var.specifiers = spec;
        current_scope->add(id, sym);
        return AST_NEW(Ast_VarDecleration, id, expression, var_type, AST_SPECIFIER_NONE);
    }
    consume(T_COLON_EQUAL, "Expected ':' or ':=' in variable decleration");  
    Ast_Expression* expression = parse_expression();
    
    AstDataType expr_type = get_expression_type(expression);

    consume(T_SEMICOLON, "Expected ';' in variable decleration");
    SymbolDefinition sym;
    sym.type = DEF_VAR;
    sym.var.var_type = expr_type;
    current_scope->add(id, sym);
    return AST_NEW(Ast_VarDecleration, id, expression, expr_type, AST_SPECIFIER_NONE);
}

const char* Parser::parse_identifier(const char* error_msg) {
    consume(T_IDENTIFIER, error_msg);
    Token* start_token = peek(-1);
    char* id = (char*) start_token->start;
    id[start_token->size] = '\0';
    return (const char*) id;
}

Ast_IfStatement* Parser::parse_if() {
    Ast_Expression* condition = parse_expression();
    consume(T_LCURLY, "Expected '{' in if statement");
    Ast_Scope* scope = parse_scope();

    Ast_IfStatement* if_statement = AST_NEW(Ast_IfStatement, condition, scope);

    Ast_ConditionalStatement* current = if_statement;
    while (match(T_ELIF)) {
        current->next = parse_elif();
        current = current->next;
    }

    if (match(T_ELSE)) 
        current->next = parse_else();

    return if_statement;
}

Ast_ElifStatement* Parser::parse_elif() {
    Ast_Expression* condition = parse_expression();
    consume(T_LCURLY, "Expected '{' in elif statement");
    Ast_Scope* scope = parse_scope();
    return AST_NEW(Ast_ElifStatement, condition, scope);
}

Ast_ElseStatement* Parser::parse_else() {
    if (current_scope->previous->last.type == DEF_FUN) {
        return_warning_enabled = false;
    }

    consume(T_LCURLY, "Expected '{' in else statement");
    Ast_Scope* scope = parse_scope();
    return AST_NEW(Ast_ElseStatement, scope);
}

Ast_WhileStatement* Parser::parse_while() {
    Ast_Expression* condition = parse_expression();
    consume(T_LCURLY, "Expected '{' in while statement");
    Ast_Scope* scope = parse_scope();
    return AST_NEW(Ast_WhileStatement, condition, scope);
}

//Pratt Parsing :)
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
    if (expression->type == AST_PRIMARY) {
        auto id = AST_CAST(Ast_PrimaryExpression, expression);
        if (id->prim_type != AST_PRIM_ID)
            throw parser_error(peek(-1), "Expected Lvalue in assignment");

        SymbolDefinition sym = current_scope->get(id->ident);
        if ((sym.var.specifiers & AST_SPECIFIER_CONST))
            throw parser_error(peek(-1), "Identifier is a constant, it cannot be modified");

        auto value = parse_expression(PREC_ASSIGNMENT);
        return AST_NEW(Ast_Assignment, id, value, nullptr);
    }
    else if (expression->type == AST_ASSIGNMENT) {
        auto past_assign = AST_CAST(Ast_Assignment, expression);
        if (!IS_AST(past_assign->value, AST_PRIMARY) || AST_CAST(Ast_PrimaryExpression, past_assign->value)->prim_type != AST_PRIM_ID)
            throw parser_error(peek(-1), "Expected an identifier in assignment expression");
        auto id = AST_CAST(Ast_PrimaryExpression, past_assign->value);

        SymbolDefinition sym = current_scope->get(id->ident);
        if ((sym.var.specifiers & AST_SPECIFIER_CONST))
            throw parser_error(peek(-1), "Identifier is a constant, it cannot be modified");

        auto value = parse_expression(PREC_ASSIGNMENT);
        return AST_NEW(Ast_Assignment, id, value, past_assign);
    }
    else {
        throw parser_error(peek(-1), "Expected an assignemnt expression");
    }

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
    case T_BINARY_CONST: {
        primary->prim_type = AST_PRIM_DATA;
        primary->type_value = AST_TYPE_INT;
        primary->int_const = std::stoi(peek(-1)->start + 2, 0, 2);
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
        char* id = create_string((char*) peek(-1)->start, peek(-1)->size);
        SymbolDefinition symbol = current_scope->get(id);

        if (symbol.type == DEF_VAR) {
            primary->prim_type = AST_PRIM_ID;
            primary->type_value = symbol.var.var_type;
            primary->ident = (const char*) id;
            //Marks the use of a variable if it is local to the function it is in.
            for (int i = 0; i < locals.size(); i++) {
                if (strcmp(locals[i].c_str(), id) == 0) {
                    primary->local = true;
                    primary->local_index = i;
                }
            }
        }
        else if (symbol.type == DEF_FUN) {
            primary->prim_type = AST_PRIM_CALL;
            primary->call = new Ast_FunctionCall();
            primary->call->ident = (const char*) id;
            primary->call->func_ptr = symbol.func.function_ptr;
            
            consume(T_LPAR, "Expected '(' in function call");

            int arg_count = 0;
            while (!check(T_RPAR)) {
                primary->call->args[arg_count++] = parse_expression();
                if (!check(T_RPAR)) 
                    consume(T_COMMA, "Expected ',' between function arguments");
            }

            if (arg_count != symbol.func.arg_count) {
                if (arg_count > symbol.func.arg_count)
                    throw parser_error(peek(), "Too many arguments in function call");
                else {
                    int i = arg_count;
                    while (arg_count < symbol.func.arg_count) {
                        if (symbol.func.default_values[i]) {
                            primary->call->args[arg_count++] = symbol.func.default_values[i];
                        }
                        else 
                            throw parser_error(peek(), "Expected argument in function call");
                        i++;
                    }

                    if (arg_count != symbol.func.arg_count)
                        throw parser_error(peek(), "Not enough arguments in function call");
                }
            }
            consume(T_RPAR, "Expected ')' in function call");
            primary->call->arg_count = arg_count;
            primary->type_value = symbol.func.return_type;
        }
        else {
            throw parser_error(peek(-1), "Undefined symbol");
        }
        break;
    }
    case T_STRING_CONST: {
        char* str = (char*) peek(-1)->start;
        str[peek(-1)->size] = '\0';       
        primary->string = str; 
        primary->prim_type = AST_PRIM_DATA;
        primary->type_value = AST_TYPE_STRING;
        break;
    }
    case T_CHAR_CONST: {
        primary->char_const = *peek(-1)->start;
        primary->prim_type = AST_PRIM_DATA;
        primary->type_value = AST_TYPE_CHAR;
        break;
    }
    case T_CAST: {
        primary->prim_type = AST_PRIM_CAST;
        consume(T_LARROW, "Expected '<' after 'cast' keyword");
        primary->cast.cast_type = parse_type();
        consume(T_RARROW, "Expected '>' after type in 'cast' expression");
        consume(T_LPAR, "Expected '(' in 'cast' expression");
        primary->cast.expression = parse_expression();
        consume(T_RPAR, "Expected ')' in 'cast' expression");
        break;
    }
    default: throw parser_error(peek(-1), "Expected a primary expression");
    }

    return primary;
}

Ast_Expression* Parser::parse_binary_expression(Ast_Expression* left) {
    TokenType op = peek(-1)->type;  
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
    default: throw parser_error(peek(-1), "Expected binary type operator");
    }
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
    case T_STRING:  var_type = AST_TYPE_STRING;  break;
    case T_CHAR:    var_type = AST_TYPE_CHAR;    break;
    default: throw parser_error(peek(), "Unknown type");
    }
    match(peek()->type);

    return var_type;
}

AstSpecifierType Parser::parser_specifier() {
    if (match(T_CONSTANT)) return AST_SPECIFIER_CONST;
    return AST_SPECIFIER_NONE;
}

void Parser::check_expression_for_default_args(Token* token, Ast_Expression* expression) {
    switch (expression->type) {
    case AST_UNARY: {
        check_expression_for_default_args(token, AST_CAST(Ast_UnaryExpression, expression)->next);
        break;
    }
    case AST_BINARY: {
        Ast_BinaryExpression* binary = AST_CAST(Ast_BinaryExpression, expression);
        check_expression_for_default_args(token, binary->left);
        check_expression_for_default_args(token, binary->right);
        break;
    }
    case AST_PRIMARY: {
        Ast_PrimaryExpression* primary = AST_CAST(Ast_PrimaryExpression, expression);
        if (primary->prim_type == AST_PRIM_NESTED)
            return check_expression_for_default_args(token, primary->nested);
        else if (primary->prim_type == AST_PRIM_CAST)
            throw parser_error(token, "Cannot have 'cast' expression in default argument expression");
        else if (primary->prim_type == AST_PRIM_ID)
            throw parser_error(token, "Cannot have identifier in default argument expression");
        break;
    }
    case AST_ASSIGNMENT: throw parser_error(token, "Cannot assign value in a default argument expression");
    default: break;
    }
}

bool Parser::is_unary(Token* token) {
    return (token->type == T_MINUS || token->type == T_EXCLAMATION || token->type == T_NOT);
}

bool Parser::is_primary(Token* token) {
    return (token->type == T_INT_CONST || token->type == T_FLOAT_CONST || token->type == T_LPAR || 
            token->type == T_TRUE || token->type == T_FALSE || token->type == T_IDENTIFIER ||
            token->type == T_CAST || token->type == T_STRING_CONST || token->type == T_BINARY_CONST || 
            token->type == T_CHAR_CONST);
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