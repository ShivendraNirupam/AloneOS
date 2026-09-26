#include "heap.h"
#include "status.h"
#include "memory/memory.h"

static int heap_validate_table(void* ptr, void* end, struct heap_table* table) {
    uintptr_t start_addr = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t end_addr  = reinterpret_cast<uintptr_t>(end);

    if(start_addr >= end_addr) {
        return -EINVARG;
    }

    size_t table_size = reinterpret_cast<size_t>(end_addr - start_addr);
    size_t total_blocks = table_size / ALONEOS_HEAP_BLOCK_SIZE;

    if(table->total != total_blocks) {
        return -EINVARG;
    }

    return 0;
}

static bool heap_validate_alignment(void* ptr) {
    return (reinterpret_cast<uintptr_t>(ptr) % ALONEOS_HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap* heap, void* ptr, void* end, struct heap_table* table) {
    
    int res = 0;
    
    if(heap_validate_alignment(ptr) || heap_validate_alignment(end)) {
        return -EINVARG;
    }

    memset(heap, 0, sizeof(struct heap));
    heap->saddr = ptr;
    heap->table = table;

    res = heap_validate_table(ptr, end, table);

    if(res < 0) {
        return res;
    }

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries, HEAP_BLOCK_TABLE_FREE, table_size);

    return res;
}