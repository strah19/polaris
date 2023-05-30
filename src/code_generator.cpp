#include "code_generator.h"
#include <string.h>

CodeGenerator::CodeGenerator(Ast_TranslationUnit* root, Vector<int>* function_indices, Scope* scope) : root(root), function_indices(function_indices), scope(scope) { }

CodeGenerator::~CodeGenerator() {
    bytecode_free(&bytecode);
}

void CodeGenerator::run() {
    bytecode_init(&bytecode);
    
    bytecode.start_address = bytecode.count;
    for (int i = 0; i < root->declerations.size(); i++) {
        Ast* ast = root->declerations[i];
        generate_from_ast(ast);
    }   

    bytecode_write(OP_HALT, 0, &bytecode);
}

void CodeGenerator::generate_from_ast(Ast* ast) {
    if (ast->type == AST_VAR_DECLERATION) 
        generate_variable_decleration(AST_CAST(Ast_VarDecleration, ast));
    if (ast->type == AST_FUNCTION) 
        generate_function(AST_CAST(Ast_Function, ast));
    else if (ast->type == AST_PRINT)
        generate_print_statement(AST_CAST(Ast_PrintStatement, ast));
    else if (ast->type == AST_EXPRESSION_STATEMENT)
        generate_expression(AST_CAST(Ast_ExpressionStatement, ast)->expression);
    else if (ast->type == AST_SCOPE)
        generate_scope(AST_CAST(Ast_Scope, ast));
    else if (ast->type == AST_RETURN) 
        generate_return_statement(AST_CAST(Ast_ReturnStatement, ast));
    else if (ast->type == AST_IF) 
        generate_if_statement(AST_CAST(Ast_IfStatement, ast));
}

void CodeGenerator::generate_function(Ast_Function* function) {
    
}

void CodeGenerator::generate_if_statement(Ast_IfStatement* if_statement) {
    
}

int CodeGenerator::generate_conditional_statement(Ast_ConditionalStatement* conditional) {
    int start_address = bytecode.count;
 
    return start_address;
}

void CodeGenerator::generate_scope(Ast_Scope* scope) {
    for (int i = 0; i < scope->declerations.size(); i++) {
        Ast* ast = scope->declerations[i];
        generate_from_ast(ast);
    }
}

void CodeGenerator::generate_variable_decleration(Ast_VarDecleration* decleration) {
    generate_expression(decleration->expression);
    references[decleration->ident] = max_references_address++;
    write(OP_CONST, decleration);
    write_constant(INT_VALUE(references[decleration->ident]), decleration); //writes the address of the references
    write(OP_GSTORE, decleration);
}

void CodeGenerator::generate_print_statement(Ast_PrintStatement* print_statement) {
    generate_expression(print_statement->expression);
    write(OP_PRINT, print_statement);
}

void CodeGenerator::generate_return_statement(Ast_ReturnStatement* return_statement) {
  
}

void CodeGenerator::generate_expression(Ast_Expression* expression) {
    if (expression->type == AST_UNARY) {
        auto unary = AST_CAST(Ast_UnaryExpression, expression);
        generate_expression(unary->next);
        switch (unary->op) {
        case AST_UNARY_MINUS: write(OP_NEGATE, unary); break;
        }
    }
    else if (expression->type == AST_BINARY) {
        auto bin = AST_CAST(Ast_BinaryExpression, expression);
        generate_expression(bin->left);
        generate_expression(bin->right);

        uint8_t op;
        switch (bin->op) {
        case AST_OPERATOR_MULTIPLICATIVE:        op = OP_MUL; break;
        case AST_OPERATOR_DIVISION:              op = OP_DIV; break;
        case AST_OPERATOR_MODULO:                op = OP_MOD; break;
        case AST_OPERATOR_ADD:                   op = OP_ADD; break;
        case AST_OPERATOR_SUB:                   op = OP_MIN; break;
        case AST_OPERATOR_COMPARITIVE_EQUAL:     op = OP_EQL; break;
        case AST_OPERATOR_COMPARITIVE_NOT_EQUAL: op = OP_NEQ; break;
        case AST_OPERATOR_LTE:                   op = OP_LTE; break;
        case AST_OPERATOR_GTE:                   op = OP_GTE; break;
        case AST_OPERATOR_LT:                    op = OP_LT;  break;
        case AST_OPERATOR_GT:                    op = OP_GT;  break;
        case AST_OPERATOR_AND:                   op = OP_AND; break;
        case AST_OPERATOR_OR:                    op = OP_OR;  break;
        case AST_OPERATOR_BIT_XOR:               op = OP_XOR; break;
        case AST_OPERATOR_BIT_OR:                op = OP_OR;  break;
        case AST_OPERATOR_BIT_AND:               op = OP_BAN; break;
        case AST_OPERATOR_LSHIFT:                op = OP_LSF; break;
        case AST_OPERATOR_RSHIFT:                op = OP_RSF; break;
        }
        write(op, bin);
    }
    else if (expression->type == AST_PRIMARY) {
        auto prim = AST_CAST(Ast_PrimaryExpression, expression);
        if (prim->prim_type == AST_PRIM_DATA) {
            write(OP_CONST, prim);
            switch (prim->type_value) {
            case AST_TYPE_INT:     write_constant(INT_VALUE(prim->int_const), prim);     break;
            case AST_TYPE_FLOAT:   write_constant(FLOAT_VALUE(prim->float_const), prim); break;
            case AST_TYPE_BOOLEAN: write_constant(BOOLEAN_VALUE(prim->boolean), prim);   break;
            case AST_TYPE_STRING:  write_constant(OBJ_VALUE(allocate_string(prim->string)), prim); break;
            }
        }
        else if (prim->prim_type == AST_PRIM_NESTED) {
            generate_expression(prim->nested);
        }
        else if (prim->prim_type == AST_PRIM_ID) {
            write(OP_CONST, prim);
            write_constant(INT_VALUE(references[prim->ident]), prim); //writes the address of the references
            write(OP_GLOAD, prim);
        }
        else if (prim->prim_type == AST_PRIM_CALL) {
            
        }
    }
    else if (expression->type == AST_ASSIGNMENT) {
        auto assign = AST_CAST(Ast_Assignment, expression);
        generate_expression(assign->value);

        write(OP_CONST, assign);
        write_constant(INT_VALUE(references[AST_CAST(Ast_PrimaryExpression, assign->id)->ident]), assign); //writes the address of the references
        write(OP_GSTORE, assign);

        if (assign->next)
            generate_expression(assign->next);
    }
}

ObjString* CodeGenerator::allocate_string(const char* str) {
    ObjString* str_obj = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    str_obj->len = strlen(str);
    str_obj->chars = (char*) malloc(str_obj->len + 1);
    memcpy(str_obj->chars, str, str_obj->len + 1);
    return str_obj;
}

void CodeGenerator::write(uint8_t opcode, Ast* ast) {
    bytecode_write(opcode, ast->line, &bytecode);
}

void CodeGenerator::write_constant(Value value, Ast* ast) {
    write(bytecode_add_constant(value, &bytecode), ast);
}