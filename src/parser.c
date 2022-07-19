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
static void parse_infix();
static void parse_unary();
static void parse_parenthesis();
static void parse_float();
static void parse_int();
static void parse_binary();
static void parse_boolean();
static void parse_primary(int type);
static int  get_constant_address(int type);
static ParserRule* get_rule(TokenType token);

static ParserRule rules [] = {
    [T_PLUS]          = { NULL,              parse_infix, PREC_TERM          },
    [T_MINUS]         = { parse_unary,       parse_infix, PREC_TERM          },
    [T_STAR]          = { NULL,              parse_infix, PREC_FACTOR        },
    [T_SLASH]         = { NULL,              parse_infix, PREC_FACTOR        },
    [T_INT_CONST]     = { parse_int,         NULL,        PREC_PRIMARY       },
    [T_FLOAT_CONST]   = { parse_float,       NULL,        PREC_PRIMARY       },
    [T_BINARY_CONST]  = { parse_binary,      NULL,        PREC_PRIMARY       },
    [T_LPAR]          = { parse_parenthesis, NULL,        PREC_NONE          },
    [T_SEMICOLON]     = { NULL,              NULL,        PREC_NONE          },
    [T_PERCENT]       = { NULL,              parse_infix, PREC_FACTOR        },
    [T_LARROW]        = { NULL,              parse_infix, PREC_COMPARE       },
    [T_RARROW]        = { NULL,              parse_infix, PREC_COMPARE       },
    [T_LTE]           = { NULL,              parse_infix, PREC_COMPARE       },
    [T_GTE]           = { NULL,              parse_infix, PREC_COMPARE       },
    [T_COMPARE_EQUAL] = { NULL,              parse_infix, PREC_EQUAL         },
    [T_NOT_EQUAL]     = { NULL,              parse_infix, PREC_EQUAL         },
    [T_AND]           = { NULL,              parse_infix, PREC_AND           },
    [T_OR]            = { NULL,              parse_infix, PREC_OR            },
    [T_EXCLAMATION]   = { parse_unary,       NULL,        PREC_UNARY         },
    [T_NOT]           = { parse_unary,       NULL,        PREC_UNARY         },
    [T_AMPERSAND]     = { NULL,              parse_infix, PREC_BITWISE_AND   },
    [T_LINE]          = { NULL,              parse_infix, PREC_BITWISE_OR    },
    [T_CARET]         = { NULL,              parse_infix, PREC_BITWISE_XOR   },
    [T_LSHIFT]        = { NULL,              parse_infix, PREC_BITWISE_SHIFT },
    [T_RSHIFT]        = { NULL,              parse_infix, PREC_BITWISE_SHIFT },
    [T_TRUE]          = { parse_boolean,     NULL,        PREC_PRIMARY       },
    [T_FALSE]         = { parse_boolean,     NULL,        PREC_PRIMARY       }
};

void parser_init() {
    parser.errors = 0;
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
    parser.errors++;
    if (token->type == T_EOF)
        report_error("At the end of the file %s.\n", msg);
    else
        report_error("on line %d at '%.*s' %s.\n", token->line, token->size, token->start, msg);
}

int parser_get_errors() {
    return parser.errors;
}

void parse_expression() {
    parse_precedence(PREC_NONE);
}

static void parse_precedence(int prec) {
    parser_advance();
    ParseFn left = get_rule(parser.previous.type)->prefix;
    if (!left) {
        parser_error(&parser.previous, "Expected unary or primary expression");
        return;
    }
    left();

    while (parser.current.type != T_EOF && prec < get_rule(parser.current.type)->precedence) {
        parser_advance();
        get_rule(parser.previous.type)->infix();
    }
}

static void parse_infix() {
    int op = parser.previous.type;
    parse_precedence(get_rule(op)->precedence);

    switch(op) {
    case T_PLUS:           generator_emit_bytecode(OP_PLUS);      break;
    case T_MINUS:          generator_emit_bytecode(OP_MINUS);     break;
    case T_STAR:           generator_emit_bytecode(OP_MULTIPLY);  break;
    case T_SLASH:          generator_emit_bytecode(OP_DIVIDE);    break;
    case T_PERCENT:        generator_emit_bytecode(OP_MODULO);    break;
    case T_LARROW:         generator_emit_bytecode(OP_LT);        break;
    case T_RARROW:         generator_emit_bytecode(OP_GT);        break;
    case T_LTE:            generator_emit_bytecode(OP_LTE);       break;
    case T_GTE:            generator_emit_bytecode(OP_GTE);       break;
    case T_COMPARE_EQUAL:  generator_emit_bytecode(OP_EQUAL);     break;
    case T_NOT_EQUAL:      generator_emit_bytecode(OP_NOT_EQUAL); break;
    case T_OR:             generator_emit_bytecode(OP_OR);        break;
    case T_AND:            generator_emit_bytecode(OP_AND);       break;
    case T_AMPERSAND:      generator_emit_bytecode(OP_BIT_AND);   break;
    case T_LINE:           generator_emit_bytecode(OP_BIT_OR);    break;
    case T_CARET:          generator_emit_bytecode(OP_BIT_XOR);   break;
    case T_LSHIFT:         generator_emit_bytecode(OP_LSHIFT);    break;
    case T_RSHIFT:         generator_emit_bytecode(OP_RSHIFT);    break;
    }

}

static void parse_unary() {
    int prefix = parser.previous.type;
    parse_expression();
    switch (prefix) {
    case T_MINUS:       generator_emit_bytecode(OP_NEGATE);  break;
    case T_EXCLAMATION: generator_emit_bytecode(OP_NOT);     break;
    case T_NOT:         generator_emit_bytecode(OP_BIT_NOT); break;
    }
}

static void parse_parenthesis() {
    parse_expression();
    parser_consume(T_RPAR, "Expected ')");
}

void parse_primary(int type) {
    int constant_addr = get_constant_address(type);
    if (constant_addr > 255) {
        generator_append_new_chunk();
        constant_addr = get_constant_address(type);
    }
    generator_emit_bytecode(OP_CONSTANT);
    generator_emit_bytecode(constant_addr);
}

int get_constant_address(int type) {
    switch (type) {
    case T_INT_CONST:    return generator_emit_int_constant(parser.previous);
    case T_FLOAT_CONST:  return generator_emit_float_constant(parser.previous);
    case T_BINARY_CONST: return generator_emit_binary_constant(parser.previous);
    case T_TRUE:         return generator_emit_true();
    case T_FALSE:        return generator_emit_false();
    default: 
        parser_error(&parser.previous, "Unknown type found");
        return 0;
    }
}

static void parse_float() {
    parse_primary(T_FLOAT_CONST);
}

static void parse_int() {
    parse_primary(T_INT_CONST);
}

static void parse_binary() {
    parse_primary(T_BINARY_CONST);
}

static void parse_boolean() {
    switch (parser.previous.type) {
    case T_TRUE:  parse_primary(T_TRUE);  break;
    case T_FALSE: parse_primary(T_FALSE); break;
    default: return;
    }
}

static ParserRule* get_rule(TokenType token) {
    return &rules[token];
}

bool parser_match(TokenType type) {
    if (parser_check(type)) {
        parser_advance();
        return true;
    }
    return false;
}

bool parser_check(TokenType type) {
    return (type == parser.current.type);
}

void parse_decleration() {
    parse_statement();

    if (parser.panic) parser_synchronize();
}

void parser_synchronize() {
    while (!parser_check(T_SEMICOLON) && !parser_check(T_EOF))
        parser_advance();
    parser_advance();
    parser.panic = false;
}

void parse_statement() {
    if (parser_match(T_PRINT))
        parse_print_statement();
    else
        parse_expression_statement();
}

void parse_print_statement() {
    parse_expression();
    generator_emit_bytecode(OP_PRINT);
    parser_consume(T_SEMICOLON, "Expected ';' after print statement");
}

void parse_expression_statement() {
    parse_expression();
    generator_emit_bytecode(OP_POP);
    parser_consume(T_SEMICOLON, "Expected ';' after expression statement");
}