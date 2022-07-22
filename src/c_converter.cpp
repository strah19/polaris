/**
 * @file c_converter.cpp
 * @author strah19
 * @brief 
 * @version 0.1
 * @date 2022-07-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include "c_converter.h"
#include "error.h"

#define MAX_FILE_LEN 256
#define MAX_CMD_LEN 512

void Converter::run(Ast_TranslationUnit* unit) {
    open_file();

    fprintf(file, PREAMBLE_BOILERPLATE);
    fprintf(file, MAIN_BOILERPLATE);
    convert_translation_unit(unit);
    fprintf(file, POSTAMBLE_BOILERPLATE);

    fclose(file);
}

void Converter::open_file() {
    char filename_buffer[MAX_FILE_LEN];

    if (!build_directory)
        sprintf(filename_buffer, "%s.c", filename);
    else
        sprintf(filename_buffer, "%s/%s.c", build_directory, objname);
        
    file = fopen(filename_buffer, "w");
    if (!file) fatal_error("Unable to open '%s' file for conversion.\n", objname); 
}

void Converter::convert_translation_unit(Ast_TranslationUnit* unit) {
    for (auto& dec : unit->declerations) {
        switch (dec->type) {
        case AST_EXPRESSION_STATEMENT: convert_expression_statement(AST_CAST(Ast_ExpressionStatement, dec));
        }
    }
}

void Converter::compile() {
    char run_buffer[MAX_CMD_LEN];
    sprintf(run_buffer, "gcc %s.c -o %s", filename, objname);
    system(run_buffer);
}

void Converter::convert_expression_statement(Ast_ExpressionStatement* expression_statment) {
    convert_expression(expression_statment->expression);
    semicolon();
}

void Converter::convert_expression(Ast_Expression* expression) {
    switch (expression->type) {
    case AST_PRIMARY: {
        Ast_PrimaryExpression* primary = AST_CAST(Ast_PrimaryExpression, expression);
        switch (primary->type_value) {
        case AST_TYPE_INT: fprintf(file, "%d", primary->int_const); break;
        case AST_TYPE_NESTED: {
            write("(");
            convert_expression(primary->nested);
            write(")");
            break;
        }
        }
        break;
    }
    case AST_BINARY: {
        Ast_BinaryExpression* binary = AST_CAST(Ast_BinaryExpression, expression);
        convert_expression(binary->left);
        switch (binary->op) {
        case AST_OPERATOR_ADD:                   write("+");  break;
        case AST_OPERATOR_SUB:                   write("-");  break;
        case AST_OPERATOR_MULTIPLICATIVE:        write("*");  break;
        case AST_OPERATOR_DIVISION:              write("/");  break;
        case AST_OPERATOR_MODULO:                write("%");  break;
        case AST_OPERATOR_COMPARITIVE_EQUAL:     write("=="); break;
        case AST_OPERATOR_COMPARITIVE_NOT_EQUAL: write("!="); break;
        case AST_OPERATOR_LT:                    write("<");  break;
        case AST_OPERATOR_GT:                    write(">");  break;
        case AST_OPERATOR_GTE:                   write("<="); break;
        case AST_OPERATOR_LTE:                   write(">="); break;
        case AST_OPERATOR_OR:                    write("||"); break;
        case AST_OPERATOR_AND:                   write("&&"); break;
        case AST_OPERATOR_BIT_AND:               write("&");  break;
        case AST_OPERATOR_BIT_OR:                write("|");  break;
        case AST_OPERATOR_BIT_XOR:               write("^");  break;
        case AST_OPERATOR_LSHIFT:                write("<<"); break;
        case AST_OPERATOR_RSHIFT:                write(">>"); break;
        }
        convert_expression(binary->right);
        break;
    }
    case AST_UNARY: {
        Ast_UnaryExpression* unary = AST_CAST(Ast_UnaryExpression, expression);
        switch (unary->op) {
        case AST_UNARY_MINUS:   write("-"); break;
        case AST_UNARY_BIT_NOT: write("~"); break;
        case AST_UNARY_NOT:     write("!"); break;
        }
        convert_expression(unary->next);
        break;
    }
    }
}

void Converter::semicolon() {
    fprintf(file, ";\n");
}

void Converter::write(const char* data) {
    fprintf(file, data);
}