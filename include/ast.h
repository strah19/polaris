#ifndef AST_H
#define AST_H

#include <cinttypes>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>

template<class T>
using Vector = std::vector<T>;

template <class Key, class Val>
using Map = std::map<Key, Val>;

using String = std::string;

enum AstType {
    AST_EXPRESSION,
    AST_UNARY,
    AST_PRIMARY,
    AST_BINARY,
    AST_ASSIGNMENT,
    AST_DECLERATION,
    AST_VAR_DECLERATION,
    AST_FUNCTION,
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
    AST_TYPE_FLOAT,
    AST_TYPE_INT,
    AST_TYPE_BOOLEAN,
    AST_TYPE_STRING,
    AST_TYPE_CHAR,
    AST_TYPE_VOID,
    AST_TYPE_NONE
};

enum AstSpecifierType {
    AST_SPECIFIER_NONE  = 0x00,
    AST_SPECIFIER_CONST = 0x01
};

#define AST_CAST(type, base) static_cast<type*>(base)

struct Ast;
struct Ast_Expression;
struct Ast_Scope;
struct Ast_Decleration;

static void delete_ast(Ast_Decleration* ast);
static void delete_expression(Ast_Expression* expr);

struct Ast {
    Ast() { }
    ~Ast() { }

	AstType type = AST_NONE;
    uint32_t line = 0;
    const char* file;
};

struct Ast_Expression : Ast {
    Ast_Expression() { type = AST_EXPRESSION; }
    ~Ast_Expression() { }
};

struct Ast_FunctionCall {
    Ast_FunctionCall(const char* ident, const Vector<Ast_Expression*>& args) : ident(ident), args(args) { }
    ~Ast_FunctionCall() { 
        for (auto& arg : args) delete_expression(arg);
    }
    const char* ident;
    Vector<Ast_Expression*> args;
};

struct Ast_Cast {
    Ast_Cast(Ast_Expression* expression, AstDataType cast_type) : cast_type(cast_type), expression(expression) { }
    ~Ast_Cast() {
        delete_expression(expression);
    }
    Ast_Expression* expression = nullptr;
    AstDataType cast_type = AST_TYPE_NONE;
};

struct Ast_PrimaryExpression : public Ast_Expression {
    Ast_PrimaryExpression() { type = AST_PRIMARY; }
    ~Ast_PrimaryExpression() {
        switch (prim_type) {
        case AST_PRIM_NESTED:    delete nested; break;
        case AST_PRIM_CAST:      delete cast;   break;
        case AST_PRIM_CALL:      delete call;   break;
        default:                                break;
        }
    }

    AstPrimaryType prim_type = AST_PRIM_NONE;
    AstDataType type_value = AST_TYPE_NONE;
    
    union {
        int         int_const;
        float       float_const;
        const char* ident;
        const char* string;
        char        char_const;
        bool        boolean;
        
        Ast_FunctionCall* call;
        Ast_Expression*   nested;
        Ast_Cast*         cast;
    };
};

struct Ast_BinaryExpression : public Ast_Expression {
    Ast_BinaryExpression() { type = AST_BINARY; }
    Ast_BinaryExpression(Ast_Expression* left, AstOperatorType op, Ast_Expression* right) 
        : left(left), op(op), right(right) { type = AST_BINARY; }
    ~Ast_BinaryExpression() {
        delete_expression(left);
        delete_expression(right);
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
    ~Ast_UnaryExpression() {
        delete_expression(next);
    }
    Ast_Expression* next = nullptr;
    AstUnaryType op = AST_UNARY_NONE;
};

struct Ast_Assignment : public Ast_Expression {
    Ast_Assignment() { type = AST_ASSIGNMENT; }
    Ast_Assignment(Ast_Expression* expression, Ast_Expression* id, AstEqualType equal_type = AST_EQUAL) : expression(expression), id(id), equal_type(equal_type) { type = AST_ASSIGNMENT; }
    ~Ast_Assignment() {
        delete_expression(expression);
    }
    AstEqualType equal_type = AST_EQUAL;
    Ast_Expression* id = nullptr;
    Ast_Expression* expression = nullptr;
};

struct Ast_Decleration : public Ast {
    Ast_Decleration() { type = AST_DECLERATION; }
    ~Ast_Decleration() { } 
};

struct Ast_Statement : public Ast_Decleration {
    Ast_Statement() { type = AST_STATEMENT; }
    ~Ast_Statement() { }
};

struct Ast_Scope : public Ast_Statement {
    Ast_Scope() { type = AST_SCOPE; }
    ~Ast_Scope() {
        for (auto& dec : declerations) delete_ast(dec);
    }

    Vector<Ast_Decleration*> declerations;
};

struct Ast_ExpressionStatement : public Ast_Statement {
    Ast_ExpressionStatement(Ast_Expression* expression) : expression(expression) { type = AST_EXPRESSION_STATEMENT; }
    ~Ast_ExpressionStatement() {
        delete_expression(expression);
    }
    Ast_Expression* expression = nullptr;
};

struct Ast_PrintStatement : public Ast_Statement {
    Ast_PrintStatement(const Vector<Ast_Expression*>& expressions) : expressions(expressions) { type = AST_PRINT; }
    ~Ast_PrintStatement() {
        for (auto& expr : expressions) delete_expression(expr);
    }
    Vector<Ast_Expression*> expressions;
};

struct Ast_ConditionalStatement : public Ast_Statement {
    Ast_ConditionalStatement() { type = AST_CONDITIONAL; }
    Ast_ConditionalStatement(Ast_Expression* condition, Ast_Scope* scope) : condition(condition), scope(scope) { type = AST_CONDITIONAL; }
    ~Ast_ConditionalStatement() {
        delete_expression(condition);
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

struct Ast_WhileLoop : Ast_ConditionalStatement {
    Ast_WhileLoop() { type = AST_WHILE; }
    Ast_WhileLoop(Ast_Expression* condition, Ast_Scope* scope) : Ast_ConditionalStatement(condition, scope) { type = AST_WHILE; }
    ~Ast_WhileLoop() { }
};

struct Ast_ReturnStatement : Ast_Statement {
    Ast_ReturnStatement(Ast_Expression* expression) : expression(expression) { type = AST_RETURN; }
    ~Ast_ReturnStatement() {
        delete_expression(expression);
    }
    Ast_Expression* expression = nullptr;
};

struct Ast_VarDecleration : public Ast_Decleration {
    Ast_VarDecleration() { type = AST_VAR_DECLERATION; }
    Ast_VarDecleration(const char* ident, Ast_Expression* expression, AstDataType type_value, AstSpecifierType specifiers) 
        : ident(ident), expression(expression), type_value(type_value), specifiers(specifiers) { type = AST_VAR_DECLERATION; }
    ~Ast_VarDecleration() {
        delete_expression(expression);
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
    ~Ast_Function() {
        for (auto& arg : args) delete arg;
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
        for (auto& dec : declerations) delete_ast(dec);
    }

    Vector<Ast_Decleration*> declerations;
};

void delete_ast(Ast_Decleration* ast) {
    switch (ast->type) {
    case AST_EXPRESSION_STATEMENT: delete AST_CAST(Ast_ExpressionStatement, ast); break;
    case AST_FUNCTION:             delete AST_CAST(Ast_Function, ast);            break;
    case AST_PRINT:                delete AST_CAST(Ast_PrintStatement, ast);      break;
    case AST_SCOPE:                delete AST_CAST(Ast_Scope, ast);               break;
    case AST_VAR_DECLERATION:      delete AST_CAST(Ast_VarDecleration, ast);      break;
    case AST_IF:                   delete AST_CAST(Ast_IfStatement, ast);         break;
    case AST_ELIF:                 delete AST_CAST(Ast_ElifStatement, ast);       break;
    case AST_ELSE:                 delete AST_CAST(Ast_ElseStatement, ast);       break;
    case AST_WHILE:                delete AST_CAST(Ast_WhileLoop, ast);           break;
    case AST_RETURN:               delete AST_CAST(Ast_ReturnStatement, ast);     break;
    default:                       delete ast;                                    break;        
    }
}

void delete_expression(Ast_Expression* expr) {
    switch (expr->type) {
    case AST_PRIMARY:    delete AST_CAST(Ast_PrimaryExpression, expr); break;
    case AST_BINARY:     delete AST_CAST(Ast_BinaryExpression, expr);  break;
    case AST_UNARY:      delete AST_CAST(Ast_UnaryExpression, expr);   break;
    case AST_ASSIGNMENT: delete AST_CAST(Ast_Assignment, expr);        break;
    default:             delete expr;                                  break;
    }
}



#endif //!AST_H