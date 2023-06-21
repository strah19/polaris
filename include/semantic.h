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

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

void semantic_checker(Ast_TranslationUnit* root);

int semantic_error_count();

AstDataType get_expression_type(Ast_Expression* expression, AstDataType can_it_be = AST_TYPE_NONE);

void print_expression(Ast_Expression* expression);

#endif //SEMANTIC_H