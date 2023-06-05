#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

void semantic_checker(Ast_TranslationUnit* root);

int semantic_error_count();

AstDataType get_expression_type(Ast_Expression* expression, AstDataType can_it_be = AST_TYPE_NONE);

#endif //SEMANTIC_H