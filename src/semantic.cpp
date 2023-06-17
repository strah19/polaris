#include "semantic.h"
#include "error.h"

void check_variable_decleration(Ast_VarDecleration* decleration);
void check_function_decleration(Ast_Function* function);
void check_return_statement(Ast_ReturnStatement* return_statement);
void check_print_statement(Ast_PrintStatement* print_statement);
void check_scope(Ast_Scope* scope);
void check_ast(Ast* ast);
void check_condition(Ast_ConditionalStatement* conditional_statement);
void check_while(Ast_WhileStatement* while_statement);

void check_expression(Ast_Expression* expression, AstDataType* current_expr_type, AstDataType can_it_be = AST_TYPE_NONE);

void report_semantic_error(Ast* ast, const char* msg);

void semantic_checker(Ast_TranslationUnit* root) {
    for (int i = 0; i < root->declerations.size(); i++) {
        Ast* ast = root->declerations[i];
        check_ast(ast);
    }
}

void check_variable_decleration(Ast_VarDecleration* decleration) {
    if (decleration->expression) {
        AstDataType expr_type = get_expression_type(decleration->expression, decleration->type_value);
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
    else if (ast->type == AST_EXPRESSION_STATEMENT)
        get_expression_type(AST_CAST(Ast_ExpressionStatement, ast)->expression);
    else if (ast->type == AST_PRINT)
        check_print_statement(AST_CAST(Ast_PrintStatement, ast));
    else if (ast->type == AST_FUNCTION) 
        check_function_decleration(AST_CAST(Ast_Function, ast));
    else if (ast->type == AST_WHILE) 
        check_while(AST_CAST(Ast_WhileStatement, ast));
    else if (ast->type == AST_IF)
        check_condition(AST_CAST(Ast_IfStatement, ast));
}

void check_condition(Ast_ConditionalStatement* conditional_statement) {
    if (conditional_statement->condition)
        get_expression_type(conditional_statement->condition);
    check_scope(conditional_statement->scope);
    if (conditional_statement->next)
        check_condition(conditional_statement->next);
}

void check_while(Ast_WhileStatement* while_statement) {
    get_expression_type(while_statement->condition);
    check_scope(while_statement->scope);
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

void check_print_statement(Ast_PrintStatement* print_statement) {
    for (auto& expr : print_statement->expressions) {
        get_expression_type(expr);
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

AstDataType get_expression_type(Ast_Expression* expression) {

}

void check_expression(Ast_Expression* expression, AstDataType* current_expr_type, AstDataType can_it_be) {

}