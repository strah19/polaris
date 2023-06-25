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

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "sym_table.h"
#include "ast.h"
#include "scope.h"
#include <cinttypes>

enum Precedence {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_BITWISE_OR,
    PREC_BITWISE_XOR,
    PREC_BITWISE_AND,
    PREC_EQUAL,
    PREC_COMPARE,
    PREC_BITWISE_SHIFT,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_PRIMARY
};

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
    Ast_TranslationUnit* get_unit() { return unit; }

    Token* peek(int index = 0);
    Token* advance();

    //Error functions
    ParserError parser_error(Token* token, const char* msg);
    void        parser_warning(Token* token, const char* msg);

    //Utility parser functions
    Token*      consume(int type, const char* msg);
    bool        match(int type);
    bool        check(int type);
    bool        is_end();
    bool        has_errors() { return errors; }
private:
    Ast* default_ast(Ast* ast);
    void init(Token* tokens, const char* filepath);
    void synchronize();

    Ast_Decleration*            parse_decleration();
    Ast_Statement*              parse_statement();
    Ast_Function*               parse_function();
    Ast_VarDecleration*         parse_variable_decleration();
    Ast_ExpressionStatement*    parse_expression_statement();
    Ast_PrintStatement*         parse_print_statement();
    Ast_Scope*                  parse_scope(bool check_for_return = true);
    Ast_Scope*                  parse_function_scope(bool return_needed, Ast_FunctionArgument* args);
    Ast_IfStatement*            parse_if();
    Ast_ElifStatement*          parse_elif();
    Ast_ElseStatement*          parse_else();
    Ast_WhileStatement*         parse_while();
    Ast_Expression*             parse_expression(Precedence precedence = PREC_NONE);
    AstDataType                 parse_type();
    AstSpecifierType            parser_specifier();
    Ast_ReturnStatement*        parse_return();
    const char*                 parse_identifier(const char* error_msg);
    Ast_FunctionArgument parse_function_arguments(SymbolDefinition* sym);

    Ast_Expression* parse_assignment_expression(Ast_Expression* expression, AstEqualType equal);
    Ast_Expression* parse_binary_expression(Ast_Expression* left);
    Ast_Expression* parse_unary_expression();
    Ast_Expression* parse_primary_expression();

    bool is_unary(Token* token);
    bool is_primary(Token* token);
    bool is_equal(Token* token);

    AstEqualType    convert_to_equal(TokenType type);
    AstOperatorType convert_to_op(TokenType type);

    void check_expression_for_default_args(Token* token, Ast_Expression* expression);
private:
    const char* filepath = nullptr;
    Token* tokens = nullptr;
    uint32_t current = 0;
    Ast_TranslationUnit* unit = nullptr;

    Vector<String> locals;

    bool errors = false;
    bool return_warning_enabled = true;
    bool end_non_void_function_warning_enabled = false;

    //Used to track the return type for the current function being parsed
    AstDataType current_function_return = AST_TYPE_VOID;

    Scope* current_scope;
    Scope main_scope;
};

#endif //!PARSER_H