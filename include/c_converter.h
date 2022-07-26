#ifndef C_CONVERTER_H
#define C_CONVERTER_H

#include "ast.h"

static const char* PREAMBLE_BOILERPLATE = 
"#include <stdio.h>\n"
"#include <stdbool.h>\n"
"#include <stdint.h>\n\n"
"typedef uint32_t u32;\n"
"typedef int32_t i32;\n"
"typedef float f32;\n"
"typedef double f64;\n"
"typedef uint64_t u64;\n"
"typedef int64_t i64;\n";

static const char* MAIN_BOILERPLATE = 
"\n"
"int main(int argc, char *argv[]) {\n";

static const char* POSTAMBLE_BOILERPLATE = 
"\n}";

struct Converter {
    void run(Ast_TranslationUnit* unit);
    void compile();

    const char* objname  = "polaris";
    const char* filename = "polaris";
    const char* flags = "";
    const char* build_directory = nullptr;
private:
    void open_file();
    void convert_function(Ast_Function* function);
    void convert_decleration(Ast_Decleration* decleration);
    void convert_expression_statement(Ast_ExpressionStatement* expression_statment);
    void convert_variable_decleration(Ast_VarDecleration* variable_decleration, bool semi = true);
    void convert_scope(Ast_Scope* scope);
    void convert_if(Ast_IfStatement* if_statement);
    void convert_elif(Ast_ElifStatement* elif_statement);
    void convert_else(Ast_ElseStatement* else_statement);
    void convert_while(Ast_WhileStatement* while_statement);

    void convert_type(AstDataType type);
    void convert_expression(Ast_Expression* expression);
    void semicolon();
    void write(const char* data);
private:
    FILE* file = nullptr;
};

#endif //!C_CONVERTER_H