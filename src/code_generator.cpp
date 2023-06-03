#include "code_generator.h"
#include <string.h>

CodeGenerator::CodeGenerator(Ast_TranslationUnit* root, Vector<int>* function_indices, Scope* scope) : root(root), function_indices(function_indices), scope(scope) { }

CodeGenerator::~CodeGenerator() {
    bytecode_free(&bytecode);
}

void CodeGenerator::run() {
    bytecode_init(&bytecode);
    
    for (auto& function : *function_indices) {
        generate_function(AST_CAST(Ast_Function, root->declerations[function]));
    }

    bytecode.start_address = bytecode.count;
    for (int i = 0; i < root->declerations.size(); i++) {
        Ast* ast = root->declerations[i];
        if (ast->type != AST_FUNCTION)
            generate_from_ast(ast);
    }   

    bytecode_write(OP_HALT, 0, &bytecode);
}

void CodeGenerator::generate_from_ast(Ast* ast) {
    if (ast->type == AST_VAR_DECLERATION) 
        generate_variable_decleration(AST_CAST(Ast_VarDecleration, ast));
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
    else if (ast->type == AST_WHILE)
        generate_while_statement(AST_CAST(Ast_WhileStatement, ast));
}

void CodeGenerator::generate_function(Ast_Function* function) {
    function_pointers[function->ident] = bytecode.count;  

    generate_scope(function->scope);
    if (bytecode.code[bytecode.count - 1] != OP_RETV)
        write(OP_RET, function);
}

void CodeGenerator::generate_if_statement(Ast_IfStatement* if_statement) {
    generate_conditional_statement(if_statement);
}

int CodeGenerator::generate_conditional_statement(Ast_ConditionalStatement* conditional) {
    int start_address = bytecode.count;
    if (conditional->condition) {
        generate_expression(conditional->condition);
    }

    int skip_condition_location = -1;
    if (conditional->type == AST_IF || conditional->type == AST_ELIF) {
        write(OP_JMPN, conditional);
        skip_condition_location = bytecode.count;
        write(0x00, conditional); //lower
    }

    generate_scope(conditional->scope);
    int go_to_end_location = -1;
    if (conditional->type == AST_IF || conditional->type == AST_ELIF) {
        write(OP_JMP, conditional);
        go_to_end_location = bytecode.count;
        write(0x00, conditional); //higher
    }   

    if (conditional->next) {
        int skip_condition_address = generate_conditional_statement(conditional->next);
        bytecode.code[skip_condition_location] = skip_condition_address;
    }
    else if (conditional->type == AST_IF || conditional->type == AST_ELIF) {
        int skip_address = bytecode.count;
        bytecode.code[skip_condition_location] = skip_address;
    }

    if (go_to_end_location != -1) {
        int skip_address = bytecode.count;
        bytecode.code[go_to_end_location] = skip_address;
    }
    return start_address;
}

void CodeGenerator::generate_while_statement(Ast_WhileStatement* while_statement) {
    uint32_t return_location = bytecode.count;
    generate_expression(while_statement->condition);

    write(OP_JMPN, while_statement);
    uint32_t skip_location = bytecode.count;
    write(0x00, while_statement);

    generate_scope(while_statement->scope);

    write(OP_JMP, while_statement);
    write(return_location, while_statement);
    bytecode.code[skip_location] = bytecode.count;
}

void CodeGenerator::generate_scope(Ast_Scope* scope) {
    for (int i = 0; i < scope->declerations.size(); i++) {
        Ast* ast = scope->declerations[i];
        generate_from_ast(ast);
    }
}

void CodeGenerator::generate_variable_decleration(Ast_VarDecleration* decleration) {
    generate_expression(decleration->expression);
    if (!references[decleration->ident].init)
        references[decleration->ident] = Reference(max_references_address++);
    write(OP_GSTORE, decleration);
    write(references[decleration->ident].address, decleration); //writes the address of the references
}

void CodeGenerator::generate_print_statement(Ast_PrintStatement* print_statement) {
    generate_expression(print_statement->expression);
    write(OP_PRINT, print_statement);
}

void CodeGenerator::generate_return_statement(Ast_ReturnStatement* return_statement) {
    generate_expression(return_statement->expression);
    write(OP_RETV, return_statement);
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
            if (prim->local) {
                write(OP_LOAD, prim);
                write(-3 - prim->local_index, prim);
            }
            else {
                write(OP_GLOAD, prim);
                if (!references[prim->ident].init) 
                    references[prim->ident] = Reference(max_references_address++);
                write(references[prim->ident].address, prim); //writes the address of the references
            }

        }
        else if (prim->prim_type == AST_PRIM_CALL) {
            for (int i = prim->call->args.size() - 1; i >= 0 ; i--) {
                generate_expression(prim->call->args[i]);
            }

            write(OP_CALL, prim);
            write(function_pointers[prim->call->ident], prim);
            write((uint32_t) prim->call->args.size(), prim);
        }
    }
    else if (expression->type == AST_ASSIGNMENT) {
        auto assign = AST_CAST(Ast_Assignment, expression);
        generate_expression(assign->value);

        auto assign_id = AST_CAST(Ast_PrimaryExpression, assign->id);
        if (assign_id->local) {
            write(OP_STORE, assign);
            write(-3 - assign_id->local_index, assign);
        }
        else {
            write(OP_GSTORE, assign);
            write(references[AST_CAST(Ast_PrimaryExpression, assign->id)->ident].address, assign); //writes the address of the references
        }

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

void CodeGenerator::write(uint32_t opcode, Ast* ast) {
    bytecode_write(opcode, ast->line, &bytecode);
}

void CodeGenerator::write_constant(Value value, Ast* ast) {
    write(bytecode_add_constant(value, &bytecode), ast);
}