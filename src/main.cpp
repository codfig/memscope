#include "generator.h"
#include "memory.h"
#include "program.h"

#include <cstdlib>
#include <iostream>

using namespace std;

static unsigned int parse_seed(int argc, char **argv) {
    if (argc > 1) {
        return static_cast<unsigned int>(strtoul(argv[1], nullptr, 10));
    }
    return 1001U;
}

int main(int argc, char **argv) {
    const unsigned int seed = parse_seed(argc, argv);
    ProgramGenerator generator;
    Program program = generator.build(seed);
    MemoryLayout memory;

    memory.loadProgram(program);

    cout << "memscope no-prompt-no-heap prototype\n";
    cout << "=======================\n";
    cout << "Language: C++17\n";
    cout << "Mode: non-interactive\n";
    cout << "Heap allocation: disabled in this branch stage\n\n";

    program.printSource(cout);
    program.printBytecode(cout);
    memory.print(cout);

    return 0;
}
