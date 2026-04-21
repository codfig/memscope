#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void copy_name(char *dest, const char *src) {
    snprintf(dest, 4, "%s", src);
}

static void setup_globals(Program *program) {
    static const char *names[] = {"a", "b", "c"};
    int i;

    program->global_count = 2;
    for (i = 0; i < program->global_count; ++i) {
        copy_name(program->globals[i].name, names[i]);
        program->globals[i].address = 4800 + i;
        program->globals[i].value = 0;
    }
}

static void setup_function(Function *function, const char *name, int code_address) {
    memset(function, 0, sizeof(*function));
    copy_name(function->name, name);
    function->code_address = code_address;
}

static void add_local(Function *function, const char *name, int address) {
    Variable *local = &function->locals[function->local_count++];
    copy_name(local->name, name);
    local->address = address;
    local->value = 0;
}

static void add_operation(Function *function, OperationKind kind, const char *dest, const char *lhs, const char *rhs, const char *callee, int number, int address, int size) {
    Operation *op = &function->operations[function->operation_count++];
    memset(op, 0, sizeof(*op));
    op->kind = kind;
    if (dest != NULL) {
        copy_name(op->dest, dest);
    }
    if (lhs != NULL) {
        copy_name(op->lhs, lhs);
    }
    if (rhs != NULL) {
        copy_name(op->rhs, rhs);
    }
    if (callee != NULL) {
        copy_name(op->callee, callee);
    }
    op->number = number;
    op->bytecode_address = address;
    op->bytecode_size = size;
}

void generator_build_program(Program *program, unsigned int seed) {
    Function *f0;
    Function *f1;
    Function *mainf;

    memset(program, 0, sizeof(*program));
    srand(seed);
    program->program_id = seed;

    setup_globals(program);

    program->function_count = 3;

    f0 = &program->functions[0];
    f1 = &program->functions[1];
    mainf = &program->functions[2];

    setup_function(f0, "f", 4864);
    add_local(f0, "x", 4856);
    add_operation(f0, OP_ASSIGN_INT, "x", NULL, NULL, NULL, 1, 4864, 4);
    add_operation(f0, OP_RETURN_VALUE, NULL, "x", NULL, NULL, 0, 4868, 3);

    setup_function(f1, "g", 4871);
    add_local(f1, "y", 4859);
    add_operation(f1, OP_ASSIGN_CALL, "y", NULL, NULL, "f", 0, 4871, 5);
    add_operation(f1, OP_RETURN_VALUE, NULL, "y", NULL, NULL, 0, 4876, 3);

    setup_function(mainf, "m4", 4879);
    add_local(mainf, "z", 4862);
    add_operation(mainf, OP_ASSIGN_CALL, "z", NULL, NULL, "g", 0, 4879, 5);
    add_operation(mainf, OP_RETURN_VOID, NULL, NULL, NULL, NULL, 0, 4884, 1);
}
