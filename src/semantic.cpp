#include "semantic.h"
#include "error.h"

void check_variable_decleration(Ast_VarDecleration* decleration);
void check_function_decleration(Ast_Function* function);
void check_return_statement(Ast_ReturnStatement* return_statement);
void check_scope(Ast_Scope* scope);
void check_ast(Ast* ast);

bool can_type_be_this(AstDataType type, AstDataType trying);

void check_expression(Ast_Expression* expression, AstDataType* current_expr_type);
void compare_current_type(Ast* ast, AstDataType type, AstDataType* current_expr_type);

void report_semantic_error(Ast* ast, const char* msg);

void semantic_checker(Ast_TranslationUnit* root) {
    for (int i = 0; i < root->declerations.size(); i++) {
        Ast* ast = root->declerations[i];
        check_ast(ast);
    }
}

void check_variable_decleration(Ast_VarDecleration* decleration) {
    if (decleration->expression) {
        AstDataType expr_type = get_expression_type(decleration->expression);
        if (!can_type_be_this(decleration->type_value, expr_type)) {
            report_semantic_error(decleration, "Type in expression does not match variable decleration type");
        }
    }
}

void check_function_decleration(Ast_Function* function) {
    check_scope(function->scope);
}

void check_scope(Ast_Scope* scope) {
    for (int i = 0; i < scope->declerations.size(); i++) {
        Ast* ast = scope->declerations[i];
        check_ast(ast);
    }
}

void check_ast(Ast* ast) {
    if (ast->type == AST_VAR_DECLERATION)
        check_variable_decleration(AST_CAST(Ast_VarDecleration, ast));
    else if (ast->type == AST_RETURN)
        check_return_statement(AST_CAST(Ast_ReturnStatement, ast));
    else if (ast->type == AST_EXPRESSION)
        get_expression_type(AST_CAST(Ast_Expression, ast));
    else if (ast->type == AST_EXPRESSION_STATEMENT)
        get_expression_type(AST_CAST(Ast_ExpressionStatement, ast)->expression);
    else if (ast->type == AST_PRINT)
        get_expression_type(AST_CAST(Ast_PrintStatement, ast)->expression);
}

void check_return_statement(Ast_ReturnStatement* return_statement) {
    if (!return_statement->expression && return_statement->expected_return_type != AST_TYPE_VOID) {
        report_semantic_error(return_statement, "Return statement expected an expression");
        return;
    }

    if (return_statement->expected_return_type == AST_TYPE_VOID && return_statement->expression) {
        report_semantic_error(return_statement, "No return expression is allowed as function is type void");
        return;
    }
    if (return_statement->expression) {
        AstDataType expr_type = get_expression_type(return_statement->expression);
        if (expr_type != return_statement->expected_return_type) {
            printf("Mismatched %d and %d\n", expr_type, return_statement->expected_return_type);
            report_semantic_error(return_statement, "Type in expression does not match return type");
        }
    }
}

int error_count = 0;
void report_semantic_error(Ast* ast, const char* msg) {
    report_error("In file '%s' on line %d, '%s'.\n", ast->file, ast->line, msg);
    error_count++;
}

int semantic_error_count() {
    return error_count;
}

bool can_type_be_this(AstDataType type, AstDataType trying) {
    if (type == trying) return true;
    if (type == AST_TYPE_FLOAT && trying == AST_TYPE_INT) return true;
    else return false;
}

AstDataType get_expression_type(Ast_Expression* expression) {
    AstDataType current_expr_type = AST_TYPE_NONE;
    check_expression(expression, &current_expr_type);
    return current_expr_type;
}

void check_expression(Ast_Expression* expression, AstDataType* current_expr_type) {
    switch (expression->type) {
    case AST_BINARY: {
        Ast_BinaryExpression* bin = AST_CAST(Ast_BinaryExpression, expression);
        check_expression(bin->left, current_expr_type);
        AstDataType left = *current_expr_type;
        check_expression(bin->right, current_expr_type);
        if ((left == AST_TYPE_STRING || *current_expr_type == AST_TYPE_STRING) && bin->op != AST_OPERATOR_ADD) {
            report_semantic_error(bin, "Binary operations cannot be conducted on strings");
        }
        break;
    }
    case AST_UNARY: {
        Ast_UnaryExpression* unary = AST_CAST(Ast_UnaryExpression, expression);
        check_expression(unary->next, current_expr_type);
        break;
    }
    case AST_ASSIGNMENT: {
        Ast_Assignment* assign = AST_CAST(Ast_Assignment, expression);
        check_expression(assign->id, current_expr_type);
        AstDataType id_type = *current_expr_type;
        AstDataType value_type = AST_TYPE_NONE;
        check_expression(assign->value, &value_type);
        if (!can_type_be_this(id_type, value_type))
            report_semantic_error(assign, "Type does not match in assignment");
        if (assign->next) {
            value_type = AST_TYPE_NONE;
            check_expression(assign->next, &value_type);
        }
        break;
    }
    case AST_PRIMARY: {
        Ast_PrimaryExpression* prim = AST_CAST(Ast_PrimaryExpression, expression);

        switch (prim->prim_type) {
        case AST_PRIM_DATA: {
            switch (prim->type_value) {
            case AST_TYPE_INT: {
                compare_current_type(prim, AST_TYPE_INT, current_expr_type);
                break;
            }
            case AST_TYPE_FLOAT: {
                compare_current_type(prim, AST_TYPE_FLOAT, current_expr_type);
                break;
            }
            case AST_TYPE_BOOLEAN: {
                compare_current_type(prim, AST_TYPE_BOOLEAN, current_expr_type);
                break;
            }
            case AST_TYPE_STRING: {
                compare_current_type(prim, AST_TYPE_STRING, current_expr_type);
                break;
            }
            }

            break;
        }
        case AST_PRIM_ID: 
        case AST_PRIM_CALL: 
            compare_current_type(prim, prim->type_value, current_expr_type);
            break;
        case AST_PRIM_NESTED: {
            check_expression(prim->nested, current_expr_type);
        }
        }
    }
    }
}

void compare_current_type(Ast* ast, AstDataType type, AstDataType* current_expr_type) {
    if (*current_expr_type == AST_TYPE_NONE) {
        *current_expr_type = type;
    }
    else {
        if (*current_expr_type != type) {
            report_semantic_error(ast, "Mismatched type in expression");
        }
        else {
            *current_expr_type = type;
        }
    }
}