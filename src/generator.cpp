#include "generator.h"

#include <algorithm>
#include <array>
#include <random>
#include <string>
#include <vector>

using namespace std;

namespace {

constexpr int STATIC_BASE = 4800;
constexpr int STACK_BASE = 4808;
constexpr int CODE_BASE = 4864;

struct FunctionPlan {
    string name;
    string localName;
    int localAddress = 0;
    int codeAddress = 0;
};

string pickUnique(mt19937 &rng, vector<string> &pool) {
    uniform_int_distribution<int> indexDist(0, static_cast<int>(pool.size()) - 1);
    const int index = indexDist(rng);
    const string value = pool[index];
    pool.erase(pool.begin() + index);
    return value;
}

int operationSize(OperationKind kind) {
    switch (kind) {
    case OperationKind::AssignInt:
        return 4;
    case OperationKind::AssignSum:
        return 7;
    case OperationKind::Call:
        return 3;
    case OperationKind::AssignCall:
        return 5;
    case OperationKind::ReturnVoid:
        return 1;
    case OperationKind::ReturnValue:
        return 3;
    }

    return 0;
}

} // namespace

Program ProgramGenerator::build(unsigned int seed) const {
    mt19937 rng(seed);
    Program program;
    program.setProgramId(seed);

    vector<string> globalPool = {"a", "b", "c", "x", "y", "z"};
    uniform_int_distribution<int> globalCountDist(1, 3);
    const int globalCount = globalCountDist(rng);
    for (int i = 0; i < globalCount; ++i) {
        program.addGlobal(pickUnique(rng, globalPool), STATIC_BASE + i, 0);
    }

    uniform_int_distribution<int> totalFunctionDist(2, 4);
    const int totalFunctionCount = totalFunctionDist(rng);
    const int helperCount = totalFunctionCount - 1;

    vector<string> functionPool = {"f", "g", "h", "f1", "g1", "h1"};
    vector<string> localPool = {"i", "j", "k", "m", "n", "p", "q", "r", "u", "v"};
    vector<FunctionPlan> plans;
    plans.reserve(totalFunctionCount);

    for (int i = 0; i < helperCount; ++i) {
        plans.push_back({pickUnique(rng, functionPool), pickUnique(rng, localPool), 0, 0});
    }
    plans.push_back({"m4", pickUnique(rng, localPool), 0, 0});

    int nextStackAddress = STACK_BASE;
    for (int i = totalFunctionCount - 1; i >= 0; --i) {
        ++nextStackAddress; // reserve one cell for the frame header / return slot
        plans[i].localAddress = nextStackAddress++;
    }

    int nextCodeAddress = CODE_BASE;
    for (int i = 0; i < totalFunctionCount; ++i) {
        plans[i].codeAddress = nextCodeAddress;
        if (i == 0) {
            nextCodeAddress += operationSize(OperationKind::AssignInt);
            nextCodeAddress += operationSize(OperationKind::ReturnValue);
        } else if (plans[i].name == "m4") {
            nextCodeAddress += operationSize(OperationKind::AssignCall);
            nextCodeAddress += operationSize(OperationKind::ReturnVoid);
        } else {
            nextCodeAddress += operationSize(OperationKind::AssignCall);
            nextCodeAddress += operationSize(OperationKind::ReturnValue);
        }
    }

    uniform_int_distribution<int> numberDist(1, 9);
    for (int i = 0; i < totalFunctionCount; ++i) {
        Function &function = program.addFunction(plans[i].name, plans[i].codeAddress);
        function.addLocal(plans[i].localName, plans[i].localAddress, 0);

        int bytecodeAddress = plans[i].codeAddress;
        if (i == 0) {
            const int constant = numberDist(rng);
            function.addOperation(OperationKind::AssignInt,
                                  plans[i].localName,
                                  "",
                                  "",
                                  "",
                                  constant,
                                  bytecodeAddress,
                                  operationSize(OperationKind::AssignInt),
                                  plans[i].localAddress);
            bytecodeAddress += operationSize(OperationKind::AssignInt);
            function.addOperation(OperationKind::ReturnValue,
                                  "",
                                  plans[i].localName,
                                  "",
                                  "",
                                  0,
                                  bytecodeAddress,
                                  operationSize(OperationKind::ReturnValue),
                                  0,
                                  plans[i].localAddress);
            continue;
        }

        function.addOperation(OperationKind::AssignCall,
                              plans[i].localName,
                              "",
                              "",
                              plans[i - 1].name,
                              0,
                              bytecodeAddress,
                              operationSize(OperationKind::AssignCall),
                              plans[i].localAddress,
                              0,
                              0,
                              plans[i - 1].codeAddress);
        bytecodeAddress += operationSize(OperationKind::AssignCall);

        if (plans[i].name == "m4") {
            function.addOperation(OperationKind::ReturnVoid,
                                  "",
                                  "",
                                  "",
                                  "",
                                  0,
                                  bytecodeAddress,
                                  operationSize(OperationKind::ReturnVoid));
        } else {
            function.addOperation(OperationKind::ReturnValue,
                                  "",
                                  plans[i].localName,
                                  "",
                                  "",
                                  0,
                                  bytecodeAddress,
                                  operationSize(OperationKind::ReturnValue),
                                  0,
                                  plans[i].localAddress);
        }
    }

    return program;
}
