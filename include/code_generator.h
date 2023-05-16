#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "ast.h"

class CodeGenerator {
public:
    CodeGenerator(Ast_TranslationUnit* root);
    ~CodeGenerator();
    void run();
private:
    Ast_TranslationUnit* root = nullptr;
};

#endif // !CODE_GENERATOR_H