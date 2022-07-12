#include "common.h"
#include "config.h"
#include "error.h"
#include "lexer.h"
#include "benchmark.h"
#include "parser.h"

// For VM
#ifdef USE_VM
    #include "vm.h"
    #include "opcodes.h"
    #include "debug.h"
#endif // !USE_VM

VMResults run_file(const char* filepath);
char* open_file(const char* filepath);
void repl();
VMResults run(const char* source);
bool compile(const char* source, BytecodeChunk* chunk);
void end_compilation();

int main(int argc, char* argv[]) {
    init_vm();
    VMResults result;
    if (argc == 1) 
        repl();
    else if (argc == 2) 
        result = run_file(argv[1]);
    else 
        fatal_error("Incorrect number of arguments passed to Polaris.\n");

    free_vm();

    switch (result) {
    case VM_COMPILER_ERROR: fatal_error("Exitng with compiler errors.\n");
    case VM_RUNTIME_ERROR:  fatal_error("Exiting with runtime errors.\n");
    default: printf("Exiting with no errors.\n");
    }

    return 0;
}

void repl() {
    
}

VMResults run_file(const char* filepath) {
    char* start = open_file(filepath);

    begin_benchmark("compiler");
    VMResults result = run(start);
    stop_benchmark();
    
    free(start);
    return result;
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


VMResults run(const char* source) {
    BytecodeChunk chunk;
    init_chunk(&chunk);

    if (!compile(source, &chunk)) {
        free_chunk(&chunk);
        return VM_COMPILER_ERROR;
    }

    VMResults results = run_vm(&chunk);
    free_chunk(&chunk);
    return results;
}

bool compile(const char* source, BytecodeChunk* chunk) {
    set_current_chunk(chunk);
    init_lexer(source);
    init_parser();

    advance_parser();
    expression();
    consume(T_SEMICOLON, "Expected ';'");

    end_compilation();
    return !errors();
}

void end_compilation() {
    emit_return();
}