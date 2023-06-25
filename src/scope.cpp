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

#include "scope.h"

void Scope::add(const char* name, SymbolDefinition defn) {
    Symbol* new_node = enter_symbol(name, defn, &root);
    last = defn;
}

SymbolDefinition Scope::get(const char* name) {
    Scope* current = this;
    while (current) {
        Symbol* node = search_symbol_table(name, current->root);
        if (node) return node->defn;
        current = current->previous;
    }
    return SymbolDefinition();
}

bool Scope::in_any(const char* name) {
    Scope* current = this;
    while (current) {
        if (search_symbol_table(name, current->root)) return true;
        current = current->previous;
    }
    return false;
}
