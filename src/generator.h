#ifndef MEMSCOPE_GENERATOR_H
#define MEMSCOPE_GENERATOR_H

#include "program.h"

class ProgramGenerator {
public:
    Program build(unsigned int seed) const;
};

#endif
