#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

void semantic_checker(Ast_TranslationUnit* root);

int semantic_error_count();

AstDataType get_expression_type(Ast_Expression* expression);

#endif //SEMANTIC_H