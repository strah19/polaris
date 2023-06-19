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
bool can_convert(AstDataType type1, AstDataType type2);

void report_semantic_error(Ast* ast, const char* msg);

void semantic_checker(Ast_TranslationUnit* root) {
    for (int i = 0; i < root->declerations.size(); i++) {
        Ast* ast = root->declerations[i];
        check_ast(ast);
    }
}

void check_variable_decleration(Ast_VarDecleration* decleration) {
    if (decleration->expression) {
        AstDataType expr_type = AST_TYPE_NONE;
        check_expression(decleration->expression, &expr_type, decleration->type_value);
        if (!can_convert(decleration->type_value, expr_type)) {
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
        if (!can_convert(expr_type, return_statement->expected_return_type)) {
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
    AstDataType type = AST_TYPE_NONE;
    check_expression(expression, &type);
    return type;
}

static const AstDataType STD_CONVERSION_TABLE[6][6] = {
    AST_TYPE_NONE, AST_TYPE_NONE,    AST_TYPE_NONE,    AST_TYPE_NONE,    AST_TYPE_NONE,   AST_TYPE_NONE,
    AST_TYPE_NONE, AST_TYPE_FLOAT,   AST_TYPE_BOOLEAN, AST_TYPE_FLOAT,   AST_TYPE_NONE,   AST_TYPE_FLOAT,
    AST_TYPE_NONE, AST_TYPE_BOOLEAN, AST_TYPE_BOOLEAN, AST_TYPE_BOOLEAN, AST_TYPE_NONE,   AST_TYPE_BOOLEAN,
    AST_TYPE_NONE, AST_TYPE_FLOAT,   AST_TYPE_BOOLEAN, AST_TYPE_INT,     AST_TYPE_NONE,   AST_TYPE_CHAR,
    AST_TYPE_NONE, AST_TYPE_NONE,    AST_TYPE_NONE,    AST_TYPE_NONE,    AST_TYPE_STRING, AST_TYPE_NONE,
    AST_TYPE_NONE, AST_TYPE_FLOAT,   AST_TYPE_BOOLEAN, AST_TYPE_CHAR,    AST_TYPE_NONE,   AST_TYPE_CHAR
};

void convert_primary(Ast_PrimaryExpression* primary, AstDataType new_type) {
    if (primary->type_value == new_type) return;
    if (primary->type_value == AST_TYPE_INT && new_type == AST_TYPE_FLOAT) {
        primary->float_const = primary->int_const;
    }
    else if (primary->type_value == AST_TYPE_FLOAT && new_type == AST_TYPE_INT) {
        primary->int_const = primary->float_const;
    }
}

void check_expression(Ast_Expression* expression, AstDataType* current_expr_type, AstDataType can_it_be) {
    if (expression->type == AST_BINARY) {
        auto bin = AST_CAST(Ast_BinaryExpression, expression);
        check_expression(bin->left, current_expr_type, can_it_be);
        check_expression(bin->right, current_expr_type, can_it_be);

        //Check string stuff here...strings can only do '+' and comparative operands.
    }
    else if (expression->type == AST_UNARY) {
        auto unary = AST_CAST(Ast_UnaryExpression, expression);
        check_expression(unary->next, current_expr_type, can_it_be);
    }
    else if (expression->type == AST_ASSIGNMENT) {
        Ast_Assignment* assign = AST_CAST(Ast_Assignment, expression);
        check_expression(assign->id, current_expr_type, can_it_be);
        AstDataType id_type = *current_expr_type;
        AstDataType value_type = id_type;
        check_expression(assign->value, &value_type, id_type);

        if (assign->next) {
            value_type = AST_TYPE_NONE;
            check_expression(assign->next, &value_type, id_type);
        }
    }
    else if (expression->type == AST_PRIMARY) {
        auto primary = AST_CAST(Ast_PrimaryExpression, expression);
        if (primary->prim_type == AST_PRIM_DATA || primary->prim_type == AST_PRIM_ID || primary->prim_type == AST_PRIM_CALL) {
            if (can_it_be != AST_TYPE_NONE) {
                if (can_convert(can_it_be, primary->type_value)) {
                    convert_primary(primary, can_it_be);
                    primary->type_value = can_it_be;
                    *current_expr_type = can_it_be;
                }
                else
                    report_semantic_error(primary, "Unable to convert types");
            }
            else {
                if (*current_expr_type == AST_TYPE_NONE) 
                    *current_expr_type = primary->type_value;
                else if (*current_expr_type != primary->type_value && !can_convert(*current_expr_type, primary->type_value)) 
                    report_semantic_error(primary, "Mismatched types! Unable to auto convert types");
                else {
                    *current_expr_type = STD_CONVERSION_TABLE[*current_expr_type][primary->type_value];
                }
            }
        }
        else if (primary->prim_type == AST_PRIM_NESTED) {
            check_expression(primary->nested, current_expr_type, can_it_be);
        }
    }
}

bool can_convert(AstDataType type1, AstDataType type2) {
    if (type1 == AST_TYPE_NONE || type2 == AST_TYPE_NONE)     return false;
    if (type1 == AST_TYPE_STRING && type2 != AST_TYPE_STRING) return false;
    if (type1 != AST_TYPE_STRING && type2 == AST_TYPE_STRING) return false;
    return true;
}