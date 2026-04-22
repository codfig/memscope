#ifndef MEMSCOPE_PROGRAM_H
#define MEMSCOPE_PROGRAM_H

#include <iosfwd>
#include <string>
#include <vector>

enum class OperationKind {
    AssignInt,
    AssignSum,
    Call,
    AssignCall,
    ReturnVoid,
    ReturnValue
};

class Variable {
public:
    Variable(std::string name = "", int address = 0, int value = 0);

    const std::string &getName() const;
    int getAddress() const;
    int getValue() const;
    std::string declarationText() const;

private:
    std::string name_;
    int address_;
    int value_;
};

class Operation {
public:
    Operation(OperationKind kind,
              std::string dest,
              std::string lhs,
              std::string rhs,
              std::string callee,
              int number,
              int bytecodeAddress,
              int bytecodeSize);

    int getBytecodeAddress() const;
    int getBytecodeSize() const;
    std::string sourceText() const;
    std::string bytecodeText() const;

private:
    OperationKind kind_;
    std::string dest_;
    std::string lhs_;
    std::string rhs_;
    std::string callee_;
    int number_;
    int bytecodeAddress_;
    int bytecodeSize_;
};

class Function {
public:
    Function(std::string name = "", int codeAddress = 0);

    void addLocal(const std::string &name, int address, int value = 0);
    void addOperation(OperationKind kind,
                      const std::string &dest,
                      const std::string &lhs,
                      const std::string &rhs,
                      const std::string &callee,
                      int number,
                      int bytecodeAddress,
                      int bytecodeSize);

    const std::string &getName() const;
    int getCodeAddress() const;
    const std::vector<Variable> &getLocals() const;
    const std::vector<Operation> &getOperations() const;

private:
    std::string name_;
    int codeAddress_;
    std::vector<Variable> params_;
    std::vector<Variable> locals_;
    std::vector<Operation> operations_;
};

class Program {
public:
    Program();

    void setProgramId(unsigned int programId);
    unsigned int getProgramId() const;

    void addGlobal(const std::string &name, int address, int value = 0);
    Function &addFunction(const std::string &name, int codeAddress);

    const std::vector<Variable> &getGlobals() const;
    const std::vector<Function> &getFunctions() const;

    void printSource(std::ostream &out) const;
    void printBytecode(std::ostream &out) const;

private:
    unsigned int programId_;
    std::vector<Variable> globals_;
    std::vector<Function> functions_;
};

#endif
