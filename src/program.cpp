#include "program.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace {

string formatAddressPart(int value) {
    ostringstream out;
    out << setw(2) << setfill('0') << value;
    return out.str();
}

vector<string> splitAddress(int address) {
    return {formatAddressPart(address / 100), formatAddressPart(address % 100)};
}

} // namespace

Variable::Variable(string name, int address, int value)
    : name_(move(name)), address_(address), value_(value) {
}

const string &Variable::getName() const {
    return name_;
}

int Variable::getAddress() const {
    return address_;
}

int Variable::getValue() const {
    return value_;
}

string Variable::declarationText() const {
    return "var " + name_;
}

Operation::Operation(OperationKind kind,
                     string dest,
                     string lhs,
                     string rhs,
                     string callee,
                     int number,
                     int bytecodeAddress,
                     int bytecodeSize,
                     int destAddress,
                     int lhsAddress,
                     int rhsAddress,
                     int calleeAddress)
    : kind_(kind),
      dest_(move(dest)),
      lhs_(move(lhs)),
      rhs_(move(rhs)),
      callee_(move(callee)),
      number_(number),
      bytecodeAddress_(bytecodeAddress),
      bytecodeSize_(bytecodeSize),
      destAddress_(destAddress),
      lhsAddress_(lhsAddress),
      rhsAddress_(rhsAddress),
      calleeAddress_(calleeAddress) {
}

OperationKind Operation::getKind() const {
    return kind_;
}

const string &Operation::getCallee() const {
    return callee_;
}

int Operation::getBytecodeAddress() const {
    return bytecodeAddress_;
}

int Operation::getBytecodeSize() const {
    return bytecodeSize_;
}

string Operation::sourceText() const {
    switch (kind_) {
    case OperationKind::AssignInt:
        return dest_ + " = " + to_string(number_);
    case OperationKind::AssignSum:
        return dest_ + " = " + lhs_ + " + " + rhs_;
    case OperationKind::Call:
        return callee_ + "()";
    case OperationKind::AssignCall:
        return dest_ + " = " + callee_ + "()";
    case OperationKind::ReturnVoid:
        return "return";
    case OperationKind::ReturnValue:
        return "return " + lhs_;
    }

    return "?";
}

string Operation::bytecodeText() const {
    const vector<string> bytes = bytecodeCells();
    if (bytes.empty()) {
        return "-";
    }

    ostringstream out;
    for (size_t i = 0; i < bytes.size(); ++i) {
        if (i != 0) {
            out << ' ';
        }
        out << bytes[i];
    }

    return out.str();
}

vector<string> Operation::bytecodeCells() const {
    switch (kind_) {
    case OperationKind::AssignInt:
        return {"01",
                splitAddress(destAddress_)[0],
                splitAddress(destAddress_)[1],
                formatAddressPart(number_)};
    case OperationKind::AssignSum:
        return {"02",
                splitAddress(destAddress_)[0],
                splitAddress(destAddress_)[1],
                splitAddress(lhsAddress_)[0],
                splitAddress(lhsAddress_)[1],
                splitAddress(rhsAddress_)[0],
                splitAddress(rhsAddress_)[1]};
    case OperationKind::Call:
        return {"03",
                splitAddress(calleeAddress_)[0],
                splitAddress(calleeAddress_)[1]};
    case OperationKind::AssignCall:
        return {"04",
                splitAddress(destAddress_)[0],
                splitAddress(destAddress_)[1],
                splitAddress(calleeAddress_)[0],
                splitAddress(calleeAddress_)[1]};
    case OperationKind::ReturnVoid:
        return {"05"};
    case OperationKind::ReturnValue:
        return {"06",
                splitAddress(lhsAddress_)[0],
                splitAddress(lhsAddress_)[1]};
    }

    return {};
}

Function::Function(string name, int codeAddress)
    : name_(move(name)), codeAddress_(codeAddress) {
}

void Function::addLocal(const string &name, int address, int value) {
    locals_.emplace_back(name, address, value);
}

void Function::addOperation(OperationKind kind,
                            const string &dest,
                            const string &lhs,
                            const string &rhs,
                            const string &callee,
                            int number,
                            int bytecodeAddress,
                            int bytecodeSize,
                            int destAddress,
                            int lhsAddress,
                            int rhsAddress,
                            int calleeAddress) {
    operations_.emplace_back(kind,
                             dest,
                             lhs,
                             rhs,
                             callee,
                             number,
                             bytecodeAddress,
                             bytecodeSize,
                             destAddress,
                             lhsAddress,
                             rhsAddress,
                             calleeAddress);
}

const string &Function::getName() const {
    return name_;
}

int Function::getCodeAddress() const {
    return codeAddress_;
}

const vector<Variable> &Function::getLocals() const {
    return locals_;
}

const vector<Operation> &Function::getOperations() const {
    return operations_;
}

Program::Program() : programId_(0) {
}

void Program::setProgramId(unsigned int programId) {
    programId_ = programId;
}

unsigned int Program::getProgramId() const {
    return programId_;
}

void Program::addGlobal(const string &name, int address, int value) {
    globals_.emplace_back(name, address, value);
}

Function &Program::addFunction(const string &name, int codeAddress) {
    functions_.emplace_back(name, codeAddress);
    return functions_.back();
}

const vector<Variable> &Program::getGlobals() const {
    return globals_;
}

const vector<Function> &Program::getFunctions() const {
    return functions_;
}

void Program::printSource(ostream &out) const {
    int line = 1;

    out << "Program (source)\n";
    out << "----------------\n";

    for (const Variable &global : globals_) {
        out << line++ << " | " << global.declarationText() << '\n';
    }

    for (const Function &function : functions_) {
        out << line++ << " | func " << function.getName() << "()\n";

        for (const Variable &local : function.getLocals()) {
            out << line++ << " |   " << local.declarationText() << '\n';
        }

        for (const Operation &operation : function.getOperations()) {
            out << line++ << " |   " << operation.sourceText() << '\n';
        }
    }

    out << "\nProgram ID: " << programId_ << "\n";
}

void Program::printBytecode(ostream &out) const {
    out << "\nProgram (bytecode)\n";
    out << "------------------\n";

    for (const Function &function : functions_) {
        out << "-> " << function.getName() << " @ " << function.getCodeAddress() << '\n';

        for (const Operation &operation : function.getOperations()) {
            out << "   " << operation.getBytecodeAddress() << " | "
                << operation.bytecodeText() << '\n';
        }
    }
}
