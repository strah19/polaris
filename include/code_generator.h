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

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "ast.h"
#include "parser.h"

extern "C" {
    #include "opcodes.h"
    #include "bytecode.h"
}

struct Reference {
    Reference() = default;
    Reference(int address) : address(address) { init = true; }
    int address = 0;
    bool init = false;
};

class CodeGenerator {
public:
    CodeGenerator(Ast_TranslationUnit* root);
    ~CodeGenerator();
    void run();

    Bytecode* get_bytecode() { return &bytecode; }
private:
    void write(uint32_t opcode, Ast* ast);
    void write_constant(Value value, Ast* ast);

    void generate_from_ast(Ast* ast);
    void generate_scope(Ast_Scope* scope);
    void generate_variable_decleration(Ast_VarDecleration* decleration);
    void generate_print_statement(Ast_PrintStatement* print_statement);
    void generate_expression(Ast_Expression* expression);
    void generate_function(Ast_Function* function);
    void generate_return_statement(Ast_ReturnStatement* return_statement);
    void generate_if_statement(Ast_IfStatement* if_statement);
    int generate_conditional_statement(Ast_ConditionalStatement* conditional);
    void generate_while_statement(Ast_WhileStatement* while_statement);

    ObjString* allocate_string(const char* str);
private:
    Ast_TranslationUnit* root = nullptr;
    Bytecode bytecode;

    Map<String, Reference> references;
    int max_references_address = 0;
};

#endif // !CODE_GENERATOR_H