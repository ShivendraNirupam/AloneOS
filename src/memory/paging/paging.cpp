#include "paging.h"
#include "memory/heap/kheap.h"

extern "C" void paging_load_directory(uint32_t* directory);

static uint32_t* current_directory = 0;

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags) {
    uint32_t* directory = static_cast<uint32_t*>(
        kzalloc(sizeof(uint32_t) * paging::PAGING_TOTAL_ENTRIES_PER_TABLE)
    );

    int offset = 0;

    for(size_t i = 0; i < paging::PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
        uint32_t* entry = static_cast<uint32_t*>(
            kzalloc(sizeof(uint32_t) * paging::PAGING_TOTAL_ENTRIES_PER_TABLE)
        );
        for(size_t j = 0; j < paging::PAGING_TOTAL_ENTRIES_PER_TABLE; j++) {
            entry[j] = (offset + (j * paging::PAGING_PAGE_SIZE)) | flags;
        }

        offset += (paging::PAGING_TOTAL_ENTRIES_PER_TABLE * paging::PAGING_PAGE_SIZE);
        directory[i] = reinterpret_cast<uint32_t>(entry) | flags | paging::PAGING_IS_WRITEABLE;
    }

    struct paging_4gb_chunk* chunk_4gb = static_cast<struct paging_4gb_chunk*>(
        kzalloc(sizeof(struct paging_4gb_chunk))
    );

    chunk_4gb->directory_entry = directory;

    return chunk_4gb;
};

void paging_switch(uint32_t* directory) {
    paging_load_directory(directory);
    current_directory = directory;
}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk) {
    return chunk->directory_entry;
}