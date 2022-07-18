#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "vm.h"
#include "parser.h"

extern void generator_get_parser(Parser* parser);

extern Bytecode* generator_get_current_bytecode();

extern void generator_set_current_bytecode(Bytecode* bytecode);

extern void generator_emit_bytecode(uint8_t code);

extern void generator_emit_return();

extern void generator_append_new_chunk();

extern int generator_emit_float_constant(Token token);

extern int generator_emit_int_constant(Token token);

extern int generator_emit_binary_constant(Token token);

extern int generator_emit_true();

extern int generator_emit_false();

#endif // !CODE_GENERATOR_H