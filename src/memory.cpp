#include "memory.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

namespace {

const Function *findFunctionByName(const Program &program, const string &name) {
    for (const Function &function : program.getFunctions()) {
        if (function.getName() == name) {
            return &function;
        }
    }

    return nullptr;
}

const Function *findMainFunction(const Program &program) {
    if (const Function *mainFunction = findFunctionByName(program, "m4")) {
        return mainFunction;
    }

    const vector<Function> &functions = program.getFunctions();
    if (functions.empty()) {
        return nullptr;
    }

    return &functions.back();
}

const Operation *findPrimaryCall(const Function &function) {
    for (const Operation &operation : function.getOperations()) {
        if (operation.getKind() == OperationKind::Call ||
            operation.getKind() == OperationKind::AssignCall) {
            return &operation;
        }
    }

    return nullptr;
}

} // namespace

Segment::Segment(SegmentKind kind, int start, int size, string label)
    : kind_(kind), start_(start), size_(size), label_(move(label)) {
}

const string &Segment::getLabel() const {
    return label_;
}

MemoryCell::MemoryCell(int address, string content, string annotation)
    : address_(address), content_(move(content)), annotation_(move(annotation)) {
}

int MemoryCell::getAddress() const {
    return address_;
}

const string &MemoryCell::getContent() const {
    return content_;
}

const string &MemoryCell::getAnnotation() const {
    return annotation_;
}

void MemoryCell::write(const string &content, const string &annotation) {
    content_ = content;
    annotation_ = annotation;
}

MemoryLayout::MemoryLayout()
    : staticData_(SegmentKind::StaticData, 4800, 8, "STATIC"),
      stack_(SegmentKind::Stack, 4808, 48, "STACK"),
      heap_(SegmentKind::Heap, 4856, 8, "HEAP"),
      code_(SegmentKind::Code, 4864, 64, "CODE") {
    resetCells();
}

MemoryCell *MemoryLayout::cellAt(int address) {
    const int index = address - MEMORY_BASE_ADDRESS;
    if (index < 0 || index >= static_cast<int>(cells_.size())) {
        return nullptr;
    }

    return &cells_[index];
}

void MemoryLayout::writeCell(int address, const string &content, const string &annotation) {
    MemoryCell *cell = cellAt(address);
    if (cell != nullptr) {
        cell->write(content, annotation);
    }
}

void MemoryLayout::resetCells() {
    for (int i = 0; i < static_cast<int>(cells_.size()); ++i) {
        cells_[i] = MemoryCell(MEMORY_BASE_ADDRESS + i, "..", "");
    }
}

void MemoryLayout::loadProgram(const Program &program) {
    resetCells();

    const vector<Variable> &globals = program.getGlobals();
    for (int i = 0; i < static_cast<int>(globals.size()); ++i) {
        const string content = globals[i].getName() + "=0";
        writeCell(globals[i].getAddress(), content, i == 0 ? staticData_.getLabel() : "");
    }

    writeCell(4856, "--", heap_.getLabel());

    const Function *current = findMainFunction(program);
    int callerReturnAddress = -1;
    bool stackLabelWritten = false;

    while (current != nullptr) {
        const vector<Variable> &locals = current->getLocals();
        if (!locals.empty()) {
            int headerAddress = locals.front().getAddress() - 1;
            const string header = callerReturnAddress < 0
                                      ? "ret:-"
                                      : "ret:" + to_string(callerReturnAddress);
            writeCell(headerAddress, header, stackLabelWritten ? "" : stack_.getLabel());
            stackLabelWritten = true;
        }

        for (const Variable &local : locals) {
            writeCell(local.getAddress(), local.getName() + "=" + to_string(local.getValue()), "");
        }

        const Operation *call = findPrimaryCall(*current);
        if (call == nullptr) {
            break;
        }

        callerReturnAddress = call->getBytecodeAddress() + call->getBytecodeSize();
        current = findFunctionByName(program, call->getCallee());
    }

    bool codeLabelWritten = false;
    for (const Function &function : program.getFunctions()) {
        for (const Operation &operation : function.getOperations()) {
            const vector<string> bytes = operation.bytecodeCells();
            for (int i = 0; i < static_cast<int>(bytes.size()); ++i) {
                string annotation;
                if (!codeLabelWritten && i == 0) {
                    annotation = code_.getLabel();
                    codeLabelWritten = true;
                }
                writeCell(operation.getBytecodeAddress() + i, bytes[i], annotation);
            }
        }
    }
}

void MemoryLayout::print(ostream &out) const {
    const int rows = MEMORY_CELL_COUNT / MEMORY_COLUMN_COUNT;
    vector<string> renderedCells;
    renderedCells.reserve(cells_.size());
    size_t columnWidth = 0;

    for (const MemoryCell &cell : cells_) {
        ostringstream rendered;
        rendered << cell.getAddress() << " [" << cell.getContent() << "]";
        if (!cell.getAnnotation().empty()) {
            rendered << ' ' << cell.getAnnotation();
        }

        renderedCells.push_back(rendered.str());
        columnWidth = max(columnWidth, renderedCells.back().size());
    }

    out << "\nMemory\n";
    out << "------\n";

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < MEMORY_COLUMN_COUNT; ++column) {
            const int index = row + (rows * column);
            out << left << setw(static_cast<int>(columnWidth)) << renderedCells[index];

            if (column != MEMORY_COLUMN_COUNT - 1) {
                out << "  ";
            }
        }
        out << '\n';
    }
}
