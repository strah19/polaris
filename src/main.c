#include "common.h"
#include "config.h"
#include "error.h"
#include "lexer.h"
#include "benchmark.h"

// From VM
#include "vm.h"
#include "opcodes.h"
#include "debug.h"

void run_file(const char* filepath);
char* open_file(const char* filepath);
void repl();
void compile(const char* source);

int main(int argc, char* argv[]) {
    init_vm();

    BytecodeChunk bytecode;
    init_chunk(&bytecode);
    write_chunk(OP_CONSTANT, &bytecode, 1);
    write_chunk(add_constant(1.2, &bytecode), &bytecode, 1);
    write_chunk(OP_CONSTANT, &bytecode, 3);
    write_chunk(add_constant(55, &bytecode), &bytecode, 3);
    write_chunk(OP_NEGATE, &bytecode, 3);
    write_chunk(OP_PLUS, &bytecode, 3);
    write_chunk(OP_RETURN, &bytecode, 3);

    interpret(&bytecode);

    free_chunk(&bytecode);
    free_vm();

    if (argc == 1) 
        repl();
    else if (argc == 2) 
        run_file(argv[1]);
    else 
        fatal_error("Incorrect number of arguments passed to Polaris.\n");

    return 0;
}

void run_file(const char* filepath) {
    char* start = open_file(filepath);

    begin_benchmark("compiler");
    compile(start);
    stop_benchmark();
    
    free(start);
}

char* open_file(const char* filepath) {
    FILE* file = fopen(filepath, "rb");

    if (!file)
        fatal_error("Unable to open file '%s' for compilation.\n", filepath);

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);

    if (!buffer)
        fatal_error("Unable to allocate memory to read file '%s'\n", filepath);

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}

void repl() {
    
}

void compile(const char* source) {
    Lexer lexer = init_lexer(source);

    while (true) {
        Token token = scan_token(&lexer);

        if (token.type == T_ERROR) fatal_error("%.*s.\n", token.size, token.start);
        else if (token.type == T_EOF) break;

        printf ("%d : '%.*s' (%d)\n", token.line, token.size, token.start, token.type);
    }
}