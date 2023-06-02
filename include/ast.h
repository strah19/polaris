#ifndef AST_H
#define AST_H

#include <cinttypes>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>

//These are temporary until custom data structures are made.

template<class T>
using Vector = std::vector<T>;

template <class Key, class Val>
using Map = std::map<Key, Val>;

using String = std::string;

enum AstType {
    AST_FUNCTION,
    AST_EXPRESSION,
    AST_UNARY,
    AST_PRIMARY,
    AST_BINARY,
    AST_ASSIGNMENT,
    AST_DECLERATION,
    AST_VAR_DECLERATION,
    AST_STATEMENT,
    AST_PRINT,
    AST_EXPRESSION_STATEMENT,
    AST_SCOPE,
    AST_CONDITIONAL,
    AST_IF,
    AST_ELIF, 
    AST_ELSE,
    AST_WHILE,
    AST_RETURN,
    AST_TRANSLATION_UNIT,
    AST_NONE,
};

enum AstOperatorType {
    AST_OPERATOR_MULTIPLICATIVE,
    AST_OPERATOR_DIVISION,
    AST_OPERATOR_MODULO,
    AST_OPERATOR_ADD,
    AST_OPERATOR_SUB,
    AST_OPERATOR_COMPARITIVE_EQUAL,
    AST_OPERATOR_COMPARITIVE_NOT_EQUAL,
    AST_OPERATOR_LTE,
    AST_OPERATOR_GTE,
    AST_OPERATOR_LT,
    AST_OPERATOR_GT,
    AST_OPERATOR_AND,
    AST_OPERATOR_OR,
    AST_OPERATOR_BIT_XOR,
    AST_OPERATOR_BIT_OR,
    AST_OPERATOR_BIT_AND,
    AST_OPERATOR_LSHIFT,
    AST_OPERATOR_RSHIFT,
    AST_OPERATOR_NONE
};

enum AstUnaryType {
    AST_UNARY_MINUS,
    AST_UNARY_NOT,
    AST_UNARY_BIT_NOT,
    AST_UNARY_NONE
};

enum AstEqualType {
    AST_EQUAL,
    AST_EQUAL_PLUS,
    AST_EQUAL_MINUS,
    AST_EQUAL_MULTIPLY,
    AST_EQUAL_DIVIDE,
    AST_EQUAL_MOD
};

enum AstPrimaryType {
    AST_PRIM_ID,
    AST_PRIM_CAST,
    AST_PRIM_CALL,
    AST_PRIM_NESTED,
    AST_PRIM_DATA,
    AST_PRIM_NONE
};

enum AstDataType {
    AST_TYPE_NONE = 0x00,
    AST_TYPE_FLOAT = 0x01,
    AST_TYPE_BOOLEAN = 0x02,
    AST_TYPE_INT = 0x04,
    AST_TYPE_STRING = 0x08,
    AST_TYPE_CHAR = 0x10,
    AST_TYPE_VOID = 0x20,
};

enum AstSpecifierType {
    AST_SPECIFIER_NONE  = 0x00,
    AST_SPECIFIER_CONST = 0x01
};

#define AST_CAST(type, base) ((type*) base)

#define IS_AST(base, type_to_check) (base->type == type_to_check)

struct Ast;
struct Ast_Expression;
struct Ast_Scope;
struct Ast_Decleration;

struct Ast {
    Ast() { }
    virtual ~Ast() { }

	AstType type = AST_NONE;
    uint32_t line = 0;
    const char* file;
};

struct Ast_Expression : Ast {
    Ast_Expression() { type = AST_EXPRESSION; }
    virtual ~Ast_Expression() {    }
};

struct Ast_FunctionCall {
    Ast_FunctionCall() { }
    Ast_FunctionCall(const char* ident, const Vector<Ast_Expression*>& args) : ident(ident), args(args) { }
    ~Ast_FunctionCall() { }
    const char* ident;
    Vector<Ast_Expression*> args;
};

struct Ast_Cast {
    Ast_Cast(Ast_Expression* expression, AstDataType cast_type) : cast_type(cast_type), expression(expression) { }
    ~Ast_Cast() {
        delete expression;
    }
    Ast_Expression* expression = nullptr;
    AstDataType cast_type = AST_TYPE_NONE;
};

struct Ast_PrimaryExpression : public Ast_Expression {
    Ast_PrimaryExpression() { type = AST_PRIMARY; }
    ~Ast_PrimaryExpression() override {
        switch (prim_type) {
        case AST_PRIM_NESTED:    delete nested; break;
        case AST_PRIM_CALL:      delete call;   break;
        default:                                break;
        }
    }

    AstPrimaryType prim_type = AST_PRIM_NONE;
    AstDataType type_value = AST_TYPE_NONE;

    bool local = false;
    int local_index = 0;
    
    union {
        int         int_const;
        float       float_const;
        const char* ident;
        const char* string;
        char        char_const;
        bool        boolean;
        
        Ast_FunctionCall* call;
        Ast_Expression*   nested;
        Ast_Cast          cast;
    };
};

struct Ast_BinaryExpression : public Ast_Expression {
    Ast_BinaryExpression() { type = AST_BINARY; }
    Ast_BinaryExpression(Ast_Expression* left, AstOperatorType op, Ast_Expression* right) 
        : left(left), op(op), right(right) { type = AST_BINARY; }
    ~Ast_BinaryExpression() override {
        delete left;
        delete right;
    }
    AstOperatorType op = AST_OPERATOR_NONE;

    Ast_Expression* left = nullptr;
    Ast_Expression* right = nullptr;

    Ast_BinaryExpression(const Ast_BinaryExpression& bin) {
        op = bin.op;
        left = bin.left;
        right = bin.right;
    }
};

struct Ast_UnaryExpression : public Ast_Expression {
    Ast_UnaryExpression() { type = AST_UNARY; }
    Ast_UnaryExpression(Ast_Expression* next, AstUnaryType op) : op(op), next(next) { type = AST_UNARY; }
    ~Ast_UnaryExpression() override {
        delete next;
    }
    Ast_Expression* next = nullptr;
    AstUnaryType op = AST_UNARY_NONE;
};

struct Ast_Assignment : public Ast_Expression {
    Ast_Assignment() { type = AST_ASSIGNMENT; }
    Ast_Assignment(Ast_PrimaryExpression* id, Ast_Expression* value, Ast_Assignment* next, AstEqualType equal_type = AST_EQUAL) : id(id), value(value), next(next), equal_type(equal_type) { type = AST_ASSIGNMENT; }
    ~Ast_Assignment() override {
        delete next;
    }

    AstEqualType equal_type = AST_EQUAL;

    Ast_PrimaryExpression* id = nullptr;
    Ast_Expression* value = nullptr;
    Ast_Assignment* next = nullptr;
};

struct Ast_Decleration : public Ast {
    Ast_Decleration() { type = AST_DECLERATION; }
    virtual ~Ast_Decleration() { } 
};

struct Ast_Statement : public Ast_Decleration {
    Ast_Statement() { type = AST_STATEMENT; }
    virtual ~Ast_Statement() { }
};

struct Ast_Scope : public Ast_Statement {
    Ast_Scope() { type = AST_SCOPE; }
    ~Ast_Scope() override {
        for (auto& dec : declerations) delete dec;
    }

    Vector<Ast_Decleration*> declerations;
};

struct Ast_ExpressionStatement : public Ast_Statement {
    Ast_ExpressionStatement(Ast_Expression* expression) : expression(expression) { type = AST_EXPRESSION_STATEMENT; }
    ~Ast_ExpressionStatement() override {
        delete expression;
    }
    Ast_Expression* expression = nullptr;
};

struct Ast_PrintStatement : public Ast_Statement {
    Ast_PrintStatement(Ast_Expression* expression) : expression(expression) { type = AST_PRINT; }
    ~Ast_PrintStatement() override {
        delete expression;
    }

    Ast_Expression* expression = nullptr;
};

struct Ast_ConditionalStatement : public Ast_Statement {
    Ast_ConditionalStatement() { type = AST_CONDITIONAL; }
    Ast_ConditionalStatement(Ast_Expression* condition, Ast_Scope* scope) : condition(condition), scope(scope) { type = AST_CONDITIONAL; }
    ~Ast_ConditionalStatement() override {
        delete condition;
        delete scope;
        delete next;
    }

    Ast_Expression* condition = nullptr;
    Ast_Scope* scope = nullptr;
    Ast_ConditionalStatement* next = nullptr;
};

struct Ast_IfStatement : Ast_ConditionalStatement {
    Ast_IfStatement() { type = AST_IF; }
    Ast_IfStatement(Ast_Expression* condition, Ast_Scope* scope) : Ast_ConditionalStatement(condition, scope) { type = AST_IF; }
    ~Ast_IfStatement() { }
};

struct Ast_ElifStatement : Ast_ConditionalStatement {
    Ast_ElifStatement() { type = AST_ELIF; }
    Ast_ElifStatement(Ast_Expression* condition, Ast_Scope* scope) : Ast_ConditionalStatement(condition, scope) { type = AST_ELIF; }
    ~Ast_ElifStatement() { }
};

struct Ast_ElseStatement : Ast_ConditionalStatement {
    Ast_ElseStatement() { type = AST_ELSE; }
    Ast_ElseStatement(Ast_Scope* scope) : Ast_ConditionalStatement(nullptr, scope) { type = AST_ELSE; }
    ~Ast_ElseStatement() { }
};

struct Ast_WhileStatement : Ast_ConditionalStatement {
    Ast_WhileStatement() { type = AST_WHILE; }
    Ast_WhileStatement(Ast_Expression* condition, Ast_Scope* scope) : Ast_ConditionalStatement(condition, scope) { type = AST_WHILE; }
    ~Ast_WhileStatement() { }
};

struct Ast_ReturnStatement : Ast_Statement {
    Ast_ReturnStatement(Ast_Expression* expression, AstDataType expected_return_type) : expression(expression), expected_return_type(expected_return_type) { type = AST_RETURN; }
    ~Ast_ReturnStatement() override {
        delete expression;
    }
    Ast_Expression* expression = nullptr;
    AstDataType expected_return_type = AST_TYPE_NONE;
};

struct Ast_VarDecleration : public Ast_Decleration {
    Ast_VarDecleration() { type = AST_VAR_DECLERATION; }
    Ast_VarDecleration(const char* ident, Ast_Expression* expression, AstDataType type_value, AstSpecifierType specifiers) 
        : ident(ident), expression(expression), type_value(type_value), specifiers(specifiers) { type = AST_VAR_DECLERATION; }
    ~Ast_VarDecleration() override {
        delete expression;
    }

    AstDataType type_value = AST_TYPE_NONE;
    AstSpecifierType specifiers = AST_SPECIFIER_NONE;
    const char* ident = nullptr;

    Ast_Expression* expression = nullptr;
};

struct Ast_Function : public Ast_Decleration {
    Ast_Function() { type = AST_FUNCTION; }
    Ast_Function(const char* ident, AstDataType return_type, const Vector<Ast_VarDecleration*> args, Ast_Scope* scope) : 
        ident(ident), return_type(return_type), args(args), scope(scope) { type = AST_FUNCTION; }
    ~Ast_Function() override {
        for (auto& arg : args) {
            delete arg;
        }
        delete scope;
    }

    const char* ident = nullptr;
    AstDataType return_type = AST_TYPE_VOID;
    Vector<Ast_VarDecleration*> args;
    Ast_Scope* scope = nullptr;
};

struct Ast_TranslationUnit : public Ast {
    Ast_TranslationUnit() { type = AST_TRANSLATION_UNIT; }
    ~Ast_TranslationUnit() {
        for (auto& dec : declerations) delete dec;
    }

    Vector<Ast_Decleration*> declerations;
};

#endif //!AST_H