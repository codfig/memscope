#ifndef MEMSCOPE_MEMORY_H
#define MEMSCOPE_MEMORY_H

#include <array>
#include <iosfwd>
#include <string>

#include "program.h"

constexpr int MEMORY_CELL_COUNT = 128;
constexpr int MEMORY_BASE_ADDRESS = 4800;
constexpr int MEMORY_COLUMN_COUNT = 4;

enum class SegmentKind {
    StaticData,
    Stack,
    Heap,
    Code
};

class Segment {
public:
    Segment(SegmentKind kind = SegmentKind::StaticData,
            int start = 0,
            int size = 0,
            std::string label = "");

    const std::string &getLabel() const;

private:
    SegmentKind kind_;
    int start_;
    int size_;
    std::string label_;
};

class MemoryCell {
public:
    MemoryCell(int address = 0, std::string content = "..", std::string annotation = "");

    int getAddress() const;
    const std::string &getContent() const;
    const std::string &getAnnotation() const;
    void write(const std::string &content, const std::string &annotation);

private:
    int address_;
    std::string content_;
    std::string annotation_;
};

class MemoryLayout {
public:
    MemoryLayout();

    void loadProgram(const Program &program);
    void print(std::ostream &out) const;

private:
    MemoryCell *cellAt(int address);
    void writeCell(int address, const std::string &content, const std::string &annotation);
    void resetCells();

    Segment staticData_;
    Segment stack_;
    Segment heap_;
    Segment code_;
    std::array<MemoryCell, MEMORY_CELL_COUNT> cells_;
};

#endif
