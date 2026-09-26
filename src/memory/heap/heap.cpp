#include "heap.h"
#include "status.h"
#include "memory/memory.h"


static bool heap_validate_alignment(void* ptr) {
    return (reinterpret_cast<uintptr_t>(ptr) % ALONEOS_HEAP_BLOCK_SIZE) == 0;
}

static int heap_validate_table(void* ptr, void* end, heap_table* table) {
    uintptr_t start_addr = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t end_addr   = reinterpret_cast<uintptr_t>(end);

    if (start_addr >= end_addr) {
        return -EINVARG;
    }

    size_t heap_size    = end_addr - start_addr;  
    size_t total_blocks = heap_size / ALONEOS_HEAP_BLOCK_SIZE;

    if (table->total != total_blocks) {
        return -EINVARG;
    }

    return 0;
}


int heap_create(heap* heap, void* ptr, void* end, heap_table* table) {
    // Note the '!': reject when NOT aligned
    if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end)) {
        return -EINVARG;
    }

    int res = heap_validate_table(ptr, end, table);
    if (res < 0) {
        return res;
    }

    memset(heap, 0, sizeof(*heap));
    heap->saddr = ptr;
    heap->table = table;
    heap->first_free_hint = 0;  

    memset(table->entries, HEAP_BLOCK_TABLE_FREE,
           sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total);

    return 0;
}


static size_t heap_align_value_to_upper(size_t val) {
    size_t rem = val % ALONEOS_HEAP_BLOCK_SIZE;
    if (rem == 0) {
        return val;
    }
    return val + (ALONEOS_HEAP_BLOCK_SIZE - rem);
}

static inline uint8_t heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry) {
    return entry & 0x0F;
}

static void* heap_block_to_address(heap* heap, size_t block) {
    return static_cast<uint8_t*>(heap->saddr) + block * ALONEOS_HEAP_BLOCK_SIZE;
}

static int heap_address_to_block(heap* heap, void* address) {
    uintptr_t addr  = reinterpret_cast<uintptr_t>(address);
    uintptr_t start = reinterpret_cast<uintptr_t>(heap->saddr);

    if (addr < start) {
        return -EINVARG;
    }

    uintptr_t offset = addr - start;
    if (offset % ALONEOS_HEAP_BLOCK_SIZE != 0) {
        return -EINVARG;
    }

    size_t block = offset / ALONEOS_HEAP_BLOCK_SIZE;
    if (block >= heap->table->total) {
        return -EINVARG;
    }

    return static_cast<int>(block);
}

static int heap_get_start_block(heap* heap, size_t total_blocks) {
    heap_table* table = heap->table;

    if (total_blocks == 0 || total_blocks > table->total) {
        return -ENOMEM;
    }

    const size_t last_possible_start = table->total - total_blocks;

    size_t run_start = 0;
    size_t run_len   = 0;

    for (size_t i = heap->first_free_hint; i < table->total; i++) {
        if (heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_FREE) {
            run_len = 0;

            if (i >= last_possible_start) {
                break;
            }
            continue;
        }

        if (run_len == 0) {
            run_start = i;
        }

        if (++run_len == total_blocks) {
            return static_cast<int>(run_start);
        }
    }

    return -ENOMEM;
}

static void heap_mark_blocks_taken(heap* heap, size_t start_block, size_t total_blocks) {
    const size_t end_block = start_block + total_blocks - 1;

    for (size_t i = start_block; i <= end_block; i++) {
        HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_TAKEN;

        if (i == start_block) {
            entry |= HEAP_BLOCK_IS_FREE;
        }
        if (i != end_block) {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }

        heap->table->entries[i] = entry;
    }
}

static void heap_mark_blocks_free(heap* heap, size_t start_block) {
    heap_table* table = heap->table;

    for (size_t i = start_block; i < table->total; i++) {
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_FREE;

        if (!(entry & HEAP_BLOCK_HAS_NEXT)) {
            break;
        }
    }
}


void* heap_malloc_block(heap* heap, size_t total_blocks) {
    int start_block = heap_get_start_block(heap, total_blocks);
    if (start_block < 0) {
        return nullptr;
    }

    heap_mark_blocks_taken(heap, start_block, total_blocks);

    if (static_cast<size_t>(start_block) == heap->first_free_hint) {
        heap->first_free_hint = start_block + total_blocks;
    }

    return heap_block_to_address(heap, start_block);
}

void* heap_malloc(heap* heap, size_t size) {
    size_t aligned_size = heap_align_value_to_upper(size);
    size_t total_blocks = aligned_size / ALONEOS_HEAP_BLOCK_SIZE;
    return heap_malloc_block(heap, total_blocks);
}

void heap_free(heap* heap, void* ptr) {
    if (ptr == nullptr) {
        return;
    }

    int block = heap_address_to_block(heap, ptr);
    if (block < 0) {
        return;   
    }

    if (!(heap->table->entries[block] & HEAP_BLOCK_IS_FREE)) {
        return;
    }

    heap_mark_blocks_free(heap, block);

    if (static_cast<size_t>(block) < heap->first_free_hint) {
        heap->first_free_hint = block;
    }
}