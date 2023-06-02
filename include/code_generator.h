#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "ast.h"
#include "parser.h"

extern "C" {
    #include "opcodes.h"
    #include "bytecode.h"
}

using FunctionScope = Map<String, int>;

class CodeGenerator {
public:
    CodeGenerator(Ast_TranslationUnit* root, Vector<int>* function_indices, Scope* scope);
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

    ObjString* allocate_string(const char* str);
private:
    Ast_TranslationUnit* root = nullptr;
    Vector<int>* function_indices = nullptr;
    Scope* scope = nullptr;
    Bytecode bytecode;

    Map<String, int> references;
    Map<String, int> function_pointers;
    int max_references_address = 0;
};

#endif // !CODE_GENERATOR_H