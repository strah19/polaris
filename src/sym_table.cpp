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

#include "sym_table.h"

Symbol* search_symbol_table(const char* name, Symbol* root) {
    while (root != nullptr) {
        int cmp = strcmp(name, root->name);
        if (cmp == 0) return root;
        root = (cmp < 0) ? root->left : root->right;
    }
    return nullptr;
}

Symbol* enter_symbol(const char* name, SymbolDefinition defn, Symbol** root) {
    Symbol* new_node = (Symbol*) malloc(sizeof(Symbol));
    new_node->name = (char*) malloc(strlen(name));
    strcpy(new_node->name, name);
    new_node->left = new_node->right = NULL;
    new_node->defn = defn;

    Symbol* test_node;
    while ((test_node = *root) != NULL) {
        int cmp = strcmp(name, test_node->name);
        root = (cmp < 0) ? &test_node->left : &test_node->right;
    }

    *root = new_node;
    return new_node;
}

void free_symbol_table(Symbol* root) {
    if (root == NULL) return;

    free_symbol_table(root->left);
    free_symbol_table(root->right);

    free(root->name);
    free(root);
}

void log_symbol(Symbol* symbol) {
    if (!symbol) return;

    if (symbol->defn.type == DEF_VAR) {
        printf("VAR '%s', TYPE: %d, SPECIFIERS: %d.\n", symbol->name, symbol->defn.var.var_type, symbol->defn.var.specifiers);
    }
    else if (symbol->defn.type == DEF_FUN) {
        printf("FUNC '%s', ARG COUNT: %d, RET TYPE: %d.\n", symbol->name, symbol->defn.func.return_type, symbol->defn.func.arg_count);
    }
}