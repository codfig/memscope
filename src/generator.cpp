#include "generator.h"

using namespace std;

Program ProgramGenerator::build(unsigned int seed) const {
    Program program;
    program.setProgramId(seed);

    program.addGlobal("a", 4800, 0);
    program.addGlobal("b", 4801, 0);

    Function &f = program.addFunction("f", 4864);
    f.addLocal("x", 4856, 0);
    f.addOperation(OperationKind::AssignInt, "x", "", "", "", 1, 4864, 4);
    f.addOperation(OperationKind::ReturnValue, "", "x", "", "", 0, 4868, 3);

    Function &g = program.addFunction("g", 4871);
    g.addLocal("y", 4859, 0);
    g.addOperation(OperationKind::AssignCall, "y", "", "", "f", 0, 4871, 5);
    g.addOperation(OperationKind::ReturnValue, "", "y", "", "", 0, 4876, 3);

    Function &mainFunction = program.addFunction("m4", 4879);
    mainFunction.addLocal("z", 4862, 0);
    mainFunction.addOperation(OperationKind::AssignCall, "z", "", "", "g", 0, 4879, 5);
    mainFunction.addOperation(OperationKind::ReturnVoid, "", "", "", "", 0, 4884, 1);

    return program;
}
