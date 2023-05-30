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
#include "c_converter.h"
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

void print_scope(Scope* scope, int tabs = 1) {
    for (int i = 0; i < tabs; i++) printf("\t");
    printf("--------New Scope--------\n");

    for (int i = 0; i < tabs; i++) printf("\t");

    if (scope->definitions.size() != 0)
        printf("Symbols in Scope\n");
    for (auto& sym : scope->definitions) {
        for (int i = 0; i < tabs; i++) printf("\t");
        printf("Name: '%s', Type: %d.\n", sym.first.c_str(), sym.second.is);
    }

    for (auto& child : scope->children_scopes) {
        print_scope(&child, tabs++);
    }
}

void write(uint32_t opcode, Bytecode* code) {
    bytecode_write(opcode, 0, code);
}

void write_constant(Value value, Bytecode* code) {
    write(bytecode_add_constant(value, code), code);
}

void run_src_file(const char* filepath) {
    char* src = open_file(filepath);

    Benchmark compiler_benchmark("Compiler");
  
    Lexer lexer(src);
    Tokens tokens = lexer.run();
    if (log_tokens) lexer.log(tokens);

    Parser parser(&tokens[0], filepath);
    parser.parse();

    //print_scope(parser.get_scope());

    semantic_checker(parser.get_unit());

    if (!parser.has_errors() && !semantic_error_count()) {
        CodeGenerator generator(parser.get_unit(), parser.get_functions(), parser.get_scope());
        generator.run();
        compiler_benchmark.stop();
        vm_init();

        Bytecode code;
        bytecode_init(&code);
        write(OP_CONST, &code);
        write_constant(INT_VALUE(10), &code);
        write(OP_CONST, &code);
        write_constant(INT_VALUE(20), &code);
        write(OP_CALL, &code);
        write(8, &code);
        write(2, &code);
        write(OP_HALT, &code);
        write(OP_LOAD, &code);
        write(-3, &code);
        write(OP_LOAD, &code);
        write(-4, &code);
        write(OP_ADD, &code);
        write(OP_PRINT, &code);
        write(OP_RET, &code);

/*
    ICONST, 10,
    ICONST, 20,
    CALL, 8, 2,
    HALT,

    LOAD, -3,
    LOAD, -4,
    IADD, 
    SYS_WRITE,
    RET,
*/



        Benchmark vm_benchmark("Virtual Machine");
        if (!vm_run(&code))
            printf("Exiting with run time error(s).\n");
        vm_benchmark.stop();

        vm_reset_stack();
        vm_free();
    } else fatal_error("Exiting with compiler error(s).\n");

    delete src;
}