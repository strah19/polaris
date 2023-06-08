/**
 * @file main.cpp
 * @author strah19
 * @brief Main entry point into Polaris!
 * @version 0.1
 * @date 2022-07-19
 * 
 * @copyright Copyright (c) 2022
 * 
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
    if (log_tokens) lexer.log(tokens);

    Parser parser(&tokens[0], filepath);
    parser.parse();

    semantic_checker(parser.get_unit());

    if (!parser.has_errors() && !semantic_error_count()) {
        CodeGenerator generator(parser.get_unit(), parser.get_functions(), parser.get_scope());
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