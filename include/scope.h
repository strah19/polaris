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

struct Scope {
    Scope() = default;
    Symbol* root = nullptr;
    Scope* previous = nullptr;

    SymbolDefinition last;

    void add(const char* name, SymbolDefinition defn);
    bool in_any(const char* name);
    SymbolDefinition get(const char* name); 
};  