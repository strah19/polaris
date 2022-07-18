/**
* @file parser.c
* @author strah19
* @date July 16, 2022
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
* The main part of the compiler!
*/

#include "parser.h"
#include "error.h"
#include "code_generator.h"

static Parser parser;

static void parse_precedence(int prec);
static void parse_binary();
static void parse_unary();
static void parse_parenthesis();
static void parse_number();
static ParserRule* get_rule(TokenType token);

static ParserRule rules [] = {
    [T_PLUS]          = { NULL,              parse_binary, PREC_TERM    },
    [T_MINUS]         = { parse_unary,       parse_binary, PREC_TERM    },
    [T_STAR]          = { NULL,              parse_binary, PREC_FACTOR  },
    [T_SLASH]         = { NULL,              parse_binary, PREC_FACTOR  },
    [T_INT_CONST]     = { parse_number,      NULL,         PREC_PRIMARY },
    [T_FLOAT_CONST]   = { parse_number,      NULL,         PREC_PRIMARY },
    [T_LPAR]          = { parse_parenthesis, NULL,         PREC_NONE    },
    [T_SEMICOLON]     = { NULL,              NULL,         PREC_NONE    },
    [T_PERCENT]       = { NULL,              parse_binary, PREC_FACTOR  },
    [T_LARROW]        = { NULL,              parse_binary, PREC_COMPARE },
    [T_RARROW]        = { NULL,              parse_binary, PREC_COMPARE },
    [T_LTE]           = { NULL,              parse_binary, PREC_COMPARE },
    [T_GTE]           = { NULL,              parse_binary, PREC_COMPARE },
    [T_COMPARE_EQUAL] = { NULL,              parse_binary, PREC_EQUAL   },
    [T_NOT_EQUAL]     = { NULL,              parse_binary, PREC_EQUAL   },
    [T_AND]           = { NULL,              parse_binary, PREC_AND     },
    [T_OR]            = { NULL,              parse_binary, PREC_OR      },
    [T_EXCLAMATION]   = { parse_unary,       NULL,         PREC_UNARY   }
};

void parser_init() {
    parser.error_found = false;
    parser.panic = false;
    generator_get_parser(&parser);
}

void parser_advance() {
    parser.previous = parser.current;

    while (true) {
        parser.current = lexer_scan();
        if (parser.current.type != T_ERROR) break;
        parser_error_at_current(parser.current.start);
    }
}

void parser_consume(TokenType type, const char* msg) {
    if (parser.current.type == type) 
        parser_advance();
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
    if (token->type == T_EOF)
        report_error("At the end of the file : %s.\n", msg);
    else
        report_error("on line %d at '%.*s' : %s.\n", token->line, token->size, token->start, msg);
}

bool parser_get_errors() {
    return parser.error_found;
}

void parser_expression() {
    parse_precedence(PREC_NONE);
}

static void parse_precedence(int prec) {
    parser_advance();
    ParseFn left = get_rule(parser.previous.type)->prefix;
    if (!left) {
        parser_error(&parser.previous, "Expected prefix-style expression");
        return;
    }
    left();

    while (parser.current.type != T_EOF && prec < get_rule(parser.current.type)->precedence) {
        parser_advance();
        get_rule(parser.previous.type)->infix();
    }
}

static void parse_binary() {
    int op = parser.previous.type;
    parse_precedence(get_rule(op)->precedence);

    switch(op) {
    case T_PLUS:          generator_emit_bytecode(OP_PLUS);      break;
    case T_MINUS:         generator_emit_bytecode(OP_MINUS);     break;
    case T_STAR:          generator_emit_bytecode(OP_MULTIPLY);  break;
    case T_SLASH:         generator_emit_bytecode(OP_DIVIDE);    break;
    case T_PERCENT:       generator_emit_bytecode(OP_MODULO);    break;
    case T_LARROW:        generator_emit_bytecode(OP_LT);        break;
    case T_RARROW:        generator_emit_bytecode(OP_GT);        break;
    case T_LTE:           generator_emit_bytecode(OP_LTE);       break;
    case T_GTE:           generator_emit_bytecode(OP_GTE);       break;
    case T_COMPARE_EQUAL: generator_emit_bytecode(OP_EQUAL);     break;
    case T_NOT_EQUAL:     generator_emit_bytecode(OP_NOT_EQUAL); break;
    case T_OR:            generator_emit_bytecode(OP_OR);        break;
    case T_AND:           generator_emit_bytecode(OP_AND);       break;
    }

}

static void parse_unary() {
    int prefix = parser.previous.type;
    parser_expression();
    switch (prefix) {
    case T_MINUS:       generator_emit_bytecode(OP_NEGATE); break;
    case T_EXCLAMATION: generator_emit_bytecode(OP_NOT); break;
    }
}

static void parse_parenthesis() {
    parser_expression();
    parser_consume(T_RPAR, "Expected ')");
}

static void parse_number() {
    int constant_addr = bytecode_add_constant(strtod(parser.previous.start, NULL), generator_get_current_bytecode());
    if (constant_addr > 255) {
        generator_append_new_chunk();
        constant_addr = bytecode_add_constant(strtod(parser.previous.start, NULL), generator_get_current_bytecode());
    }
    generator_emit_bytecode(OP_CONSTANT);
    generator_emit_bytecode(constant_addr);
}

static ParserRule* get_rule(TokenType token) {
    return &rules[token];
}