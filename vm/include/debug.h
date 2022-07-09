#ifndef DEBUG_H
#define DEBUG_H

#include "bytecode.h"

extern void disassemble_chunk(BytecodeChunk* chunk, const char* name);

extern int disassemble_instruction(BytecodeChunk* chunk, int off);

extern int simple_instruction(const char* name, int off);

extern int constant_instruction(BytecodeChunk* chunk, int off);

#endif // !DEBUG_H