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

#include "error.h"
#include "lexer.h"
#include "util.h"
#include "code_generator.h"
#include "parser.h"
#include "benchmark.h"
#include "semantic.h"

extern "C" {
    #include "vm.h"
}

#define BENCHMARK_DEBUG

void compile_source(const char* filepath) {
    char* src = open_file(filepath);

#ifdef BENCHMARK_DEBUG
    Benchmark compiler_benchmark("Compiler");
#endif

    Lexer lexer(src);
    Tokens tokens = lexer.run();
    

    Parser parser(&tokens[0], filepath);
    parser.parse();

    if (!parser.errors())
        semantic_checker(parser.get_unit());

    if (!parser.errors() && !semantic_error_count()) {
        CodeGenerator generator(parser.get_unit());
        generator.run();
    #ifdef BENCHMARK_DEBUG
        compiler_benchmark.stop();
    #endif
    
        vm_init();

        {
    #ifdef BENCHMARK_DEBUG
        Benchmark vm_benchmark("Virtual Machine");
    #endif

        if (!vm_run(generator.get_bytecode()))
            printf("Exiting with run time error(s).\n");
        }

        vm_reset_stack();
        vm_free();
    } else fatal_error("Exiting with %d compiler error%s.\n", parser.errors(), (parser.errors() > 1) ? "s" : "");

    delete src;
}