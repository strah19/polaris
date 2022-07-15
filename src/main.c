/**
* @file main.c
* @author strah19
* @date July 13, 2022
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
* Main function of Polaris!
*/

#include "config.h"
#include "error.h"
#include "benchmark.h"
#include "compiler.h"
#include "misc.h"

#define MAX_REPL_SIZE 256

// For VM
#ifdef USE_VM
#include "vm.h"
#include "opcodes.h"
#include "debug.h"
#else
#error "Must use Virtual Machine with Polaris"
#endif // !USE_VM

VMResults run_source_file(const char* filepath);
void repl();

int main(int argc, char* argv[]) {
    vm_init();

    VMResults result;
    if (argc == 1) 
        repl();
    else if (argc == 2) 
        result = run_source_file(argv[1]);
    else 
        fatal_error("Incorrect number of arguments passed to Polaris.\n");

    vm_free();

    switch (result) {
    case VM_COMPILER_ERROR: fatal_error("Exiting with compiler errors.\n");
    case VM_RUNTIME_ERROR:  fatal_error("Exiting with runtime errors.\n");
    default: printf("Exiting with no errors.\n");
    }

    return 0;
}

void repl() {
    while (true) {
        printf (">>> ");
        char buffer[MAX_REPL_SIZE];
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            report_error("\nNothing entered into REPL.\n");
            return;
        }
        if (compiler_run(buffer) != VM_OK) return; 
    }
}

VMResults run_source_file(const char* filepath) {
    char* start = open_file(filepath);

    begin_benchmark("compiler");
    VMResults result = compiler_run(start);
    stop_benchmark();
    
    free(start);
    return result;
}