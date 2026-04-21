#ifndef MEMSCOPE_MEMORY_H
#define MEMSCOPE_MEMORY_H

#include "program.h"

#define MEMORY_CELL_COUNT 128
#define MEMORY_BASE_ADDRESS 4800
#define MEMORY_COLUMN_COUNT 4

typedef enum SegmentKind {
    SEG_STATIC,
    SEG_STACK,
    SEG_HEAP,
    SEG_CODE
} SegmentKind;

typedef struct Segment {
    SegmentKind kind;
    int start;
    int size;
    const char *label;
} Segment;

typedef struct MemoryCell {
    int address;
    char content[16];
    const char *annotation;
} MemoryCell;

typedef struct MemoryLayout {
    Segment static_data;
    Segment stack;
    Segment heap;
    Segment code;
    MemoryCell cells[MEMORY_CELL_COUNT];
    int cell_count;
} MemoryLayout;

void memory_init(MemoryLayout *layout);
void memory_load_program(MemoryLayout *layout, const Program *program);
void memory_print(const MemoryLayout *layout);

#endif
