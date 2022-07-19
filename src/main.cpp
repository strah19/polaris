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
#include "parser.h"
#include <stdio.h>
#include <time.h>

// For VM
#ifdef USE_VM
#include "vm.h"
#include "opcodes.h"
#endif //!USE_VM

#define MAX_REPL_SIZE 256

void repl();
void run_src_file(const char* filepath);
void run(const char* src, const char* filepath);

int main(int argc, char* argv[]) {

    if (argc == 1) 
        repl();
    else if (argc == 2)
        run_src_file(argv[1]);
    else
        fatal_error("Unknown arguments passed into Polaris.\n");
    return 0;
}

void repl() {
    printf("Polaris %d.%d\n", POLARIS_VERSION_MAJOR, POLARIS_VERSION_MINOR);
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
        run(buffer, nullptr);
    }
}

void run_src_file(const char* filepath) {
    char* src = open_file(filepath);

    run(src, filepath);

    delete src;
}

void run(const char* src, const char* filepath) {
    Lexer lexer(src);
    Tokens tokens = lexer.run();
    Lexer::log(tokens);
    Parser parser(&tokens[0], filepath);
    parser.parse();
}