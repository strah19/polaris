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

/**
 * @brief Sets the parser so it's easier to emit bytecode.
 * 
 * @param parser 
 */
void generator_get_parser(Parser* parser) {
    current_parser = parser;
}

/**
 * @brief Returns the current bytecode being worked on.
 * 
 * @return Bytecode* 
 */
Bytecode* generator_get_current_bytecode() {
    return current_bytecode;
}

/**
 * @brief Lets the code generator know what bytecode needs to be worked on.
 * 
 * @param bytecode 
 */
void generator_set_current_bytecode(Bytecode* bytecode) {
    current_bytecode = bytecode;
}

/**
 * @brief Writes into the bytecode.
 * 
 * @param code 
 */
void generator_emit_bytecode(uint8_t code) {
    bytecode_write(code, current_parser->current.line, current_bytecode);
}

/**
 * @brief Writes a return into the bytecode.
 * 
 */
void generator_emit_return() {
    generator_emit_bytecode(OP_RETURN);
}

/**
 * @brief If the currentrt bytecode fills up, appends a new one to it.
 * 
 */
void generator_append_new_chunk() {
    Bytecode* next = (Bytecode*) malloc(sizeof(Bytecode));
    if (!next) fatal_error("Unable to initalize new chunk of bytecode!\n");
    bytecode_init(next);
    generator_emit_return();
    bytecode_append(current_bytecode, next);
    generator_set_current_bytecode(next);
}