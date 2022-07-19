#ifndef AST_H
#define AST_H

#include <cinttypes>
#include <stdio.h>

enum {
    AST_EXPRESSION,
    AST_UNARY,
    AST_PRIMARY,
    AST_BINARY,
    AST_DECLERATION,
    AST_VAR_DECLERATION,
    AST_FUNCTION,
    AST_ASSIGNMENT,
    AST_STATEMENT,
    AST_PRINT,
    AST_EXPRESSION_STATEMENT,
    AST_SCOPE,
    AST_CONDITIONAL,
    AST_IF,
    AST_ELIF,
    AST_ELSE,
    AST_FOR,
    AST_WHILE,
    AST_RETURN,
    AST_TRANSLATION_UNIT
};

enum {
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
    AST_OPERATOR_BIT_LEFT,
    AST_OPERATOR_BIT_RIGHT,
    AST_OPERATOR_NONE
};

enum {
    AST_UNARY_MINUS,
    AST_UNARY_NOT,
    AST_UNARY_BIT_NOT,
    AST_UNARY_NONE
};

enum {
    AST_EQUAL,
    AST_EQUAL_PLUS,
    AST_EQUAL_MINUS,
    AST_EQUAL_MULTIPLY,
    AST_EQUAL_DIVIDE,
    AST_EQUAL_MOD
};

enum {
    AST_FLOAT,
    AST_INT,
    AST_BOOLEAN,
    AST_STRING,
    AST_NESTED,
    AST_CHAR,
    AST_INPUT,
    AST_ID,
    AST_CAST,
    AST_FUNC_CALL,
    AST_VOID,
    AST_TYPE_NONE
};

enum {
    AST_SPECIFIER_NONE  = 0x00,
    AST_SPECIFIER_CONST = 0x01
};

struct Ast;
struct Ast_Expression;
struct Ast_Scope;

struct Ast {
    Ast() { }
    ~Ast() { printf("AST\n"); }

	int type = 0;
    uint32_t line = 0;
    const char* file;
};

struct Ast_Expression : Ast {
    Ast_Expression() { type = AST_EXPRESSION; }
    ~Ast_Expression() {
        printf("Expression\n");
    }
};

struct Ast_FunctionCall {
    Ast_FunctionCall(const char* ident, const std::vector<Ast_Expression*>& args) : ident(ident), args(args) { }
    ~Ast_FunctionCall() { 
        printf("FunctionCall\n");
        for (auto& arg : args) delete arg;
    }
    const char* ident;
    std::vector<Ast_Expression*> args;
};

struct Ast_Cast {
    Ast_Cast(Ast_Expression* expression, int cast_type) : cast_type(cast_type), expression(expression) { }
    ~Ast_Cast() {
        printf("Cast\n");
        delete expression;
    }
    Ast_Expression* expression = nullptr;
    int cast_type = AST_TYPE_NONE;
};

struct Ast_PrimaryExpression : public Ast_Expression {
    Ast_PrimaryExpression() { type = AST_PRIMARY; }
    Ast_PrimaryExpression(const char* ident) : ident(ident), type_value(AST_ID) { type = AST_PRIMARY; }
    Ast_PrimaryExpression(float float_const) : float_const(float_const), type_value(AST_FLOAT) { type = AST_PRIMARY; }
    Ast_PrimaryExpression(char char_const) : char_const(char_const), type_value(AST_CHAR) { type = AST_PRIMARY; }
    ~Ast_PrimaryExpression() {
        printf("Primary\n");
        switch (type_value) {
        case AST_NESTED:    delete nested; break;
        case AST_CAST:      delete cast;   break;
        case AST_FUNC_CALL: delete call;   break;
        }
    }
    int type_value = AST_TYPE_NONE;
    int array_size = -1;

    union {
        int         int_const;
        float       float_const;
        const char* ident;
        const char* string;
        char        char_const;
        bool        boolean;
        int         input_type;
        
        Ast_FunctionCall* call;
        Ast_Expression*   nested;
        Ast_Cast*         cast;
    };
};

struct Ast_BinaryExpression : public Ast_Expression {
    Ast_BinaryExpression() { type = AST_BINARY; }
    Ast_BinaryExpression(Ast_Expression* left, int op, Ast_Expression* right) : left(left), op(op), right(right) { type = AST_BINARY; }
    ~Ast_BinaryExpression() {
        printf("Binary\n");
        delete left;
        delete right;
    }
    int op = AST_OPERATOR_NONE;

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
    Ast_UnaryExpression(Ast_Expression* next, int op) : op(op), next(next) { type = AST_UNARY; }
    ~Ast_UnaryExpression() {
        printf("Unary\n");
        delete next;
    }
    Ast_Expression* next = nullptr;
    int op = AST_UNARY_NONE;
};

struct Ast_Assignment : public Ast_Expression {
    Ast_Assignment() { type = AST_ASSIGNMENT; }
    Ast_Assignment(Ast_Expression* expression, const char* id, int equal_type = AST_EQUAL) : expression(expression), id(id), equal_type(equal_type) { type = AST_ASSIGNMENT; }
    ~Ast_Assignment() {
        printf("Assignment\n");
        delete expression;
    }
    int equal_type = AST_EQUAL;
    const char* id = nullptr;
    Ast_Expression* expression = nullptr;
};

struct Ast_Decleration : public Ast {
    Ast_Decleration() { type = AST_DECLERATION; }
    ~Ast_Decleration() { printf("Decleration\n"); }
};

struct Ast_Statement : public Ast_Decleration {
    Ast_Statement() { type = AST_STATEMENT; }
    ~Ast_Statement() { printf("Statement\n"); }
};

struct Ast_Scope : public Ast_Statement {
    Ast_Scope() { type = AST_SCOPE; }
    ~Ast_Scope() {
        printf("Scope\n");
        for (auto& dec : declerations) delete dec;
    }

    std::vector<Ast_Decleration*> declerations;
};

struct Ast_ExpressionStatement : public Ast_Statement {
    Ast_ExpressionStatement(Ast_Expression* expression) : expression(expression) { type = AST_EXPRESSION_STATEMENT; }
    ~Ast_ExpressionStatement() {
        printf("ExpressionStatement\n");
        delete expression;
    }
    Ast_Expression* expression = nullptr;
};

struct Ast_PrintStatement : public Ast_Statement {
    Ast_PrintStatement(const std::vector<Ast_Expression*>& expressions) : expressions(expressions) { type = AST_PRINT; }
    ~Ast_PrintStatement() {
        printf("PrintStatement\n");
        for (auto& expr : expressions) delete expr;
    }
    std::vector<Ast_Expression*> expressions;
};

struct Ast_ConditionalStatement : public Ast_Statement {
    Ast_ConditionalStatement() { type = AST_CONDITIONAL; }
    Ast_ConditionalStatement(Ast_Expression* condition, Ast_Scope* scope) : condition(condition), scope(scope) { type = AST_CONDITIONAL; }
    ~Ast_ConditionalStatement() {
        printf("Conditional\n");
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
    ~Ast_IfStatement() { printf("If\n"); }
};

struct Ast_ElifStatement : Ast_ConditionalStatement {
    Ast_ElifStatement() { type = AST_ELIF; }
    Ast_ElifStatement(Ast_Expression* condition, Ast_Scope* scope) : Ast_ConditionalStatement(condition, scope) { type = AST_ELIF; }
    ~Ast_ElifStatement() { printf("Elif\n"); }
};

struct Ast_ElseStatement : Ast_ConditionalStatement {
    Ast_ElseStatement() { type = AST_ELSE; }
    Ast_ElseStatement(Ast_Scope* scope) : Ast_ConditionalStatement(nullptr, scope) { type = AST_ELSE; }
    ~Ast_ElseStatement() { printf("Else\n"); }
};

struct Ast_WhileLoop : Ast_ConditionalStatement {
    Ast_WhileLoop() { type = AST_WHILE; }
    Ast_WhileLoop(Ast_Expression* condition, Ast_Scope* scope) : Ast_ConditionalStatement(condition, scope) { type = AST_WHILE; }
    ~Ast_WhileLoop() { printf("While\n"); }
};

struct Ast_VarDecleration : public Ast_Decleration {
    Ast_VarDecleration() { type = AST_VAR_DECLERATION; }
    Ast_VarDecleration(const char* ident, Ast_Expression* expression, int type_value, int specifiers) 
        : ident(ident), expression(expression), type_value(type_value), specifiers(specifiers) { type = AST_VAR_DECLERATION; }
    ~Ast_VarDecleration() {
        printf("VarDec\n");
        delete expression;
    }

    int type_value = AST_TYPE_NONE;
    int specifiers = AST_SPECIFIER_NONE;
    const char* ident = nullptr;

    Ast_Expression* expression = nullptr;
};

struct Ast_Function : public Ast_Decleration {
    Ast_Function() { type = AST_FUNCTION; }
    Ast_Function(const char* ident, int return_type, const std::vector<Ast_VarDecleration*> args, Ast_Scope* scope) : 
        ident(ident), return_type(return_type), args(args), scope(scope) { type = AST_FUNCTION; }
    ~Ast_Function() {
        printf("Function\n");
        for (auto& arg : args) delete arg;
        delete scope;
    }


    const char* ident = nullptr;
    int return_type = AST_VOID;
    std::vector<Ast_VarDecleration*> args;
    Ast_Scope* scope = nullptr;
};

struct Ast_ReturnStatement : Ast_Statement {
    Ast_ReturnStatement(Ast_Expression* expression) : expression(expression) { type = AST_RETURN; }
    ~Ast_ReturnStatement() {
        printf("Return\n");
        delete expression;
    }
    Ast_Expression* expression = nullptr;
};

struct Ast_TranslationUnit : public Ast {
    Ast_TranslationUnit() { type = AST_TRANSLATION_UNIT; }
    ~Ast_TranslationUnit() {
        printf("Translation\n");
        for (auto& dec : declerations) delete dec;
    }

    std::vector<Ast_Decleration*> declerations;
};

#define AST_DELETE(type) delete type

#define AST_CAST(type, base) static_cast<type*>(base)

#endif //!AST_H