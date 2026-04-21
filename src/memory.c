#include "memory.h"

#include <stdio.h>
#include <string.h>

static MemoryCell *cell_at(MemoryLayout *layout, int address) {
    int index = address - MEMORY_BASE_ADDRESS;
    if (index < 0 || index >= layout->cell_count) {
        return NULL;
    }
    return &layout->cells[index];
}

static void write_cell(MemoryLayout *layout, int address, const char *content, const char *annotation) {
    MemoryCell *cell = cell_at(layout, address);
    if (cell == NULL) {
        return;
    }

    snprintf(cell->content, sizeof(cell->content), "%s", content);
    cell->annotation = annotation;
}

void memory_init(MemoryLayout *layout) {
    int i;

    memset(layout, 0, sizeof(*layout));
    layout->static_data.kind = SEG_STATIC;
    layout->static_data.start = 4800;
    layout->static_data.size = 8;
    layout->static_data.label = "STATIC";

    layout->stack.kind = SEG_STACK;
    layout->stack.start = 4808;
    layout->stack.size = 48;
    layout->stack.label = "STACK";

    layout->heap.kind = SEG_HEAP;
    layout->heap.start = 4856;
    layout->heap.size = 8;
    layout->heap.label = "HEAP";

    layout->code.kind = SEG_CODE;
    layout->code.start = 4864;
    layout->code.size = 64;
    layout->code.label = "CODE";

    layout->cell_count = MEMORY_CELL_COUNT;
    for (i = 0; i < layout->cell_count; ++i) {
        layout->cells[i].address = MEMORY_BASE_ADDRESS + i;
        snprintf(layout->cells[i].content, sizeof(layout->cells[i].content), "..");
        layout->cells[i].annotation = NULL;
    }
}

void memory_load_program(MemoryLayout *layout, const Program *program) {
    int i;

    for (i = 0; i < program->global_count; ++i) {
        char content[16];
        snprintf(content, sizeof(content), "%s=0", program->globals[i].name);
        write_cell(layout, program->globals[i].address, content, (i == 0) ? layout->static_data.label : NULL);
    }

    write_cell(layout, 4808, "arg: -", layout->stack.label);
    write_cell(layout, 4809, "ret:4884", NULL);
    write_cell(layout, 4810, "z=0", NULL);
    write_cell(layout, 4811, "ret:4876", NULL);
    write_cell(layout, 4812, "y=0", NULL);
    write_cell(layout, 4813, "ret:4868", NULL);
    write_cell(layout, 4814, "x=0", NULL);
    write_cell(layout, 4856, "--", layout->heap.label);

    for (i = 0; i < program->function_count; ++i) {
        char content[16];
        snprintf(content, sizeof(content), "%s()", program->functions[i].name);
        write_cell(layout, program->functions[i].code_address, content, (i == 0) ? layout->code.label : NULL);
    }
}

void memory_print(const MemoryLayout *layout) {
    int rows = layout->cell_count / MEMORY_COLUMN_COUNT;
    int row;
    int column;

    printf("\nMemory\n");
    printf("------\n");
    for (row = 0; row < rows; ++row) {
        for (column = 0; column < MEMORY_COLUMN_COUNT; ++column) {
            int index = row + (rows * column);
            const MemoryCell *cell = &layout->cells[index];
            printf("%4d [%8s]", cell->address, cell->content);
            if (cell->annotation != NULL) {
                printf(" %-6s", cell->annotation);
            } else {
                printf("       ");
            }
            if (column != MEMORY_COLUMN_COUNT - 1) {
                printf("  ");
            }
        }
        printf("\n");
    }
}
