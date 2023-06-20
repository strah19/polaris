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
#include "common.h"

Symbol* search_symbol_table(const char* name, Symbol* root) {
    while (root != NULL) {
        int cmp = strcmp(name, root->name);
        if (cmp == 0) return root;
        root = (cmp < 0) ? root->left : root->right;
    }
    return NULL;
}

Symbol* enter_symbol(const char* name, Symbol** root) {
    Symbol* new_node = (Symbol*) malloc(sizeof(Symbol));
    new_node->name = (char*) malloc(strlen(name));
    strcpy(new_node->name, name);
    new_node->left = new_node->right = NULL;
    new_node->defn.type = DEF_NONE;
    new_node->info = NULL;

    Symbol* test_node;
    while ((test_node = *root) != NULL) {
        int cmp = strcmp(name, test_node->name);
        root = (cmp < 0) ? &test_node->left : &test_node->right;
    }

    *root = new_node;
    printf("NEW NODE\n");
    return new_node;
}

void free_symbol_table(Symbol* root) {
    if (root == NULL) return;

    free_symbol_table(root->left);
    free_symbol_table(root->right);

    free(root->name);
    free(root);
}