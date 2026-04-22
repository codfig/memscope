#include "memory.h"

#include <iostream>

using namespace std;

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

    writeCell(4808, "arg: -", stack_.getLabel());
    writeCell(4809, "ret:4884", "");
    writeCell(4810, "z=0", "");
    writeCell(4811, "ret:4876", "");
    writeCell(4812, "y=0", "");
    writeCell(4813, "ret:4868", "");
    writeCell(4814, "x=0", "");
    writeCell(4856, "--", heap_.getLabel());

    const vector<Function> &functions = program.getFunctions();
    for (int i = 0; i < static_cast<int>(functions.size()); ++i) {
        writeCell(functions[i].getCodeAddress(),
                  functions[i].getName() + "()",
                  i == 0 ? code_.getLabel() : "");
    }
}

void MemoryLayout::print(ostream &out) const {
    const int rows = MEMORY_CELL_COUNT / MEMORY_COLUMN_COUNT;

    out << "\nMemory\n";
    out << "------\n";

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < MEMORY_COLUMN_COUNT; ++column) {
            const int index = row + (rows * column);
            const MemoryCell &cell = cells_[index];

            out << cell.getAddress() << " [" << cell.getContent() << "]";
            if (!cell.getAnnotation().empty()) {
                out << ' ' << cell.getAnnotation();
            }

            if (column != MEMORY_COLUMN_COUNT - 1) {
                out << "  ";
            }
        }
        out << '\n';
    }
}
