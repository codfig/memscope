#include "generator.h"
#include "memory.h"
#include "program.h"

#include <stdio.h>
#include <stdlib.h>

static unsigned int parse_seed(int argc, char **argv) {
    if (argc > 1) {
        return (unsigned int)strtoul(argv[1], NULL, 10);
    }
    return 1001U;
}

int main(int argc, char **argv) {
    unsigned int seed = parse_seed(argc, argv);
    Program program;
    MemoryLayout memory;

    generator_build_program(&program, seed);
    memory_init(&memory);
    memory_load_program(&memory, &program);

    printf("memscope bare-c prototype\n");
    printf("=========================\n");
    printf("Mode: non-interactive\n");
    printf("Heap allocation: disabled in this branch stage\n\n");

    program_print_source(&program);
    program_print_bytecode(&program);
    memory_print(&memory);

    return 0;
}
