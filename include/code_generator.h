#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "ast.h"

extern "C" {
    #include "opcodes.h"
    #include "bytecode.h"
}

class CodeGenerator {
public:
    CodeGenerator(Ast_TranslationUnit* root);
    ~CodeGenerator();
    void run();

    Bytecode* get_bytecode() { return &bytecode; }
private:
    void write(uint8_t opcode, Ast* ast);
    void write_constant(Value value, Ast* ast);

    void generate_from_ast(Ast* ast);
    void generate_print_statement(Ast_PrintStatement* print_statement);
    void generate_expression(Ast_Expression* expression);
private:
    Ast_TranslationUnit* root = nullptr;
    Bytecode bytecode;
};

#endif // !CODE_GENERATOR_H