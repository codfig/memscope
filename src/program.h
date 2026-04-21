#ifndef MEMSCOPE_PROGRAM_H
#define MEMSCOPE_PROGRAM_H

#include <stddef.h>

#define MAX_GLOBALS 3
#define MAX_FUNCTIONS 5
#define MAX_PARAMS 3
#define MAX_LOCALS 4
#define MAX_OPERATIONS 16

typedef enum OperationKind {
    OP_ASSIGN_INT,
    OP_ASSIGN_SUM,
    OP_CALL,
    OP_ASSIGN_CALL,
    OP_RETURN_VOID,
    OP_RETURN_VALUE
} OperationKind;

typedef struct Variable {
    char name[4];
    int address;
    int value;
} Variable;

typedef struct Operation {
    OperationKind kind;
    char dest[4];
    char lhs[4];
    char rhs[4];
    char callee[4];
    int number;
    int bytecode_address;
    int bytecode_size;
} Operation;

typedef struct Function {
    char name[4];
    Variable params[MAX_PARAMS];
    int param_count;
    Variable locals[MAX_LOCALS];
    int local_count;
    Operation operations[MAX_OPERATIONS];
    int operation_count;
    int code_address;
} Function;

typedef struct Program {
    unsigned int program_id;
    Variable globals[MAX_GLOBALS];
    int global_count;
    Function functions[MAX_FUNCTIONS];
    int function_count;
} Program;

void program_print_source(const Program *program);
void program_print_bytecode(const Program *program);

#endif
