#include "compiler.h"
#include "parser.h"

VMResults compiler_run(const char* source) {
    Bytecode bytecode;
    bytecode_init(&bytecode);

    if (!compiler_compile(source, &bytecode)) {
        bytecode_free(&bytecode);
        return VM_COMPILER_ERROR;
    }

    VMResults results = vm_run(&bytecode);
    bytecode_free(&bytecode);
    return results;
}

bool compiler_compile(const char* source, Bytecode* bytecode) {
    set_current_bytecode(bytecode);
    init_lexer(source);
    init_parser();

    advance_parser();
    expression();
    consume(T_SEMICOLON, "Expected ';'");

    compiler_end_compilation();
    return !errors();
}

void compiler_end_compilation() {
    emit_return();
}