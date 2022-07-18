/**
* @file code_generator.c
* @author strah19
* @date July 16, 2022
* @version 1.0
*
* @section LICENSE
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the MIT License as published
* by the Free Software Foundation.
*
* @section DESCRIPTION
*
* Handles creating the bytecode from the parser.
*/

#include "code_generator.h"
#include "error.h"

static Bytecode* current_bytecode = NULL;
static Parser* current_parser = NULL;

void generator_get_parser(Parser* parser) {
    current_parser = parser;
}

Bytecode* generator_get_current_bytecode() {
    return current_bytecode;
}

void generator_set_current_bytecode(Bytecode* bytecode) {
    current_bytecode = bytecode;
}

void generator_emit_bytecode(uint8_t code) {
    bytecode_write(code, current_parser->current.line, current_bytecode);
}

void generator_emit_return() {
    generator_emit_bytecode(OP_RETURN);
}

void generator_append_new_chunk() {
    Bytecode* next = (Bytecode*) malloc(sizeof(Bytecode));
    if (!next) fatal_error("Unable to initalize new chunk of bytecode!\n");
    bytecode_init(next);
    generator_emit_return();
    bytecode_append(current_bytecode, next);
    generator_set_current_bytecode(next);
}

int generator_emit_float_constant(Token token) {
    return bytecode_add_constant(FLOAT_VALUE(strtod(token.start, NULL)), generator_get_current_bytecode());
}

int generator_emit_int_constant(Token token) {
    return bytecode_add_constant(INT_VALUE(atoi(token.start)), generator_get_current_bytecode());
}