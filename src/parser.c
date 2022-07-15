#include "parser.h"
#include "error.h"

static Parser parser;
static Bytecode* current_chunk = NULL;

static void precedence_parser(int prec);
static void parse_binary();
static void parse_unary();
static void parse_parenthesis();
static void parse_number();
static ParserRule* get_rule(TokenType token);

static ParserRule rules [] = {
    [T_PLUS]        = { NULL,              parse_binary, PREC_TERM    },
    [T_MINUS]       = { parse_unary,       parse_binary, PREC_TERM    },
    [T_STAR]        = { NULL,              parse_binary, PREC_FACTOR  },
    [T_SLASH]       = { NULL,              parse_binary, PREC_FACTOR  },
    [T_INT_CONST]   = { parse_number,      NULL,         PREC_PRIMARY },
    [T_FLOAT_CONST] = { parse_number,      NULL,         PREC_PRIMARY },
    [T_LPAR]        = { parse_parenthesis, NULL,         PREC_NONE    },
    [T_SEMICOLON]   = { NULL,              NULL,         PREC_NONE    }
};

void init_parser() {
    parser.error_found = false;
    parser.panic = false;
}

void advance_parser() {
    parser.previous = parser.current;

    while (true) {
        parser.current = scan_token();

        if (parser.current.type != T_ERROR) break;

        parser_error_at_current(parser.current.start);
    }
}

void consume(TokenType type, const char* msg) {
    if (parser.current.type == type) 
        advance_parser();
    else
        parser_error_at_current(msg);
}

void parser_error_at_current(const char* msg) {
    parser_error(&parser.current, msg);
}

void parser_error(Token* token, const char* msg) {
    if (parser.panic) return;
    parser.panic = true;
    parser.error_found = true;
    report_error("on line %d at '%.*s' : %s.\n", token->line, token->size, token->start, msg);
}

bool errors() {
    return parser.error_found;
}

Bytecode* current_compiling_chunk() {
    return current_chunk;
}

void set_current_bytecode(Bytecode* chunk) {
    current_chunk = chunk;
}

void emit_bytecode(uint8_t code) {
    bytecode_write(code, parser.previous.line, current_chunk);
}

void emit_return() {
    emit_bytecode(OP_RETURN);
}

void expression() {
    precedence_parser(PREC_NONE);
}

static void precedence_parser(int prec) {
    advance_parser();
    ParseFn left = get_rule(parser.previous.type)->prefix;
    if (!left) {
        parser_error(&parser.previous, "Expected prefix-style expression");
        return;
    }
    left();

    while (parser.current.type != T_EOF && prec < get_rule(parser.current.type)->precedence) {
        advance_parser();
        get_rule(parser.previous.type)->infix();
    }
}

static void parse_binary() {
    int op = parser.previous.type;

    precedence_parser(get_rule(op)->precedence);

    switch(op) {
    case T_PLUS: 
        emit_bytecode(OP_PLUS);
        break;
    case T_MINUS: 
        emit_bytecode(OP_MINUS);
        break;
    case T_STAR: 
        emit_bytecode(OP_MULTIPLY);
        break;
    case T_SLASH: 
        emit_bytecode(OP_DIVIDE);
        break;
    }

}

static void parse_unary() {
    int prefix = parser.previous.type;
    expression();
    switch (prefix) {
    case T_MINUS: 
        emit_bytecode(OP_NEGATE);
        break;
    }
}

static void parse_parenthesis() {
    expression();
    consume(T_RPAR, "Expected ')");
}

static void parse_number() {
    int constant_addr = bytecode_add_constant(strtod(parser.previous.start, NULL), current_compiling_chunk());
    if (constant_addr > 255) {
        append_new_chunk();
        constant_addr = bytecode_add_constant(strtod(parser.previous.start, NULL), current_compiling_chunk());
    }
    emit_bytecode(OP_CONSTANT);
    emit_bytecode(constant_addr);
}

void append_new_chunk() {
    Bytecode* next = (Bytecode*) malloc(sizeof(Bytecode));
    if (!next) fatal_error("Unable to initalize new chunk of bytecode!\n");
    bytecode_init(next);
    emit_return();
    bytecode_append(current_chunk, next);
    set_current_bytecode(next);
}

static ParserRule* get_rule(TokenType token) {
    return &rules[token];
}