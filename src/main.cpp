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

void repl();
void run_src_file(const char* filepath);

bool log_tokens = false;

int main(int argc, char* argv[]) {
    if (argc == 1) 
        repl();
    else if (argc >= 2)
        run_src_file(argv[1]);
    else
        fatal_error("Unknown arguments passed into Polaris.\n");
    return 0;
}

void repl() {
    printf("Polaris %d.%d", POLARIS_VERSION_MAJOR, POLARIS_VERSION_MINOR);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf(" (%d-%02d-%02d %02d:%02d:%02d)\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    while (true) {
        printf (">>> ");
        char buffer[MAX_REPL_SIZE];
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            report_error("Nothing entered into REPL.\n");
            continue;
        }
    }
}
void run_src_file(const char* filepath) {
    char* src = open_file(filepath);

#ifdef BENCHMARK_DEBUG
    Benchmark compiler_benchmark("Compiler");
#endif

    Lexer lexer(src);
    Tokens tokens = lexer.run();
    //lexer.log(tokens);

    Parser parser(&tokens[0], filepath);
    parser.parse();

    semantic_checker(parser.get_unit());

    if (!parser.has_errors() && !semantic_error_count()) {
        CodeGenerator generator(parser.get_unit(), parser.get_functions());
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