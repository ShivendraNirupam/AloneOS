#include "kernel.h"
#include "kheap.h"
#include "heap.h"
#include "config.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init() {
    int total_table_entries = ALONEOS_HEAP_SIZE_BYTES / ALONEOS_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = reinterpret_cast<HEAP_BLOCK_TABLE_ENTRY*>(ALONEOS_HEAP_TABLE_ADDRESS);
    kernel_heap_table.total = total_table_entries;

    void* end = reinterpret_cast<void*>(ALONEOS_HEAP_ADDRESS + ALONEOS_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap,
                          reinterpret_cast<void*>(ALONEOS_HEAP_ADDRESS),
                          end,
                          &kernel_heap_table);

    if(res < 0) {
        print("Failed ot create heap\n"); // Todo: create panic
    }
}