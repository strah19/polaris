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

    int index = 0;

    write(PREAMBLE_BOILERPLATE);
    while (unit->declerations[index]->type == AST_FUNCTION) {
        convert_function(AST_CAST(Ast_Function, unit->declerations[index]));
        index++;
    }
    write(MAIN_BOILERPLATE);
    while (index < unit->declerations.size()) {
        convert_decleration(unit->declerations[index]);
        index++;
    }
    write(POSTAMBLE_BOILERPLATE);

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

void Converter::convert_function(Ast_Function* function) {
    convert_type(function->return_type);
    write(function->ident);
    write("(");
    for (int i = 0; i < function->args.size(); i++) {
        convert_type(function->args[i]->type_value);
        fprintf(file, "%s ", function->args[i]->ident);
        if (i < function->args.size() - 1)
            write(",");
    }
    write(")");
    convert_scope(function->scope);
}

void Converter::convert_return(Ast_ReturnStatement* return_statement) {
    write("return ");
    convert_expression(return_statement->expression);
    semicolon();
}

void Converter::convert_decleration(Ast_Decleration* decleration) {
    switch (decleration->type) {
    case AST_EXPRESSION_STATEMENT: convert_expression_statement(AST_CAST(Ast_ExpressionStatement, decleration)); break;
    case AST_VAR_DECLERATION:      convert_variable_decleration(AST_CAST(Ast_VarDecleration, decleration));      break;
    case AST_SCOPE:                convert_scope(AST_CAST(Ast_Scope, decleration));                              break;
    case AST_IF:                   convert_if(AST_CAST(Ast_IfStatement, decleration));                           break;
    case AST_WHILE:                convert_while(AST_CAST(Ast_WhileStatement, decleration));                     break;
    case AST_RETURN:               convert_return(AST_CAST(Ast_ReturnStatement, decleration));                    break;
    default: break;
    }
}

void Converter::convert_scope(Ast_Scope* scope) {
    write("{\n");
    for (auto& dec : scope->declerations) {
        convert_decleration(dec);
    }
    write("}\n");
}

void Converter::compile() {
    char run_buffer[MAX_CMD_LEN];
    sprintf(run_buffer, "gcc %s.c -o %s %s", filename, objname, flags);
    printf("%s\n", run_buffer);
    system(run_buffer);
}

void Converter::convert_expression_statement(Ast_ExpressionStatement* expression_statment) {
    convert_expression(expression_statment->expression);
    semicolon();
}

void Converter::convert_variable_decleration(Ast_VarDecleration* variable_decleration) {
    convert_type(variable_decleration->type_value);
    fprintf(file, "%s ", variable_decleration->ident);
    if (variable_decleration->expression) {
        write("= ");
        convert_expression(variable_decleration->expression);
    }
    semicolon();
}

void Converter::convert_if(Ast_IfStatement* if_statement) {
    write("if(");
    convert_expression(if_statement->condition);
    write(")\n");
    convert_scope(if_statement->scope);
    Ast_ConditionalStatement* current = if_statement->next;
    while (current) {
        if (current->type == AST_ELIF)
            convert_elif(AST_CAST(Ast_ElifStatement, current));
        else if (current->type == AST_ELSE)
            convert_else(AST_CAST(Ast_ElseStatement, current));
        current = current->next;
    }
}

void Converter::convert_elif(Ast_ElifStatement* elif_statement) {
    write("else if(");
    convert_expression(elif_statement->condition);
    write(")\n");
    convert_scope(elif_statement->scope);
}

void Converter::convert_else(Ast_ElseStatement* else_statement) {
    write("else");
    convert_scope(else_statement->scope);
}

void Converter::convert_while(Ast_WhileStatement* while_statement) {
    write("while(");
    convert_expression(while_statement->condition);
    write(")\n");
    convert_scope(while_statement->scope);
}

void Converter::convert_type(AstDataType type) {
    switch (type) {
    case AST_TYPE_INT:     write("int ");          break;
    case AST_TYPE_FLOAT:   write("float ");        break; 
    case AST_TYPE_BOOLEAN: write("bool ");         break;
    case AST_TYPE_STRING:  write("const char* ");  break;
    case AST_TYPE_VOID:    write("void ");         break;
    default:                                       break;
    }
}

void Converter::convert_expression(Ast_Expression* expression) {
    switch (expression->type) {
    case AST_PRIMARY: {
        Ast_PrimaryExpression* primary = AST_CAST(Ast_PrimaryExpression, expression);
        switch (primary->prim_type) {
        case AST_PRIM_DATA: {
            switch (primary->type_value) {
            case AST_TYPE_INT:   fprintf(file, "%d", primary->int_const);     break;
            case AST_TYPE_FLOAT: fprintf(file, "%ff", primary->float_const); break;
            case AST_TYPE_BOOLEAN: {
                if (primary->boolean) write("true");
                else                  write("false");
                break;
            }
            case AST_TYPE_STRING: fprintf(file, "\"%s\"", primary->string); break;
            default: break;
            }
            break;
        }
        case AST_PRIM_NESTED: {
            write("(");
            convert_expression(primary->nested);
            write(")");
            break;
        }
        case AST_PRIM_ID: {
            write(primary->ident);
            break;
        }
        case AST_PRIM_CAST: {
            write("(");
            convert_type(primary->cast.cast_type);
            write(")(");
            convert_expression(primary->cast.expression);
            write(")");
            break;
        }
        case AST_PRIM_CALL: {
            write(primary->call->ident);
            write("(");
            for (int i = 0; i < primary->call->args.size(); i++) {
                convert_expression(primary->call->args[i]);
                if (i < primary->call->args.size() -1 )
                    write(",");
            }
            write(")");
            break;
        }
        default: break;
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
        default:                                              break;
        }
        convert_expression(binary->right);
        break;
    }
    case AST_ASSIGNMENT: {
        Ast_Assignment* assignment = AST_CAST(Ast_Assignment, expression);
        convert_expression(assignment->id);
        switch (assignment->equal_type) {
        case AST_EQUAL_PLUS:     write("+="); break;
        case AST_EQUAL_MINUS:    write("-="); break;
        case AST_EQUAL_MULTIPLY: write("*="); break;
        case AST_EQUAL_DIVIDE:   write("/="); break;
        case AST_EQUAL:          write("=");  break;
        default: break;
        }
        convert_expression(assignment->expression);
        break;
    }
    case AST_UNARY: {
        Ast_UnaryExpression* unary = AST_CAST(Ast_UnaryExpression, expression);
        switch (unary->op) {
        case AST_UNARY_MINUS:   write("-"); break;
        case AST_UNARY_BIT_NOT: write("~"); break;
        case AST_UNARY_NOT:     write("!"); break;
        default:                            break;
        }
        convert_expression(unary->next);
        break;
    }
    default: break;
    }
}

void Converter::semicolon() {
    fprintf(file, ";\n");
}

void Converter::write(const char* data) {
    fprintf(file, "%s", data);
}