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
#include "parser.h"
#include <time.h>

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
VMResults repl();

int main(int argc, char* argv[]) {
    vm_init();

    VMResults result;
    if (argc == 1) 
        result = repl();
    else if (argc == 2) 
        result = run_source_file(argv[1]);
    else 
        fatal_error("Incorrect number of arguments passed to Polaris.\n");

    vm_free();

    switch (result) {
    case VM_COMPILER_ERROR: fatal_error("Exiting with %d compiler errors.\n", parser_get_errors());
    case VM_RUNTIME_ERROR:  fatal_error("Exiting with runtime errors.\n");
    default: printf("Exiting with no errors.\n");
    }

    return 0;
}

VMResults repl() {
    printf("Polaris %d.%d ", POLARIS_VERSION_MAJOR, POLARIS_VERSION_MINOR);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("(%d-%02d-%02d %02d:%02d:%02d)\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    while (true) {
        printf (">>> ");
        char buffer[MAX_REPL_SIZE];
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            report_error("Nothing entered into REPL.\n");
            continue;
        }
        compiler_run(buffer);
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