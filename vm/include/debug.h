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

#ifndef DEBUG_H
#define DEBUG_H

#include "bytecode.h"

extern void debug_disassemble_bytecode(Bytecode* bytecode, const char* name);

extern int  debug_disassemble_instruction(Bytecode* bytecode, int off, bool runtime);

extern void debug_disassemble_stack(Value* stack, Value* top);

#endif // !DEBUG_H