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

#include "config.h"
#include "error.h"
#include "lexer.h"
#include "util.h"
#include "code_generator.h"
#include "parser.h"
#include "benchmark.h"

extern "C" {
    #include "vm.h"
}

#include "semantic.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_REPL_SIZE 256
#define BENCHMARK_DEBUG

void run_src_file(const char* filepath);

int main(int argc, char* argv[]) {
    if (argc >= 2)
        run_src_file(argv[1]);
    else
        fatal_error("Unknown arguments passed into Polaris.\n");
    return 0;
}

void run_src_file(const char* filepath) {
    char* src = open_file(filepath);

#ifdef BENCHMARK_DEBUG
    Benchmark compiler_benchmark("Compiler");
#endif

    Lexer lexer(src);
    Tokens tokens = lexer.run();

    Parser parser(&tokens[0], filepath);
    parser.parse();

    semantic_checker(parser.get_unit());

    if (!parser.has_errors() && !semantic_error_count()) {
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
    } else fatal_error("Exiting with compiler error(s).\n");

    delete src;
}