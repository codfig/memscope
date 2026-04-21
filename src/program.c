#include "program.h"

#include <stdio.h>
#include <string.h>

static void operation_text(const Operation *op, char *buffer, size_t buffer_size) {
    switch (op->kind) {
    case OP_ASSIGN_INT:
        snprintf(buffer, buffer_size, "%s = %d", op->dest, op->number);
        break;
    case OP_ASSIGN_SUM:
        snprintf(buffer, buffer_size, "%s = %s + %s", op->dest, op->lhs, op->rhs);
        break;
    case OP_CALL:
        snprintf(buffer, buffer_size, "%s()", op->callee);
        break;
    case OP_ASSIGN_CALL:
        snprintf(buffer, buffer_size, "%s = %s()", op->dest, op->callee);
        break;
    case OP_RETURN_VOID:
        snprintf(buffer, buffer_size, "return");
        break;
    case OP_RETURN_VALUE:
        snprintf(buffer, buffer_size, "return %s", op->lhs);
        break;
    default:
        snprintf(buffer, buffer_size, "?");
        break;
    }
}

static void operation_bytecode(const Operation *op, char *buffer, size_t buffer_size) {
    switch (op->kind) {
    case OP_ASSIGN_INT:
        snprintf(buffer, buffer_size, "01 <%02d> <%02d> <%02d>", op->bytecode_address / 100, op->bytecode_address % 100, op->number);
        break;
    case OP_ASSIGN_SUM:
        snprintf(buffer, buffer_size, "02 <%s> <%s> <%s>", op->dest, op->lhs, op->rhs);
        break;
    case OP_CALL:
        snprintf(buffer, buffer_size, "03 <%s>", op->callee);
        break;
    case OP_ASSIGN_CALL:
        snprintf(buffer, buffer_size, "04 <%s> <%s>", op->dest, op->callee);
        break;
    case OP_RETURN_VOID:
        snprintf(buffer, buffer_size, "05");
        break;
    case OP_RETURN_VALUE:
        snprintf(buffer, buffer_size, "06 <%s>", op->lhs);
        break;
    default:
        snprintf(buffer, buffer_size, "?");
        break;
    }
}

void program_print_source(const Program *program) {
    int line = 1;
    int i;
    int j;

    printf("Program (source)\n");
    printf("----------------\n");

    for (i = 0; i < program->global_count; ++i) {
        printf("%2d | var %s\n", line++, program->globals[i].name);
    }

    for (i = 0; i < program->function_count; ++i) {
        const Function *function = &program->functions[i];
        printf("%2d | func %s()\n", line++, function->name);
        for (j = 0; j < function->local_count; ++j) {
            printf("%2d |   var %s\n", line++, function->locals[j].name);
        }
        for (j = 0; j < function->operation_count; ++j) {
            char text[64];
            operation_text(&function->operations[j], text, sizeof(text));
            printf("%2d |   %s\n", line++, text);
        }
    }

    printf("\nProgram ID: %u\n", program->program_id);
}

void program_print_bytecode(const Program *program) {
    int i;
    int j;

    printf("\nProgram (bytecode)\n");
    printf("------------------\n");
    for (i = 0; i < program->function_count; ++i) {
        const Function *function = &program->functions[i];
        printf("-> %s @ %d\n", function->name, function->code_address);
        for (j = 0; j < function->operation_count; ++j) {
            char text[64];
            operation_bytecode(&function->operations[j], text, sizeof(text));
            printf("   %4d | %s\n", function->operations[j].bytecode_address, text);
        }
    }
}
