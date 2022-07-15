#ifndef DEBUG_H
#define DEBUG_H

#include "bytecode.h"

extern void debug_disassemble_bytecode(Bytecode* bytecode, const char* name);

extern int  debug_disassemble_instruction(Bytecode* bytecode, int off);

extern void debug_disassemble_stack(Value* stack, Value* top);

#endif // !DEBUG_H