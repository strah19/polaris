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

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"
#include "common.h"

enum DefinitionType {
    DEF_VAR, DEF_FUN, DEF_CLS, DEF_NONE
};

struct VarSymbol {
    AstDataType var_type = AST_TYPE_NONE;
    AstSpecifierType specifiers = AST_SPECIFIER_NONE;
};

struct FunctionSymbol {
    AstDataType return_type = AST_TYPE_VOID;
    Ast_Expression* default_values[MAX_ARGS];
    size_t arg_count = 0;      
    Ast_Function* function_ptr;  
};

//Other definitions would go here too like procedures and classes.
struct SymbolDefinition {
    DefinitionType type = DEF_NONE;
    VarSymbol var;
    FunctionSymbol func;
};

struct Symbol {
    Symbol* left;
    Symbol* right;
    SymbolDefinition defn;
    char* name;
};

Symbol* search_symbol_table(const char* name, Symbol* root);

Symbol* enter_symbol(const char* name, SymbolDefinition defn, Symbol** root);

void free_symbol_table(Symbol* root);

void log_symbol(Symbol* symbol);

#endif