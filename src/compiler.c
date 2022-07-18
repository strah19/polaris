/**
* @file compiler.c
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
* The actual compiler of the polaris that will do a single run 
* and convert it to bytecode.
*/

#include "compiler.h"
#include "parser.h"
#include "code_generator.h"

VMResults compiler_run(const char* source) {
    Bytecode bytecode;
    bytecode_init(&bytecode);

    if (!compiler_compile(source, &bytecode)) {
        bytecode_free(&bytecode);
        return VM_COMPILER_ERROR;
    }

    VMResults results = vm_run(&bytecode);
    bytecode_free(&bytecode);
    vm_reset_stack();
    return results;
}

bool compiler_compile(const char* source, Bytecode* bytecode) {
    generator_set_current_bytecode(bytecode);
    lexer_init(source);
    parser_init();

    parser_advance();
    while (!parser_match(T_EOF))
        parse_decleration();

    compiler_end_compilation();
    return !parser_get_errors();
}

void compiler_end_compilation() {
    generator_emit_return();
}