#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <cinttypes>
#include <map>
#include <string>

enum Precedence {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_BITWISE_OR,
    PREC_BITWISE_XOR,
    PREC_BITWISE_AND,
    PREC_EQUAL,
    PREC_COMPARE,
    PREC_BITWISE_SHIFT,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_PRIMARY
};

struct ParserError {
    Token* token = nullptr;

    ParserError() = default;
    ParserError(Token* token) : token(token) { }
};

enum DefinitionType {
    DEF_VAR, DEF_FUN, DEF_CLS
};

struct Scope {
    Scope() = default;
    std::map<std::string, DefinitionType> definitions;
    Scope* previous = nullptr;

    void add(const std::string& name, DefinitionType type);
    bool in_scope(const std::string& name);
    bool in_any(const std::string& name);
};  

class Parser {
public:
    Parser(Token* tokens);
    Parser(Token* tokens, const char* filepath);
    ~Parser();
    void parse();
    Ast_TranslationUnit* get_unit() { return unit; }

    Token* peek(int index = 0);
    Token* advance();

    ParserError parser_error(Token* token, const char* msg);
    void        parser_warning(Token* token, const char* msg);
    Token*      consume(int type, const char* msg);
    bool        match(int type);
    bool        check(int type);
    bool        is_end();
    bool        has_errors() { return errors; }
    void        warnings(bool set) { show_warnings = set; }
private:
    Ast* default_ast(Ast* ast);
    void init(Token* tokens, const char* filepath);
    void synchronize();

    Ast_Decleration*         parse_decleration();
    Ast_Statement*           parse_statement();
    Ast_Function*            parse_function();
    Ast_VarDecleration*      parse_variable_decleration();
    Ast_ExpressionStatement* parse_expression_statement();
    Ast_Scope*               parse_scope();
    Ast_IfStatement*         parse_if();
    Ast_Expression*          parse_expression(Precedence precedence = PREC_NONE, AstDataType expected_type = AST_TYPE_NONE);

    Ast_Expression* parse_binary_expression(Ast_Expression* left);
    Ast_Expression* parse_unary_expression();
    Ast_Expression* parse_primary_expression(AstDataType expected_type = AST_TYPE_NONE);

    bool is_unary(Token* token);
    bool is_primary(Token* token);

    void check_types(Ast_PrimaryExpression* left, Ast_PrimaryExpression* right);
    bool check_either(Ast_PrimaryExpression* left, Ast_PrimaryExpression* right, AstDataType type);
    bool ignore_type(Ast_PrimaryExpression* left, Ast_PrimaryExpression* right, AstDataType type);
    bool is_type(Ast_PrimaryExpression* prim, AstDataType type);
    bool real_type(AstDataType type);
    AstDataType search_expression_for_type(Token* token, Ast_Expression* expression);
    AstDataType parse_type();
private:
    Token* tokens = nullptr;
    uint32_t current = 0;
    const char* filepath = nullptr;
    Ast_TranslationUnit* unit = nullptr;
    bool errors = false;
    bool show_warnings = true;

    Scope* current_scope;
    Scope main_scope;
};

#endif //!PARSER_H