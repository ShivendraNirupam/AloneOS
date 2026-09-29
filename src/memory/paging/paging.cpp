#include "paging.h"
#include "heap/kheap.h"

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags) {
    uint32_t* directory = static_cast<uint32_t*>(
        kzalloc(sizeof(uint32_t) * paging::PAGING_TOTAL_ENTRIES_PER_TABLE)
    );

    for(size_t i = 0; i < paging::PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
        uint32_t* entry = static_cast<uint32_t*>(
            kzalloc(sizeof(uint32_t) * paging::PAGING_TOTAL_ENTRIES_PER_TABLE)
        );
    }
};