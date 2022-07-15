/**
* @file bytecode.c
* @author strah19
* @date July 12, 2022
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
* Lays out a single 'block' of possible bytes for the virtual 
* machine to execute at a single given time. 
*/

#include "bytecode.h"
#include "mem.h"
#include <stdlib.h>

/**
 * @brief Initializesthe bytecode chunk.
 * 
 * @param bytecode 
 */
void bytecode_init(Bytecode* bytecode) {
    bytecode->capacity = 0;
    bytecode->count = 0;
    bytecode->line = NULL;
    bytecode->code = NULL;
    bytecode->next = NULL;
    value_init(&bytecode->constants);
}

/**
 * @brief Will write a code to a 'byte chunk' (plus the line number for errors).
 * 
 * @param code 
 * @param bytecode 
 * @param line 
 */
void bytecode_write(uint8_t code, int line, Bytecode* bytecode) {
    if (bytecode->capacity < bytecode->count + 1) {
        bytecode->capacity = NEW_CAPACITY(bytecode->capacity);
        bytecode->code = REALLOC(uint8_t,  bytecode->code, bytecode->capacity);
        bytecode->line = REALLOC(int, bytecode->line, bytecode->capacity);
    }

    bytecode->code[bytecode->count++] = code;
    bytecode->line[bytecode->count - 1] = line;
}

/**
 * @brief Moves the count back once, meaning it ignores the last code in execution.
 * 
 * @param bytecode 
 */
void bytecode_pop(Bytecode* bytecode) {
    bytecode->count--;
}

/**
 * @brief Adds constant to bytecode and returns constants index in array.
 * 
 * @param value 
 * @param bytecode 
 * @return int 
 */
int bytecode_add_constant(Value value, Bytecode* bytecode) {
    value_write(value, &bytecode->constants);
    return bytecode->constants.count - 1;
}

/**
 * @brief Free's the code from the chunk.
 * 
 * @param bytecode 
 */
void bytecode_free(Bytecode* bytecode) {
    FREE(uint8_t, bytecode->code);
    FREE(int, bytecode->line);
    value_free(&bytecode->constants);

    if (bytecode->next) {
        bytecode_free(bytecode->next);
        free (bytecode->next);
    }
    
    bytecode_init(bytecode);
}

/**
 * @brief Appends another chunk to the current bytecode.
 * 
 * @param bytecode 
 * @param append 
 */
void bytecode_append(Bytecode* bytecode, Bytecode* append) {
    bytecode->next = append;
}